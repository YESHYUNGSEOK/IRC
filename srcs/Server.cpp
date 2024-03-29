#include "Server.hpp"

Server::Server(int port, std::string password)
	: _port(port),
	  _password(password),
	  _server_fd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
	// 소켓 생성 (IPv4, TCP)
	if (_server_fd < 0)
		throw std::runtime_error("socket() failed: " +
								 std::string(strerror(errno)));

	_addr.sin_family = AF_INET;			// IPv4
	_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
	_addr.sin_port = htons(_port);		// network byte order

	unsigned int opt = 1; // 소켓 옵션 설정 - 재사용 허용
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt() failed: " +
								 std::string(strerror(errno)));

	// 소켓 바인딩
	if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
		throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));

	// fd_set 초기화
	FD_ZERO(&_master_fds);
	FD_ZERO(&_read_fds);
	// FD_ZERO(&_write_fds);

	// 서버 소켓을 _master에 추가
	FD_SET(_server_fd, &_master_fds);

#ifdef __APPLE__									// macOS
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
		throw std::runtime_error("[Server::run()]: fcntl() failed: " +
								 std::string(strerror(errno)));
#endif

	// SOMAXCONN은 시스템에서 지정한 최대 연결 요청 대기 큐의 크기
	if (listen(_server_fd, SOMAXCONN) < 0)
		throw std::runtime_error("[Server::run()]: listen() failed: " +
								 std::string(strerror(errno)));

  // 서버 생성 시간 저장
  time_t now = time(0);
  _created_at = asctime(gmtime(&now));
}

void Server::run()
{
	while (true)
	{
		_read_fds = _master_fds; // select()를 호출할 때마다 초기화
		// _write_fds = _master_fds;

		// const int max_sock_fd = FD_SETSIZE;

		// select()는 이벤트가 발생한 파일 디스크립터의 개수를 반환
		const int event = select(FD_SETSIZE, &_read_fds, NULL, NULL, NULL);

		if (event < 0)
			throw std::runtime_error("select() failed: " +
									 std::string(strerror(errno)));
		else if (event > 0)
		{
			std::set<Client *>::iterator it = _clients.begin();

			for (; it != _clients.end();)
			{
				try
				{
					if (FD_ISSET((*it)->get_fd(), &_read_fds)) // read event
					{
						read_client(*it);
						write_client(*it);
					}
					++it;
				}
				catch (std::runtime_error &e)
				{
					// 각종 시스템 콜 예외 - 복구 불가능
					std::cerr << "Error: " << e.what() << std::endl;
					FD_CLR((*it)->get_fd(), &_master_fds);
					delete *it;
					it = _clients.erase(it);
				}
				catch (SocketStream::ConnectionClosedException &e)
				{
					// 클라이언트 연결 종료 예외 - 복구 불가능
					FD_CLR((*it)->get_fd(), &_master_fds);
					delete *it;
					it = _clients.erase(it);
				}
				catch (...)
				{
					// 그 외 예외 - 일단 에러 메시지 출력 후 연결 종료
					FD_CLR((*it)->get_fd(), &_master_fds);
					delete *it;
					it = _clients.erase(it);
				}
			}
			try
			{
				if (FD_ISSET(_server_fd, &_read_fds)) // accept event
					accept_new_client();
			}
			catch (std::runtime_error &e)
			{
				// 시스템 콜 예외 - 복구 불가능
				std::cerr << "Error: " << e.what() << std::endl;
			}
			catch (...)
			{
				// 그 외 예외 - 일단 에러 메시지 출력 후 연결 종료
				std::cerr << "Unkown Error: " << strerror(errno) << std::endl;
			}
		}
	}
}

void Server::accept_new_client()
{
	Client *client = new Client(_server_fd); // 새로운 클라이언트 생성
	FD_SET(client->get_fd(),
		   &_master_fds);	 // 새로운 클라이언트를 _master에 추가
	_clients.insert(client); // 새로운 클라이언트를 _clients에 추가
}

void Server::read_client(Client *client) {
  // 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
  client->recv();

  std::string line;

  while (true) {
    try {
      *client >> line;

      if (line.length() == 0)
        break;
      else if (line.length() == 2)
        continue;
      Message msg(line);
      switch (msg.get_command()) {
        case Message::CAP:
          CAP(client, msg.get_params());
          break;
        case Message::PASS:
          PASS(client, msg.get_params());
          break;
        case Message::NICK:
          NICK(client, msg.get_params());
          break;
        case Message::USER:
          USER(client, msg.get_params());
          break;
        case Message::JOIN:
          join_channel(client, msg.get_params()[0]);
          break;
        case Message::PING:
          PING(client, msg.get_params());
          break;
        case Message::PONG:
          PONG(client, msg.get_params());
          break;
        default:
          *client << ERR_UNKNOWNCOMMAND_421(*client, msg.get_params()[0]);
          break;
      }
    } catch (SocketStream::MessageTooLongException &e) {
      *client << ERR_INPUTTOOLONG_417(*client);
    }
  }
}

void Server::write_client(Client *client)
{
	// 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
	client->send();
}

void Server::register_client(Client *client) {
  // 등록되지 않고, 패스워드가 확인되고, 닉네임, 유저네임이 설정되었을 때
  if (client->is_registered() || client->is_in_negotiation() ||
      !client->is_nick_set() || !client->is_user_set())
    return;
  // PING 구현할거면 PONG 수신했는지 확인 필요
  else if (true) {
    client->set_registered(true);
    // PING(client); - 구현 할지 결정 필요
    *client << RPL_WELCOME_001(*client);
    *client << RPL_YOURHOST_002(*client);
    *client << RPL_CREATED_003(*client, _created_at);
    *client << RPL_MYINFO_004(*client);
    // *client << RPL_ISUPPORT_005(*client); - 구현 필요
  }
}

bool Server::is_nick_in_use(const std::string &nickname) {
  for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end();
       it++) {
    if ((*it)->get_nickname() == nickname) return true;
  }
  return false;
}

void Server::CAP(Client *client, const std::vector<std::string> &params) {
  for (std::vector<std::string>::const_iterator it = params.begin();
       it != params.end(); it++) {
    std::cout << *it << std::endl;
  }
  if (params.size() == 0) {
    *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  } else if (params[0] == "LS") {
    if (!client->is_cap_negotiated()) client->set_in_negotiation(true);
    *client << RPL_CAP_LS(*client);
  } else if (params[0] == "LIST") {
    *client << RPL_CAP_LIST(*client);
  } else if (params[0] == "REQ") {
    if (params.size() < 2)
      *client << ERR_NEEDMOREPARAMS_461(*client);
    else
      *client << RPL_CAP_NAK(*client, params[1]);
  } else if (params[0] == "END") {
    if (client->is_in_negotiation()) {
      client->set_in_negotiation(false);
      client->set_cap_negotiated(true);
      register_client(client);
    }
  } else {
    *client << ERR_INVALIDCAPCMD_410(*client, params[0]);
  }
}
void Server::PASS(Client *client, const std::vector<std::string> &params) {
  if (params.size() == 0) {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  } else if (params[0] == _password)
    client->set_pass_confirmed(true);
  else
    *client << ERR_PASSWDMISMATCH_464(*client);
}
void Server::NICK(Client *client, const std::vector<std::string> &params) {
  if (!client->is_pass_confirmed()) {
    *client << ERR_NOTREGISTERED_451(*client);
  } else if (params.size() == 0) {
    *client << ERR_NONICKNAMEGIVEN_431(*client);
  } else if (!Message::is_valid_nick(params[0])) {
    *client << ERR_ERRONEUSNICKNAME_432(*client, params[0]);
  } else if (is_nick_in_use(params[0])) {
    *client << ERR_NICKNAMEINUSE_433(*client, params[0]);
  } else if (client->is_registered()) {
    std::string old_nickname = client->get_nickname();
    client->set_nickname(params[0]);

    std::set<Client *>::iterator it = _clients.begin();
    for (; it != _clients.end(); it++) {
      if (*it == client) continue;
      **it << RPL_BRDCAST_NICKCHANGE(*client, old_nickname);
    }
  } else {
    client->set_nickname(params[0]);
    client->set_nick_set(true);
    register_client(client);
  }
}
void Server::USER(Client *client, const std::vector<std::string> &params) {
  if (!client->is_pass_confirmed()) {
    *client << ERR_NOTREGISTERED_451(*client);
  } else if (client->is_registered()) {
    *client << ERR_ALREADYREGISTRED_462(*client);
  } else if (params.size() < 4) {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  } else {
    client->set_username(params[0]);
    client->set_hostname(params[1]);  // 서버간 통신용이지만 일단 사용
    client->set_servername(params[2]);  // 서버간 통신용이지만 일단 사용
    client->set_realname(params[3]);
    client->set_user_set(true);
    register_client(client);
  }
}

void Server::PING(Client *client, const std::vector<std::string> &params) {
  if (params.size() == 0) {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  } else {
    *client << RPL_PONG(*client, params[0]);
  }
}

void Server::PONG(Client *client, const std::vector<std::string> &params) {
  (void)client;
  (void)params;
  // 대충 PING 대기중인지 확인하는 코드
  // PING 대기중이면 PONG 수신 후 PING 대기중 해제
  // 아니면 PONG 수신 후 무시
}

std::vector<std::string> Server::split_tokens(const std::string &str,
											  char delim)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}

bool Server::check_channel_name(
	const std::vector<std::string> &channel_tokens) const
{
	for (std::vector<std::string>::const_iterator it = channel_tokens.begin();
		 it != channel_tokens.end(); it++)
	{
		// if (it->size() > 50)
		// 	return false;
		if ((*it).size() < 2 || (!(*it).rfind('#') && !(*it).rfind('&')))
			return false;
	}
	return true;
}

Channel *Server::get_target_channel(const std::string &channel_name) const
{
	for (std::set<Channel *>::iterator it = _channels.begin();
		 it != _channels.end(); it++)
	{
		if ((*it)->get_name() == channel_name)
			return *it;
	}
	return 0;
}

void Server::join_channel(Client *client, std::string msg)
{
	(void)client;
	std::vector<std::string> tokens = split_tokens(msg, ' ');

	if (tokens.size() != 2 && tokens.size() != 3)
	{
		// *client << ERR_NEEDMOREPARAMS;
		return;
	}
	std::vector<std::string> channel_tokens = split_tokens(tokens[1], ',');
	std::vector<std::string> key_tokens;	  // only if key is provided
	std::vector<std::string>::iterator keyIt; // only if key is provided
	if (tokens.size() == 3)
	{
		key_tokens = split_tokens(tokens[2], ',');
		keyIt = key_tokens.begin();
	}
	if (check_channel_name(channel_tokens) == false) // invalid channel name
	{
		//   *client << ERR_NOSUCHCHANNEL;
		return;
	}
	Channel *targetChnl = 0;
	for (std::vector<std::string>::iterator it = channel_tokens.begin();
		 it != channel_tokens.end(); it++)
	{
		targetChnl = get_target_channel(*it);
		if (!targetChnl) // channel does not exist
		{
			std::string key = "";
			if (key_tokens.empty() == false && keyIt != key_tokens.end())
			{
				key = *keyIt;
				keyIt++;
			}
			Channel *new_channel = new Channel(*it, key, false, client);
			_channels.insert(new_channel);
			new_channel->add_client(client);
			return;
		}
		// target channel exists
		if ((*it) == targetChnl->get_name())
		{
			if (targetChnl->get_mode(INVITE_ONLY) == true) // invite only
			{
				// *client << ERR_INVITEONLYCHAN;
				return;
			}
			if (targetChnl->is_channel_full() == true) // channel is full
			{
				// *client << ERR_CHANNELISFULL;
				return;
			}
			// no key
			if (key_tokens.empty() == true)
			{
				targetChnl->add_client(client);
				return;
			}
			// key exists
			if (keyIt != key_tokens.end() && targetChnl->get_key() != (*keyIt))
			{
				// *client << ERR_BADCHANNELKEY;
				return;
			}
			else if (keyIt != key_tokens.end() &&
					 targetChnl->get_key() == (*keyIt))
			{
				targetChnl->add_client(client);
				keyIt++;
				return;
			}
		}
	}
}

int Server::is_valid_option(const std::string &option)
{
	if (option.size() != 2 || option[0] != '+' || option[0] != '-')
		return -1;
	if (option == "i")
		return 0;
	else if (option == "t")
		return 1;
	else if (option == "k")
		return 2;
	else if (option == "o")
		return 3;
	else if (option == "l")
		return 4;
	return -1;
}

void Server::MODE(Client *client, const std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		// *client << ERR_NEEDMOREPARAMS_461(*client);
		return;
	}
	Channel *targetChnl = client->is_channel_operator(params[0]);
	if (!targetChnl)
	{
		// *client << ERR_CHANOPRIVSNEEDED_482(*client);
		return;
	}
	switch (is_valid_option(params[1]))
	{
	case 0:
	{
		if (params[1][0] == '+')
			targetChnl->set_channel_mode(INVITE_ONLY, true);
		else
			targetChnl->set_channel_mode(INVITE_ONLY, false);
	}
	break;
	case 1:
	{
		if (params[1][0] == '+')
			targetChnl->set_channel_mode(TOPIC_RESTRICTED, true);
		else
			targetChnl->set_channel_mode(TOPIC_RESTRICTED, false);
	}
	break;
	case 2:
	{
		if (params[1][0] == '+')
		{
			if (params.size() < 3)
			{
				// *client << ERR_NEEDMOREPARAMS_461(*client);
				return;
			}
			targetChnl->set_key(params[2]);
		}
		else
		{
			targetChnl->set_key("");
		}
	}
	break;
	case 3:
	{
		if (params.size() < 3)
		{
			// *client << ERR_NEEDMOREPARAMS_461(*client);
			return;
		}
		Client *targetIt = 0;
		for (std::set<Client *>::iterator it = _clients.begin();
			 it != _clients.end(); it++)
		{
			if ((*it)->get_nickname() == params[2])
			{
				targetIt = *it;
				break;
			}
		}
		if (!targetIt)
		{
			if (params[1][0] == '+')
				targetChnl->add_operator(targetIt);
			else
				targetChnl->remove_operator(targetIt);
		}
	}
	break;
	case 4:
	{
		if (params[1][0] == '+')
		{
			if (params.size() < 3)
			{
				// *client << ERR_NEEDMOREPARAMS_461(*client);
				return;
			}
			int max_users = std::atoi(params[2].c_str());
			if (max_users <= 0)
			{
				// *client << ERR_BADCHANMASK_476(*client);
				return;
			}
			targetChnl->set_max_clients(max_users);
		}
		else
		{
			targetChnl->set_max_clients(-1);
		}
	}
	break;
	default:
		// *client << ERR_UNKNOWNMODE_472(*client);
		break;
	}
}

// 사용하지 않는 생성자
Server::Server() : _port(0), _password(""), _server_fd(0) {
  DEBUG();
  throw std::runtime_error("Server(): default constructor is not allowed");
}

Server::Server(__unused const Server &src)
    : _port(0), _password(""), _server_fd(0) {
  DEBUG();
  throw std::runtime_error("Server(): copy constructor is not allowed");
}

Server &Server::operator=(__unused const Server &src) {
  DEBUG();
  throw std::runtime_error("Server(): operator= is not allowed");
  return *this;
}

Server::~Server() { close(_server_fd); }

// Path: srcs/Server.cpp
