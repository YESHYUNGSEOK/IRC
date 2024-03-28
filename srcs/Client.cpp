#include "Client.hpp"

Client::Client(int server_fd)
    : _nickname("*"), _stream(*new SocketStream(server_fd)), _status(0) {
  DEBUG();
}
Client::~Client() {
  delete &_stream;
  DEBUG();
}

void Client::recv() { _stream.recv(); }
void Client::send() { _stream.send(); }

const std::string &Client::get_nickname() const { return _nickname; }
const std::string &Client::get_username() const { return _username; }
const std::string &Client::get_hostname() const { return _hostname; }
const std::string &Client::get_servername() const { return _servername; }
const std::string &Client::get_realname() const { return _realname; }
int Client::get_fd() const { return _stream.get_fd(); }

const Channel * Client::is_channel_operator() const {
  if (_channels.empty() == true) return 0;
  std::vector<Channel *>::const_iterator it = _channels.rbegin();
  for (std::set<Client *>::const_iterator opIt = (*it)->get_operators().begin();
       opIt != (*it)->get_operators().end(); opIt++) {
    if (*opIt == this) return *it;
  }
  return 0;
}


void Client::set_nickname(const std::string &nickname) { _nickname = nickname; }
void Client::set_username(const std::string &username) { _username = username; }
void Client::set_hostname(const std::string &hostname) { _hostname = hostname; }
void Client::set_servername(const std::string &servername) {
  _servername = servername;
}
void Client::set_realname(const std::string &realname) { _realname = realname; }

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
Client &Client::operator>>(std::vector<Message> &vec) {
  while (true) {
    try {
      std::string msg;

      _stream >> msg;
      if (msg.length() == 0) break;
      Message m(msg);
      vec.push_back(m);
    } catch (SocketStream::MessageTooLongException &e) {
      // *this << ERR_MSGTOOLONG_STR;
    }
  }

  return *this;
}

Client::Client()  // 사용하지 않는 생성자
    : _stream(*new SocketStream(0)), _status(0) {
  DEBUG();
}

Client::Client(const Client &src)  // 사용하지 않는 복사 생성자
    : _nickname(src._nickname),
      _stream(*new SocketStream(0)),
      _status(src._status) {
  DEBUG();
}

Client &Client::operator=(
    __unused const Client &src)  // 사용하지 않는 대입 연산자
{
  DEBUG();
  return *this;
}

std::vector<Channel *> &Client::get_channels() { return _channels; }

void Client::join_channel(Channel *channel) { _channels.push_back(channel); }

void Client::part_channel(Channel *channel) {
  std::vector<Channel *>::iterator it = _channels.begin();
  while (it != _channels.end()) {
    if (*it == channel) {
      it = _channels.erase(it);
      break;
    }
    it++;
  }
}

// Path: srcs/SocketStream.cpp
