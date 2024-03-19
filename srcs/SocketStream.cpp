#include "SocketStream.hpp"

SocketStream::SocketStream(const int server_fd)
    : _addr(),
      _addr_len(sizeof(_addr)),
      _fd(accept(server_fd,
                 const_cast<struct sockaddr *>(reinterpret_cast<const struct sockaddr *>(&_addr)),
                 const_cast<socklen_t *>(&_addr_len))),
      _read_buffer(),
      _write_buffer(),
      _raw_read_buffer(new char[BUFFER_SIZE]),
      _raw_write_buffer(new char[BUFFER_SIZE])
{

  std::cout << "[SocketStream] " << _fd << " connected" << std::endl;
  // 클라이언트의 연결을 수락
  if (_fd < 0) // 소켓 연결에 실패했을 때
    throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
    throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
}

int SocketStream::get_fd() const
{
  return _fd;
}

ssize_t SocketStream::recv()
{
  ssize_t recv_len = ::recv(_fd, _raw_read_buffer, BUFFER_SIZE, 0);
  if (recv_len < 0)
  {
    if (errno == EWOULDBLOCK)
      return 0;
    else
      throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
  }
  else if (recv_len == 0)
  {
    std::cout << "[SocketStream] " << _fd << " disconnected" << std::endl;
    return 0;
  }
  _read_buffer += std::string(_raw_read_buffer, recv_len);
  return recv_len;
}

ssize_t SocketStream::send()
{
  ssize_t send_len = ::send(_fd, _write_buffer.c_str(), _write_buffer.length(), 0);
  if (send_len < 0)
  {
    if (errno == EWOULDBLOCK)
      return 0;
    else
      throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
  }
  _write_buffer = _write_buffer.substr(send_len);
  return send_len;
}

SocketStream &SocketStream::operator<<(const std::string &data)
{
  _write_buffer += data;

  return *this;
}

SocketStream &SocketStream::operator>>(std::string &data)
{
  data = _read_buffer;
  _read_buffer.clear();

  return *this;
}

// 사용하지 않는 생성자
SocketStream::SocketStream()
    : _addr(),
      _addr_len(0),
      _fd(0),
      _read_buffer(""),
      _write_buffer(""),
      _raw_read_buffer(new char[BUFFER_SIZE]),
      _raw_write_buffer(new char[BUFFER_SIZE])
{
  _raw_read_buffer[0] = '\0';
  _raw_write_buffer[0] = '\0';

  std::cout << "[SocketStream] default constructer called - need to fix" << std::endl;
}
// 사용하지 않는 생성자
SocketStream::SocketStream(const SocketStream &src)
    : _addr(src._addr),
      _addr_len(src._addr_len),
      _fd(src._fd),
      _read_buffer(src._read_buffer),
      _write_buffer(src._write_buffer)
{
  std::cout << "[SocketStream] copy constructer called - need to fix" << std::endl;
}
// 사용하지 않는 연산자
SocketStream &SocketStream::operator=(const SocketStream &src)
{
  std::cout << "[SocketStream] assign operater called - need to fix" << std::endl;
  if (this != &src)
  {
    const_cast<int &>(this->_fd) = src._fd;
    const_cast<struct sockaddr_in &>(this->_addr) = src._addr;
    const_cast<socklen_t &>(this->_addr_len) = src._addr_len;
    this->_read_buffer = src._read_buffer;
    this->_write_buffer = src._write_buffer;
  }
  return *this;
}

SocketStream::~SocketStream()
{
  delete[] _raw_read_buffer;
  delete[] _raw_write_buffer;
  close(_fd);
  std::cout << "[SocketStream] " << _fd << " disconnected" << std::endl;
}
