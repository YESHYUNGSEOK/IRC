#include "SocketStream.hpp"

SocketStream::SocketStream(const int server_fd)
    : _addr(),
      _addr_len(sizeof(_addr)),
      _fd(accept(server_fd,
                 const_cast<struct sockaddr *>(reinterpret_cast<const struct sockaddr *>(&_addr)),
                 const_cast<socklen_t *>(&_addr_len)))
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

SocketStream &SocketStream::operator<<(const std::string &data)
{
  // 데이터를 소켓에 쓰기 - partial write 처리 필요
  send(_fd, data.c_str(), data.length(), 0);
  return *this;
}
SocketStream &SocketStream::operator>>(std::string &data)
{
  char buffer[1024];
  const int size = recv(_fd, buffer, sizeof(buffer), 0);
  if (size < 0)
    throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
  else if (size == 0)
    // 연결이 끊어졌을 때의 처리
    data = "";
  else
  {
    buffer[size] = '\0';
    data = buffer;
  }
  return *this;
}

// 사용하지 않는 생성자
SocketStream::SocketStream()
    : _addr(),
      _addr_len(0),
      _fd(0),
      _read_buffer(""),
      _write_buffer("")
{
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
  std::cout << "[SocketStream] " << _fd << " disconnected" << std::endl;
  close(_fd);
}
