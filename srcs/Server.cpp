#include "../includes/Server.hpp"

Server::Server(int port, std::string password)
	: _port(port), _password(password)
{
	// 소켓 생성 (IPv4, TCP)
	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

			while (it != _clients.end())
			{
				try
				{
					if (FD_ISSET((*it)->get_fd(), &_read_fds)) // read event
					{
						read_client(*it);
						write_client(*it);
					}
					it++;
				}
				catch (SocketStream::SystemCallException &e)
				{
					// 소켓 시스템 콜 예외 - 복구 불가능
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
			catch (SocketStream::SystemCallException &e)
			{
				// 소켓 시스템 콜 예외 - 복구 불가능
				std::cerr << "Error: " << e.what() << std::endl;
			}
			catch (...)
			{
				// 그 외 예외 - 일단 에러 메시지 출력 후 연결 종료
				std::cerr << "Unknown Error: " << strerror(errno) << std::endl;
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

// 클라이언트로부터 데이터를 읽어들이는 함수 - 수정중
void Server::read_client(Client *client)
{
	// 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
	client->recv();

	// 클라이언트의 버퍼로부터 데이터를 읽어들임
	// 개행 단위로 처리를 위해 컨테이너로 반환하도록 구현 필요
	std::vector<Message> messages;

	*client >> messages;

	// 대충 메시지 생성자 사용하고, 이를 채널에 전달하는 함수 호출

	std::vector<Message>::const_iterator it = messages.begin();
	for (; it != messages.end(); it++)
	{
		switch (it->get_command())
		{
		case Message::CAP:
			// capability(client, it->get_params());
			break;
		case Message::PASS:
			// confirm_password(client, it->get_params()[0]);
			break;
		case Message::NICK:
			// set_nickname(client, it->get_params()[0]);
			break;
		case Message::USER:
			// set_userinfo(client, it->get_params()[0], it->get_params()[1],
			// it->get_params()[2], it->get_params()[3]);
			break;
		case Message::JOIN:
			join_channel(client, it->get_params()[0]);
			break;
		default:
			break;
		}
		std::cout << "command: " << it->get_command() << std::endl;
		std::vector<std::string>::const_iterator it2 = it->get_params().begin();
		std::cout << "params: " << it->get_params().size() << ": ";
		for (; it2 != it->get_params().end(); it2++)
		{
			std::cout << *it2 << " ";
		}
		std::cout << std::endl;
	}
}

void Server::write_client(Client *client)
{
	// 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
	client->send();
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
	Channel *channel = 0;
	for (std::vector<std::string>::iterator it = channel_tokens.begin(); it != channel_tokens.end(); it++)
	{
		Channel *targetChnl = get_target_channel(*it);
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
			else if (keyIt != key_tokens.end() && targetChnl->get_key() == (*keyIt))
			{
				targetChnl->add_client(client);
				keyIt++;
				return;
			}
		}
	}
}

Server::Server() : _port(0), _password(""), _server_fd(0) {}

Server::Server(const Server &src)
	: _port(src._port), _password(src._password), _server_fd(src._server_fd) {}

Server &Server::operator=(const Server &src)
{
	_port = src._port;
	_password = src._password;
	_server_fd = src._server_fd;
	return *this;
}

Server::~Server() { close(_server_fd); }

// Path: srcs/Server.cpp
