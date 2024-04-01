#include "Client.hpp"

// 사용하지 않는 생성자
Client::Client()
    : _status(std::bitset<CLIENT_STATUS_SIZE>(0)),
      _stream(*new SocketStream(0)) {
  DEBUG();
  throw std::runtime_error("Client::Client(): Do not use this constructor");
}
// 사용하지 않는 생성자
Client::Client(const Client &src)
    : _status(src._status),
      _nickname(src._nickname),
      _stream(*new SocketStream(0)) {
  DEBUG();
  throw std::runtime_error("Client::Client(): Do not use this constructor");
}
// 사용하지 않는 대입 연산자
Client &Client::operator=(__unused const Client &src) {
  DEBUG();
  throw std::runtime_error("Client::operator=(): Do not use this operator");
  return *this;
}

Client::Client(int server_fd)
    : _status(std::bitset<CLIENT_STATUS_SIZE>(0)),
      _nickname("*"),
      _stream(*new SocketStream(server_fd)) {
  DEBUG();
}
Client::~Client() {
  delete &_stream;
  DEBUG();
}

void Client::join_channel(Channel *channel) {
  _joined_channels.push_back(channel);
}

void Client::part_channel(Channel *channel) {
  std::vector<Channel *>::iterator it = _joined_channels.begin();
  while (it != _joined_channels.end()) {
    if (*it == channel) {
      _joined_channels.erase(it);
      return;
    }
    it++;
  }
}

void Client::nick(const std::string &nickname) {
  _nickname = nickname;
  _status.set(NICK_SET, true);
}

void Client::user(const std::string &username, const std::string &hostname,
                  const std::string &servername, const std::string &realname) {
  _username = username;
  _hostname = hostname;
  _servername = servername;
  _realname = realname;
  _status.set(USER_SET, true);
}

void Client::recv() { _stream.recv(); }
void Client::send() { _stream.send(); }

const std::string &Client::get_nickname() const { return _nickname; }
const std::string &Client::get_username() const { return _username; }
const std::string &Client::get_hostname() const { return _hostname; }
const std::string &Client::get_servername() const { return _servername; }
const std::string &Client::get_realname() const { return _realname; }
int Client::get_fd() const { return _stream.get_fd(); }

bool Client::is_cap_negotiated() const { return _status.test(CAP_NEGOTIATED); }
bool Client::is_in_negotiation() const { return _status.test(IN_NEGOTIATION); }
bool Client::is_pass_confirmed() const { return _status.test(PASS_CONFIRMED); }
bool Client::is_nick_set() const { return _status.test(NICK_SET); }
bool Client::is_user_set() const { return _status.test(USER_SET); }
bool Client::is_registered() const { return _status.test(REGISTERED); }

void Client::set_cap_negotiated(bool cap_negotiated) {
  _status.set(CAP_NEGOTIATED, cap_negotiated);
}
void Client::set_in_negotiation(bool in_negotiation) {
  _status.set(IN_NEGOTIATION, in_negotiation);
}
void Client::set_pass_confirmed(bool pass_confirmed) {
  _status.set(PASS_CONFIRMED, pass_confirmed);
}
void Client::set_nick_set(bool nick_set) { _status.set(NICK_SET, nick_set); }
void Client::set_user_set(bool user_set) { _status.set(USER_SET, user_set); }
void Client::set_registered(bool registered) {
  _status.set(REGISTERED, registered);
}

bool Client::operator==(const Client &other) const {
  return _stream.get_fd() == other._stream.get_fd();
}
bool Client::operator!=(const Client &other) const {
  return _stream.get_fd() != other._stream.get_fd();
}
bool Client::operator<(const Client &other) const {
  return _stream.get_fd() < other._stream.get_fd();
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
