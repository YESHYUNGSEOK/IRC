#include "Client.hpp"

Client::Client(int server_fd)
    : _status(0), _stream(*new SocketStream(server_fd)) {
  std::cout << "[Client] " << _stream.get_fd() << " connected" << std::endl;
}
Client::~Client() {
  std::cout << "[Client] " << _stream.get_fd() << " disconnected" << std::endl;
  delete &_stream;
}

std::string Client::read_buffer() {
  std::string msg;
  _stream >> msg;

  return msg;
}
void Client::recv() { _stream.recv(); }
void Client::send() { _stream.send(); }

Message Client::get_msg() {
  std::string msg;
  _stream >> msg;
  return Message(msg);
}
void Client::start_negotiation() {
  if (IS_CAP_NEGOTIATED(_status) || IS_REGISTERED(_status)) return;
  SET_IN_NEGOTIATION(_status);
  std::cout << "[Client] " << _stream.get_fd() << " start negotiation"
            << std::endl;
}
void Client::finish_negotiation() {
  if (IS_REGISTERED(_status) || (!IS_IN_NEGOTIATION(_status))) return;
  UNSET_IN_NEGOTIATION(_status);
  SET_CAP_NEGOTIATED(_status);
  std::cout << "[Client] " << _stream.get_fd() << " finish negotiation"
            << std::endl;
  register_client();
}
void Client::confirm_password() {
  SET_PASS_CONFIRMED(_status);
  std::cout << "[Client] " << _stream.get_fd() << " confirm password"
            << std::endl;
  register_client();
}
void Client::set_user(const std::string &username,
                      __unused const std::string &hostname,
                      __unused const std::string &servername,
                      const std::string &realname) {
  if ((!IS_PASS_CONFIRMED(_status))) return;
  _username = username;
  _realname = realname;
  SET_USER_SET(_status);
  std::cout << "[Client] " << _stream.get_fd() << " set user" << std::endl;
  register_client();
}
void Client::register_client() {
  if (IS_REGISTERED(_status) || (!IS_PASS_CONFIRMED(_status)) ||
      (!IS_NICK_SET(_status)) || (!IS_USER_SET(_status)) ||
      IS_IN_NEGOTIATION(_status)) {
    std::cout << "[Client] " << _stream.get_fd() << " not registered"
              << std::endl;
    std::cout << "status: " << _status << std::endl;
    return;
  }
  SET_REGISTERED(_status);
  _stream << ":localhost 001 " << _nickname
          << " :Welcome to the FT_IRC Network :" << _nickname << "!"
          << _username << "@localhost"
          << "\r\n";

  std::cout << "[Client] " << _stream.get_fd() << " registered" << std::endl;
}

unsigned int Client::get_status() const { return _status; }
bool Client::is_registered() const { return IS_REGISTERED(_status); }
const std::string &Client::get_nickname() const { return _nickname; }
void Client::set_nickname(const std::string &nickname) {
  SET_NICK_SET(_status);
  _nickname = nickname;
  register_client();
}
int Client::get_fd() const { return _stream.get_fd(); }

bool Client::operator==(const Client &other) {
  return _stream.get_fd() == other._stream.get_fd();
}
bool Client::operator!=(const Client &other) {
  return _stream.get_fd() != other._stream.get_fd();
}
bool Client::operator<(const Client &other) {
  return _stream.get_fd() < other._stream.get_fd();
}
bool Client::operator>(const Client &other) {
  return _stream.get_fd() > other._stream.get_fd();
}
bool Client::operator<=(const Client &other) {
  return _stream.get_fd() <= other._stream.get_fd();
}
bool Client::operator>=(const Client &other) {
  return _stream.get_fd() >= other._stream.get_fd();
}
Client &Client::operator<<(const std::string &data) {
  _stream << data;
  return *this;
}
Client &Client::operator>>(std::string &data) {
  _stream >> data;
  return *this;
}
Client &Client::operator>>(std::vector<std::string> &vec) {
  while (true) {
    try {
      std::string msg = read_buffer();

      vec.push_back(msg);
    } catch (SocketStream::NoNewlineException &e) {
      break;
    } catch (SocketStream::MessageTooLongException &e) {
      // *this << ERR_MSGTOOLONG_STR;
    }
  }

  return *this;
}

Client::Client()  // 사용하지 않는 생성자
    : _status(0), _stream(*new SocketStream(0)) {
  std::cout << "[Client] default constructer called - need to fix" << std::endl;
}

Client::Client(const Client &src)  // 사용하지 않는 복사 생성자
    : _status(0), _nickname(src._nickname), _stream(*new SocketStream(0)) {
  std::cout << "[Client] copy constructer called - need to fix" << std::endl;
}

Client &Client::operator=(const Client &src)  // 사용하지 않는 대입 연산자
{
  std::cout << "[Client] assign operater called - need to fix" << std::endl;
  _status = src._status;
  _nickname = src._nickname;
  // _stream = *new SocketStream(0);
  return *this;
}

// Path: srcs/SocketStream.cpp
