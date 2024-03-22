#include "Client.hpp"

Client::Client(int server_fd)
    : _is_registered(false), _stream(*new SocketStream(server_fd)) {
  std::cout << "[Client] " << _stream.get_fd() << " connected" << std::endl;
}

Client::~Client() {
  std::cout << "[Client] " << _stream.get_fd() << " disconnected" << std::endl;
  delete &_stream;
}

void Client::set_register() { _is_registered = true; }
bool Client::get_registraion() const { return _is_registered; }
void Client::set_nickname(const std::string &nickname) { _nickname = nickname; }
const std::string &Client::get_nickname() const { return _nickname; }

int Client::get_fd() const { return _stream.get_fd(); }

std::string Client::read_buffer() {
  std::string msg;
  _stream >> msg;

  return msg;
}

void Client::recv() { _stream.recv(); }

void Client::send() { _stream.send(); }

Client::Client()  // 사용하지 않는 생성자
    : _is_registered(false), _stream(*new SocketStream(0)) {
  std::cout << "[Client] default constructer called - need to fix" << std::endl;
}

Client::Client(const Client &src)  // 사용하지 않는 복사 생성자
    : _is_registered(src._is_registered),
      _nickname(src._nickname),
      _stream(*new SocketStream(0)) {
  std::cout << "[Client] copy constructer called - need to fix" << std::endl;
}

Client &Client::operator=(const Client &src)  // 사용하지 않는 대입 연산자
{
  std::cout << "[Client] assign operater called - need to fix" << std::endl;
  _is_registered = src._is_registered;
  _nickname = src._nickname;
  // _stream = *new SocketStream(0);
  return *this;
}

Message Client::get_msg() {
  std::string msg;
  _stream >> msg;
  return Message(msg);
}

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

// Path: srcs/SocketStream.cpp
