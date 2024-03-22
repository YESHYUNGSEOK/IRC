#include "SocketStream.hpp"

SocketStream::SocketStream()  // 사용하지 않는 생성자
    : _addr(),
      _addr_len(0),
      _fd(0),
      _read_buffer(""),
      _write_buffer(""),
      _raw_buffer(new char[BUFFER_SIZE]) {
  std::cout << "[SocketStream] default constructer called - need to fix"
            << std::endl;
}

SocketStream::SocketStream(const SocketStream &src)  // 사용하지 않는 생성자
    : _addr(src._addr),
      _addr_len(src._addr_len),
      _fd(src._fd),
      _read_buffer(src._read_buffer),
      _write_buffer(src._write_buffer) {
  std::cout << "[SocketStream] copy constructer called - need to fix"
            << std::endl;
}

SocketStream &SocketStream::operator=(
    const SocketStream &src)  // 사용하지 않는 연산자
{
  std::cout << "[SocketStream] assign operater called - need to fix"
            << std::endl;
  if (this != &src) {
    const_cast<int &>(this->_fd) = src._fd;
    const_cast<struct sockaddr_in &>(this->_addr) = src._addr;
    const_cast<socklen_t &>(this->_addr_len) = src._addr_len;
    this->_read_buffer = src._read_buffer;
    this->_write_buffer = src._write_buffer;
  }
  return *this;
}

SocketStream::SocketStream(const int server_fd)
    : _addr(),
      _addr_len(sizeof(_addr)),
      // 클라이언트의 연결을 수락
      _fd(accept(server_fd,
                 const_cast<struct sockaddr *>(
                     reinterpret_cast<const struct sockaddr *>(&_addr)),
                 const_cast<socklen_t *>(&_addr_len))),
      _read_buffer(),
      _write_buffer(),
      _raw_buffer(new char[BUFFER_SIZE]) {
  std::cout << "[SocketStream] " << _fd << " connected" << std::endl;

  if (_fd < 0)  // 소켓 연결에 실패했을 때
  {
    delete[] _raw_buffer;
    throw SystemCallException();
  }

  unsigned int opt = 1;
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    close(_fd);
    delete[] _raw_buffer;
    throw SystemCallException();
  }

#ifdef __APPLE__                            // macOS
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)  // 소켓을 논블로킹으로 설정
  {
    close(_fd);
    delete[] _raw_buffer;
    throw SystemCallException();
  }
#endif
}

SocketStream::~SocketStream() {
  delete[] _raw_buffer;
  close(_fd);
  std::cout << "[SocketStream] " << _fd << " disconnected" << std::endl;
}

int SocketStream::get_fd() const { return _fd; }

void SocketStream::recv() {
#ifdef __linux__  // Linux
  const ssize_t recv_len =
      ::recv(_fd, _raw_read_buffer, BUFFER_SIZE, MSG_DONTWAIT);
#else  // macOS
  const ssize_t recv_len = ::recv(_fd, _raw_buffer, BUFFER_SIZE, 0);
#endif

  if (recv_len <= 0) {
    if ((errno == EAGAIN || errno == EWOULDBLOCK)) {
      return;
    } else
      throw ConnectionClosedException();
  }

  _read_buffer += std::string(_raw_buffer, recv_len);
}

void SocketStream::send() {
  if (_write_buffer.empty()) return;
#ifdef __linux__  // Linux
  const ssize_t send_len =
      ::send(_fd, _write_buffer.c_str(), _write_buffer.length(), MSG_DONTWAIT);
#else  // macOS
  const ssize_t send_len =
      ::send(_fd, _write_buffer.c_str(), _write_buffer.length(), 0);
#endif

  if (send_len <= 0) {  // 0일 경우 처리 결정 필요
    if ((errno == EAGAIN || errno == EWOULDBLOCK))
      return;
    else
      throw ConnectionClosedException();
  }

  _write_buffer = _write_buffer.substr(send_len);
}

SocketStream &SocketStream::operator<<(const std::string &data) {
  _write_buffer += data;

  return *this;
}

SocketStream &SocketStream::operator>>(std::string &data) {
  const size_t pos = _read_buffer.find("\r\n");
  if (pos == std::string::npos) {
    std::cerr << "Debug: " << _read_buffer << std::endl;
    throw NoNewlineException();
  } else if (pos + 2 > LINE_SIZE_MAX) {
    _read_buffer = _read_buffer.substr(pos + 2);
    throw MessageTooLongException();
  } else {
    data = _read_buffer.substr(0, pos + 2);
    _read_buffer = _read_buffer.substr(pos + 2);
  }

  return *this;
}

SocketStream &SocketStream::operator>>(std::vector<std::string> &data) {
  std::string msg;

  *this >> msg;
  while (!msg.empty()) {
    if (msg.length() <= LINE_SIZE_MAX) data.push_back(msg);
    *this >> msg;
  }

  return *this;
}

SocketStream::SystemCallException::SystemCallException() {}
SocketStream::SystemCallException::SystemCallException(
    __unused const SystemCallException &src) {}
SocketStream::SystemCallException &SocketStream::SystemCallException::operator=(
    __unused const SystemCallException &src) {
  return *this;
}
SocketStream::SystemCallException::~SystemCallException() throw() {}
const char *SocketStream::SystemCallException::what() const throw() {
  return "System call failed";
}

SocketStream::ConnectionClosedException::ConnectionClosedException() {}
SocketStream::ConnectionClosedException::ConnectionClosedException(
    __unused const ConnectionClosedException &src) {}
SocketStream::ConnectionClosedException &
SocketStream::ConnectionClosedException::operator=(
    __unused const ConnectionClosedException &src) {
  return *this;
}
SocketStream::ConnectionClosedException::~ConnectionClosedException() throw() {}
const char *SocketStream::ConnectionClosedException::what() const throw() {
  return "Connection closed";
}

SocketStream::NoNewlineException::NoNewlineException() {}
SocketStream::NoNewlineException::NoNewlineException(
    __unused const NoNewlineException &src) {}
SocketStream::NoNewlineException &SocketStream::NoNewlineException::operator=(
    __unused const NoNewlineException &src) {
  return *this;
}
SocketStream::NoNewlineException::~NoNewlineException() throw() {}
const char *SocketStream::NoNewlineException::what() const throw() {
  return "IRC protocol violation: no crlf";
}

SocketStream::MessageTooLongException::MessageTooLongException() {}
SocketStream::MessageTooLongException::MessageTooLongException(
    __unused const MessageTooLongException &src) {}
SocketStream::MessageTooLongException &
SocketStream::MessageTooLongException::operator=(
    __unused const MessageTooLongException &src) {
  return *this;
}
SocketStream::MessageTooLongException::~MessageTooLongException() throw() {}
const char *SocketStream::MessageTooLongException::what() const throw() {
  return "IRC protocol violation: message too long";
}
