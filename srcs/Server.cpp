#include "Server.hpp"

bool Server::_is_shutdown = false;

Server::Server(int port, std::string password)
    : _port(port),
      _password(password),
      _server_fd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
  // 소켓 생성 (IPv4, TCP)
  if (_server_fd < 0)
    throw std::runtime_error("socket() failed: " +
                             std::string(strerror(errno)));

  _addr.sin_family = AF_INET;         // IPv4
  _addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
  _addr.sin_port = htons(_port);      // network byte order

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
  FD_ZERO(&_write_fds);

  // 서버 소켓을 _master에 추가
  FD_SET(_server_fd, &_master_fds);

#ifdef __APPLE__                                  // macOS
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
  while (_is_shutdown == false)
  {
    _read_fds = _master_fds; // select()를 호출할 때마다 초기화
    _write_fds = _master_fds;

    int max_fd = _clients.empty() ? _server_fd : _clients.rbegin()->first;

    const int event = select(max_fd + 1, &_read_fds, &_write_fds, NULL, NULL);

    if (event < 0)
      throw std::runtime_error("select() failed: " +
                               std::string(strerror(errno)));
    else if (event > 0)
    {
      for (int fd = 0; fd <= max_fd; fd++)
      {
        // 클라이언트 소켓에서 읽을 수 있는 데이터가 있는 경우
        if (FD_ISSET(fd, &_read_fds))
        {
          if (fd == _server_fd)
            continue;

          std::map<int, Client *>::iterator it = _clients.find(fd);
          if (it == _clients.end())
            continue;

          try
          {
            read_client(it->second);
          }
          catch (std::runtime_error &e)
          {
            // 각종 시스템 콜 예외 - 복구 불가능
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
          catch (SocketStream::ConnectionClosedException &e)
          {
            // 클라이언트 연결 종료 예외 - 복구 불가능
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
          catch (...)
          {
            // 그 외 예외 - 일단 에러 메시지 출력 후 연결 종료
            std::cerr << "Unkown Error: " << strerror(errno) << std::endl;
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
        }

        // 쓰기 가능한 소켓에 대한 처리
        if (FD_ISSET(fd, &_write_fds))
        {
          if (fd == _server_fd)
            continue;

          std::map<int, Client *>::iterator it = _clients.find(fd);
          if (it == _clients.end())
            continue;

          try {
            it->second->send();
          } catch (std::runtime_error &e) {
            // 각종 시스템 콜 예외 - 복구 불가능
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
          catch (SocketStream::ConnectionClosedException &e)
          {
            // 클라이언트 연결 종료 예외 - 복구 불가능
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
          catch (...)
          {
            // 그 외 예외 - 일단 에러 메시지 출력 후 연결 종료
            std::cerr << "Unkown Error: " << strerror(errno) << std::endl;
            remove_client(it->second);
            disconnect_client(it->second);
            _clients_to_disconnect.pop_back();
          }
        }
      }

      // 연결 종료된 클라이언트 소멸
      std::vector<Client *>::iterator it = _clients_to_disconnect.begin();
      for (; it != _clients_to_disconnect.end(); it++)
        disconnect_client(*it);
      _clients_to_disconnect.clear();

      // 새로운 클라이언트 연결 수락
      if (FD_ISSET(_server_fd, &_read_fds))
      {
        try
        {
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
}

void Server::accept_new_client()
{
  Client *client = new Client(_server_fd); // 새로운 클라이언트 생성
  FD_SET(client->get_fd(),
         &_master_fds); // 새로운 클라이언트를 _master에 추가
  _clients[client->get_fd()] = client;
}

void Server::remove_client(Client *client)
{
  // 채널들에서 클라이언트 제거
  std::set<Channel *>::iterator it = _channels.begin();
  for (; it != _channels.end(); it++)
  {
    if ((*it)->is_client_in_channel(client))
      (*it)->quit(client);
    if ((*it)->empty())
    {
      delete *it;
      _channels_by_name.erase((*it)->get_name());
      _channels.erase(it);
    }
  }
  _clients.erase(client->get_fd());
  _clients_by_nick.erase(client->get_nickname());
  _clients_to_disconnect.push_back(client);
}

void Server::disconnect_client(Client *client)
{
  // fd_set에서 클라이언트 제거
  FD_CLR(client->get_fd(), &_master_fds);
  FD_CLR(client->get_fd(), &_read_fds);
  FD_CLR(client->get_fd(), &_write_fds);
  // 클라이언트 소멸자 호출 - 내부 소켓 닫힘
  delete client;
}

void Server::read_client(Client *client)
{
  // 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
  client->recv();

  std::string line;

  while (true)
  {
    try
    {
      *client >> line;

      if (line.length() == 0)
        break;
      else if ((line.length() == 1 && line[0] == '\n') ||
               (line.length() == 2 && line[0] == '\r' && line[1] == '\n'))
        continue;
      Message msg(line);
      switch (msg.get_command())
      {
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
      case Message::PING:
        PING(client, msg.get_params());
        break;
      case Message::PONG:
        PONG(client, msg.get_params());
        break;
      case Message::QUIT:
        QUIT(client, msg.get_params());
        break;
      case Message::JOIN:
        JOIN(client, msg.get_params());
        break;
      case Message::PART:
        PART(client, msg.get_params());
        break;
      case Message::TOPIC:
        TOPIC(client, msg.get_params());
        break;
      case Message::INVITE:
        INVITE(client, msg.get_params());
        break;
      case Message::PRIVMSG:
        PRIVMSG(client, msg.get_params());
        break;
      case Message::MODE:
        MODE(client, msg.get_params());
        break;
      default:
        *client << ERR_UNKNOWNCOMMAND_421(*client, msg.get_params()[0]);
        break;
      }
    }
    catch (SocketStream::MessageTooLongException &e)
    {
      *client << ERR_INPUTTOOLONG_417(*client);
    }
  }
}

void Server::write_client(Client *client)
{
  // 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
  client->send();
}

// TODO: client 안으로 이동
void Server::register_client(Client *client)
{
  // 등록되지 않고, 패스워드가 확인되고, 닉네임, 유저네임이 설정되었을 때
  if (client->is_registered() || client->is_in_negotiation() ||
      !client->is_nick_set() || !client->is_user_set())
    return;
  // PING 구현할거면 PONG 수신했는지 확인 필요
  else if (true)
  {
    client->set_registered(true);
    // PING(client); - 구현 할지 결정 필요
    *client << RPL_WELCOME_001(*client);
    *client << RPL_YOURHOST_002(*client);
    *client << RPL_CREATED_003(*client, _created_at);
    *client << RPL_MYINFO_004(*client);
    // *client << RPL_ISUPPORT_005(*client); - TODO: 구현 필요
  }
}

void Server::update_nick(Client *client, const std::string &new_nick)
{
  if (client->get_nickname() == new_nick)
    return;

  // 기존 닉네임 목록에서 제거 후 새 닉네임으로 추가
  std::map<std::string, Client *>::iterator client_it =
      _clients_by_nick.find(client->get_nickname());
  if (client_it != _clients_by_nick.end())
  {
    _clients_by_nick.erase(client_it);
    _clients_by_nick[new_nick] = client;
  }
  // 채널 목록에서 닉네임 변경
  std::set<Channel *>::iterator channel_it = _channels.begin();
  for (; channel_it != _channels.end(); channel_it++)
  {
    if ((*channel_it)->is_client_in_channel(client))
      (*channel_it)->update_client_nick(client, new_nick);
  }

  client->nick(new_nick);
}

Client *Server::find_client_by_nick(const std::string &nickname)
{
  std::map<std::string, Client *>::iterator it =
      _clients_by_nick.find(nickname);
  if (it == _clients_by_nick.end())
    return NULL;
  return it->second;
}
Channel *Server::find_channel_by_name(const std::string &channel_name)
{
  std::map<std::string, Channel *>::iterator it =
      _channels_by_name.find(channel_name);
  if (it == _channels_by_name.end())
    return NULL;
  return it->second;
}

// TODO: client 안으로 이동

void Server::CAP(Client *client, const std::vector<std::string> &params) {
  if (params.empty()) {
    *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  }
  else if (params[0] == "LS")
  {
    if (!client->is_cap_negotiated())
      client->set_in_negotiation(true);
    *client << RPL_CAP_LS(*client);
  }
  else if (params[0] == "LIST")
  {
    *client << RPL_CAP_LIST(*client);
  }
  else if (params[0] == "REQ")
  {
    if (params.size() < 2)
      *client << ERR_NEEDMOREPARAMS_461(*client);
    else
      *client << RPL_CAP_NAK(*client, params[1]);
  }
  else if (params[0] == "END")
  {
    if (client->is_in_negotiation())
    {
      client->set_in_negotiation(false);
      client->set_cap_negotiated(true);
      register_client(client);
    }
  }
  else
  {
    *client << ERR_INVALIDCAPCMD_410(*client, params[0]);
  }
}
// TODO: client 안으로 이동
void Server::PASS(Client *client, const std::vector<std::string> &params)
{
  if (params.size() == 0)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else if (params[0] == _password)
    client->set_pass_confirmed(true);
  else
    *client << ERR_PASSWDMISMATCH_464(*client);
}
// TODO: client 안으로 이동
void Server::NICK(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_pass_confirmed())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else if (params.size() == 0)
  {
    *client << ERR_NONICKNAMEGIVEN_431(*client);
  }
  else if (!Message::is_valid_nick(params[0]))
  {
    *client << ERR_ERRONEUSNICKNAME_432(*client, params[0]);
  }
  else if (find_client_by_nick(params[0]))
  {
    *client << ERR_NICKNAMEINUSE_433(*client, params[0]);
  }
  else if (client->is_registered())
  {
    std::string old_nickname = client->get_nickname();
    update_nick(client, params[0]);

    *this << RPL_BRDCAST_NICKCHANGE(*client, old_nickname);
  }
  else
  {
    update_nick(client, params[0]);

    register_client(client);
  }
}
// TODO: client 안으로 이동
void Server::USER(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_pass_confirmed())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else if (client->is_registered())
  {
    *client << ERR_ALREADYREGISTRED_462(*client);
  }
  else if (params.size() < 4)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else
  {
    client->user(params[0], params[1], params[2], params[3]);
    register_client(client);
  }
}

void Server::PING(Client *client, const std::vector<std::string> &params)
{
  if (params.size() == 0)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else
  {
    *client << RPL_PONG(*client, params[0]);
  }
}

void Server::PONG(Client *client, const std::vector<std::string> &params)
{
  (void)client;
  (void)params;
  // 대충 PING 대기중인지 확인하는 코드
  // PING 대기중이면 PONG 수신 후 PING 대기중 해제
  // 아니면 PONG 수신 후 무시
}

void Server::QUIT(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else
  {
    std::string reason = params.empty() ? "" : params[0];
    *this << RPL_QUIT(*client, reason);
    *client << RPL_ERROR(*client, "Closing Link: " + reason);
  }
}

void Server::PRIVMSG(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else if (params.size() < 2)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else
  {
    const std::vector<std::string> target_tokens =
        Message::split_tokens(params[0], ',');
    const std::string message = params[1];
    std::vector<std::string>::const_iterator targetIt = target_tokens.begin();

    while (targetIt != target_tokens.end())
    {
      if (Message::is_valid_channel_name(*targetIt))
      {
        std::map<std::string, Channel *>::iterator it =
            _channels_by_name.find(*targetIt);

        if (it == _channels_by_name.end())
          *client << ERR_NOSUCHCHANNEL_403(*client, *targetIt);
        else
          it->second->privmsg(client, message);
      }
      else
      {
        Client *target = find_client_by_nick(*targetIt);

        if (!target)
          *client << ERR_NOSUCHNICK_401(*client, *targetIt);
        else
          *target << RPL_PRIVMSG(*client, target->get_nickname(), message);
      }
      targetIt++;
    }
  }
}

void Server::JOIN(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else if (params.size() == 0)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else
  {
    const std::vector<std::string> channel_tokens =
        Message::split_tokens(params[0], ',');
    const std::vector<std::string> key_tokens =
        params.size() >= 2 ? Message::split_tokens(params[1], ',')
                           : std::vector<std::string>();

    std::vector<std::string>::const_iterator channelIt = channel_tokens.begin();
    std::vector<std::string>::const_iterator keyIt = key_tokens.begin();
    while (channelIt != channel_tokens.end())
    {
      // 채널 이름이 유효한지 확인
      if (!Message::is_valid_channel_name(*channelIt))
      {
        *client << ERR_BADCHANMASK_476(*client, *channelIt);
      }
      else
      {
        // 채널이 존재하지 않으면 새로 생성
        std::map<std::string, Channel *>::iterator it =
            _channels_by_name.find(*channelIt);
        if (it == _channels_by_name.end())
        {
          // TODO: 채널 생성제한 확인 구현해야할수도 있음
          Channel *new_channel = new Channel(*channelIt);
          _channels.insert(new_channel);
          _channels_by_name[*channelIt] = new_channel;
          new_channel->init(client);
        }
        else
        {
          it->second->join(client, keyIt != key_tokens.end() ? *keyIt : "");
        }
      }
      channelIt++;
      if (keyIt != key_tokens.end())
        keyIt++;
    }
  }
}

void Server::PART(Client *client, const std::vector<std::string> &params)
{
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
  }
  else if (params.size() < 1)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
  }
  else
  {
    const std::vector<std::string> channel_tokens =
        Message::split_tokens(params[0], ',');
    const std::string reason = params.size() >= 2 ? params[1] : "";
    std::vector<std::string>::const_iterator channelIt = channel_tokens.begin();

    while (channelIt != channel_tokens.end())
    {
      std::map<std::string, Channel *>::iterator it =
          _channels_by_name.find(*channelIt);

      if (it == _channels_by_name.end())
        *client << ERR_NOSUCHCHANNEL_403(*client, *channelIt);
      else
      {
        it->second->part(client, reason);
        if (it->second->empty())
        {
          delete it->second;
          _channels.erase(it->second);
          _channels_by_name.erase(it);
        }
      }
      channelIt++;
    }
  }
}

void Server::TOPIC(Client *client, const std::vector<std::string> &params)
{
  // 등록되지 않은 클라이언트: ERR_NOTREGISTERED
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
    return;
  }
  // 파라미터 부족: ERR_NEEDMOREPARAMS
  if (params.empty())
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  }
  // 채널이 존재하지 않음: ERR_NOSUCHCHANNEL
  Channel *targetChannel = find_channel_by_name(params[0]);
  if (targetChannel == NULL)
  {
    *client << ERR_NOSUCHCHANNEL_403(*client, params[0]);
    return;
  }
  targetChannel->topic(client, params.size() == 1 ? NULL : &params[1]);
}

void Server::INVITE(Client *client, const std::vector<std::string> &params)
{
  // 등록되지 않은 클라이언트: ERR_NOTREGISTERED
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
    return;
  }
  // 파라미터 부족: ERR_NEEDMOREPARAMS
  if (params.size() < 2)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  }
  // 대상 클라이언트가 존재하지 않음: ERR_NOSUCHNICK
  Client *targetClient = find_client_by_nick(params[0]);
  if (targetClient == NULL)
  {
    *client << ERR_NOSUCHNICK_401(*client, params[0]);
    return;
  }
  // 채널이 존재하지 않음: ERR_NOSUCHCHANNEL
  Channel *targetChannel = find_channel_by_name(params[1]);
  if (targetChannel == NULL)
  {
    *client << ERR_NOSUCHCHANNEL_403(*client, params[1]);
    return;
  }
  targetChannel->invite(client, targetClient);
}

void Server::KICK(Client *client, const std::vector<std::string> &params)
{
  // 등록되지 않은 클라이언트: ERR_NOTREGISTERED
  if (!client->is_registered())
  {
    *client << ERR_NOTREGISTERED_451(*client);
    return;
  }
  // 파라미터 부족: ERR_NEEDMOREPARAMS
  if (params.size() < 2)
  {
    *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  }
  // 채널이 존재하지 않음: ERR_NOSUCHCHANNEL
  Channel *targetChannel = find_channel_by_name(params[0]);
  if (targetChannel == NULL)
  {
    *client << ERR_NOSUCHCHANNEL_403(*client, params[0]);
    return;
  }

  const std::vector<std::string> target_tokens =
      Message::split_tokens(params[1], ',');
  targetChannel->kick(client, target_tokens, params.size() >= 3 ? params[2] : "");
};

// TODO: Channel 안으로 이동
void Server::MODE(Client *client, const std::vector<std::string> &params)
{
  if (params.size() < 2)
  {
    // *client << ERR_NEEDMOREPARAMS_461(*client);
    return;
  }
  Channel *targetChnl = find_channel_by_name(params[0]);
  if (!targetChnl)
  {
    *client << ERR_UMODEUNKNOWNFLAG_501(*client);
    // *client << ERR_CHANOPRIVSNEEDED_482(*client);
    return;
  }
  if (!targetChnl->is_operator(client))
  {
    // *client << ERR_NOTONCHANNEL_442(*client);
    return;
  }
  switch (Message::is_valid_mode_flag(params[1]))
  {
  case 0:
  {
    if (params[1][0] == '+')
      targetChnl->set_mode(Channel::INVITE_ONLY, true);
    else
      targetChnl->set_mode(Channel::INVITE_ONLY, false);
  }
  break;
  case 1:
  {
    if (params[1][0] == '+')
      targetChnl->set_mode(Channel::TOPIC_RESTRICTED, true);
    else
      targetChnl->set_mode(Channel::TOPIC_RESTRICTED, false);
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
    for (std::map<int, Client *>::iterator it = _clients.begin();
         it != _clients.end(); it++)
    {
      if (it->second->get_nickname() == params[2])
      {
        targetIt = it->second;
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

Server &Server::operator<<(const std::string &message)
{
  std::map<int, Client *>::iterator it = _clients.begin();
  for (; it != _clients.end(); it++)
    *it->second << message;
  return *this;
}

// 사용하지 않는 생성자
Server::Server() : _port(0), _password(""), _server_fd(0)
{
  DEBUG();
  throw std::runtime_error("Server(): default constructor is not allowed");
}

Server::Server(__unused const Server &src)
    : _port(0), _password(""), _server_fd(0)
{
  DEBUG();
  throw std::runtime_error("Server(): copy constructor is not allowed");
}

Server &Server::operator=(__unused const Server &src)
{
  DEBUG();
  throw std::runtime_error("Server(): operator= is not allowed");
  return *this;
}

Server::~Server() { close(_server_fd); }

// Path: srcs/Server.cpp
