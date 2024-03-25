#include "Server.hpp"

Server::Server(int port, int password) : _port(port), _password(password) {
  // 소켓 생성 (IPv4, TCP)
  _server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_server_fd < 0)
    throw std::runtime_error("socket() failed: " +
                             std::string(strerror(errno)));

  _addr.sin_family = AF_INET;          // IPv4
  _addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
  _addr.sin_port = htons(_port);       // network byte order

  unsigned int opt = 1;  // 소켓 옵션 설정 - 재사용 허용
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

#ifdef __APPLE__                                   // macOS
  if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0)  // 소켓을 논블로킹으로 설정
    throw std::runtime_error("[Server::run()]: fcntl() failed: " +
                             std::string(strerror(errno)));
#endif

  // SOMAXCONN은 시스템에서 지정한 최대 연결 요청 대기 큐의 크기
  if (listen(_server_fd, SOMAXCONN) < 0)
    throw std::runtime_error("[Server::run()]: listen() failed: " +
                             std::string(strerror(errno)));
}

void Server::run() {
  while (true) {
    _read_fds = _master_fds;  // select()를 호출할 때마다 초기화
    // _write_fds = _master_fds;

    const int max_sock_fd = FD_SETSIZE;
    // _clients.empty() ? _server_fd : (*_clients.rbegin())->get_fd();

    // select()는 이벤트가 발생한 파일 디스크립터의 개수를 반환
    // int event = select(FD_SETSIZE, &_read_fds, &_write_fds, NULL, NULL);
    const int event = select(max_sock_fd, &_read_fds, NULL, NULL, NULL);

    if (event < 0)
      throw std::runtime_error("select() failed: " +
                               std::string(strerror(errno)));
    else if (event > 0) {
      std::set<Client *>::iterator it = _clients.begin();

      while (it != _clients.end()) {
        try {
          if (FD_ISSET((*it)->get_fd(), &_read_fds))  // read event
          {
            read_client(*it);
            write_client(*it);
          }
          it++;
        } catch (SocketStream::SystemCallException
                     &e) {  // 소켓 시스템 콜 예외 - 복구 불가능
          std::cerr << "Error: " << e.what() << std::endl;
          FD_CLR((*it)->get_fd(), &_master_fds);
          delete *it;
          it = _clients.erase(it);
        } catch (SocketStream::ConnectionClosedException
                     &e) {  // 클라이언트 연결 종료 예외 - 복구 불가능
          FD_CLR((*it)->get_fd(), &_master_fds);
          delete *it;
          it = _clients.erase(it);
        } catch (...) {  // 그 외 예외 - 일단 에러 메시지 출력 후 클라이언트
                         // 연결 종료
          FD_CLR((*it)->get_fd(), &_master_fds);
          delete *it;
          it = _clients.erase(it);
        }
      }
      try {
        if (FD_ISSET(_server_fd, &_read_fds))  // accept event
          accept_new_client();
      } catch (SocketStream::SystemCallException
                   &e) {  // 소켓 시스템 콜 예외 - 복구 불가능
        std::cerr << "Error: " << e.what() << std::endl;
      } catch (
          ...) {  // 그 외 예외 - 일단 에러 메시지 출력 후 클라이언트 연결 종료
        std::cerr << "Unkown Error: " << strerror(errno) << std::endl;
      }
    }
  }
}

void Server::accept_new_client() {
  Client *client = new Client(_server_fd);  // 새로운 클라이언트 생성
  FD_SET(client->get_fd(),
         &_master_fds);     // 새로운 클라이언트를 _master에 추가
  _clients.insert(client);  // 새로운 클라이언트를 _clients에 추가
}

#include <sstream>

void Server::read_client(Client *client) {
  // 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
  client->recv();

  // 클라이언트의 버퍼로부터 데이터를 읽어들임
  // 개행 단위로 처리를 위해 컨테이너로 반환하도록 구현 필요
  std::vector<std::string> messages;

  *client >> messages;

  std::vector<std::string>::iterator it = messages.begin();
  while (it != messages.end()) {
    // 메시지 처리 임시 하드코딩 - 나중에 함수로 분리
    std::stringstream ss(*it);

    std::string command;

    ss >> command;

    if (command == "CAP") {
      capability(client, ss);
    } else if (command == "PASS") {
      std::string password;
      ss >> password;
      int pass = std::stoi(password);

      if (pass == _password) client->confirm_password();
    } else if (command == "NICK") {
      std::string nickname;
      ss >> nickname;
      client->set_nickname(nickname);
    } else if (command == "USER") {
      std::string username, hostname, servername, realname;

      ss >> username >> hostname >> servername >> realname;

      if (realname[0] == ':')
        realname = realname.substr(1, realname.size() - 1);

      std::cout << "username: " << username << std::endl;
      std::cout << "hostname: " << hostname << std::endl;
      std::cout << "servername: " << servername << std::endl;
      std::cout << "realname: " << realname << std::endl;
      client->set_user(username, hostname, servername, realname);
    } else if (command == "JOIN") {
      *client << "461 * JOIN :Not enough parameters\r\n";
    } else {
      // *client << ERR_UNKNOWNCOMMAND_STR;
    }

    std::cout << "Debug: from " << client->get_fd() << " ["
              << it->substr(0, it->length() - 2) << "]" << std::endl;
    it++;
  }
}

void Server::write_client(Client *client) {
  // 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
  client->send();
}

void Server::capability(Client *client, std::stringstream &ss) {
  std::string cap_cmd;

  ss >> cap_cmd;

  if (cap_cmd == "LS") {
    std::cout << "ls" << std::endl;

    std::string cap_version;
    ss >> cap_version;

    std::cout << cap_version << std::endl;
    if (cap_version == "302") {
      // *client << "CAP * LS :multi-prefix\r\n";
      *client << "CAP * LS :\r\n";
    } else {
      // *client << ERR_UNKNOWNCAPVERSION_STR;
    }
  } else if (cap_cmd == "REQ") {
    // *client << RPL_CAP_REQ_STR;
  } else if (cap_cmd == "END") {
    client->finish_negotiation();
    // *client << RPL_CAP_END_STR;
  } else {
    // *client << ERR_UNKNOWNCAPCMD_STR;
  }
}

Server::Server() : _port(0), _password(0), _server_fd(0) {}

Server::Server(const Server &src)
    : _port(src._port), _password(src._password), _server_fd(src._server_fd) {}

Server &Server::operator=(const Server &src) {
  _port = src._port;
  _password = src._password;
  _server_fd = src._server_fd;
  return *this;
}

Server::~Server() { close(_server_fd); }

// Path: srcs/Server.cpp
