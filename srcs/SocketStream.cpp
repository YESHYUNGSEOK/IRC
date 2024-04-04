#include "SocketStream.hpp"

// 사용하지 않는 생성자
SocketStream::SocketStream() : _fd(0) {
  DEBUG();
  throw std::runtime_error(
      "SocketStream::SocketStream(): Do not use this constructor");
}

// 사용하지 않는 생성자
SocketStream::SocketStream(__unused const SocketStream &src) : _fd(0) {
  DEBUG();
  throw std::runtime_error(
      "SocketStream::SocketStream(): Do not use this constructor");
}

// 사용하지 않는 연산자 - 컨테이너를 사용하면 필요할 수 있음
SocketStream &SocketStream::operator=(__unused const SocketStream &src) {
  DEBUG();
  throw std::runtime_error(
      "SocketStream::operator=(): Do not use this operator");
  return *this;
}

SocketStream::SocketStream(const int server_fd)
    : _addr(),
      _addr_len(sizeof(_addr)),
      // 클라이언트의 연결을 수락
      _fd(accept(server_fd, reinterpret_cast<struct sockaddr *>(&_addr),
                 &_addr_len)),
      _read_buffer(),
      _write_buffer() {
  _raw_buffer[0] = '\0';
  if (_fd < 0)  // 소켓 연결 실패
    throw std::runtime_error(strerror(errno));

  unsigned int opt = 1;
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {  // 소켓 재사용 허용
    close(_fd);
    throw std::runtime_error(strerror(errno));
  }

#ifdef __APPLE__                            // macOS
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)  // 소켓 논블로킹 설정
  {
    close(_fd);
    throw std::runtime_error(strerror(errno));
  }
#endif
  DEBUG_PRINT(_fd);
}

SocketStream::~SocketStream() {
  close(_fd);
  std::cout << "[SocketStream] " << _fd << " disconnected" << std::endl;
}

void SocketStream::recv() {
#ifdef __linux__  // Linux
  const ssize_t recv_len =
      ::recv(_fd, _raw_read_buffer, SOCKET_STREAM_BUFFER_SIZE, MSG_DONTWAIT);
#else  // macOS
  const ssize_t recv_len =
      ::recv(_fd, _raw_buffer, SOCKET_STREAM_BUFFER_SIZE, 0);
#endif

  if (recv_len <= 0) {  // 0일 경우 처리 결정 필요
    if (recv_len == 0) DEBUG_PRINT(recv_len);
    if ((errno == EAGAIN || errno == EWOULDBLOCK))
      return;
    else
      throw ConnectionClosedException();
  }

  _read_buffer += std::string(_raw_buffer, recv_len);
  DEBUG_PRINT(_read_buffer);
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
    if (send_len == 0) DEBUG_PRINT(send_len);
    if ((errno == EAGAIN || errno == EWOULDBLOCK))
      return;
    else
      throw ConnectionClosedException();
  }
  DEBUG_PRINT(_write_buffer);
  _write_buffer = _write_buffer.substr(send_len);
}

int SocketStream::get_fd() const { return _fd; }

SocketStream &SocketStream::operator<<(const std::string &data) {
  _write_buffer += data;

  return *this;
}
SocketStream &SocketStream::operator>>(std::string &data) {
  const std::string::size_type crlf_pos = _read_buffer.find("\r\n");
  const std::string::size_type lf_pos = _read_buffer.find("\n");

  if (crlf_pos != std::string::npos) {
    if (crlf_pos + 2 > SOCKET_STREAM_LINE_SIZE) {
      // CRLF가 있지만 메시지가 너무 길면 무시하고 예외 발생
      _read_buffer = _read_buffer.substr(crlf_pos + 2);
      throw MessageTooLongException();
    } else {
      // CRLF가 있고 메시지가 적절하면 반환하고 버퍼에서 삭제
      data = _read_buffer.substr(0, crlf_pos + 2);
      _read_buffer = _read_buffer.substr(crlf_pos + 2);
    }
  } else if (lf_pos != std::string::npos) {
    if (lf_pos + 1 > SOCKET_STREAM_LINE_SIZE) {
      // LF가 있지만 메시지가 너무 길면 무시하고 예외 발생
      _read_buffer = _read_buffer.substr(lf_pos + 1);
      throw MessageTooLongException();
    } else {
      // LF가 있고 메시지가 적절하면 반환하고 버퍼에서 삭제
      data = _read_buffer.substr(0, lf_pos + 1);
      _read_buffer = _read_buffer.substr(lf_pos + 1);
    }
  } else {
    data = "";  // CRLF나 LF가 없으면 빈 문자열 반환
  }

  return *this;
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
