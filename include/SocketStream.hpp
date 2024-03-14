#pragma once
#ifndef SOCKETSTREAM_HPP
#define SOCKETSTREAM_HPP

#include <string>
#include "ft_irc.hpp"

class SocketStream
{
private:
  const struct sockaddr_in _addr;
  const socklen_t _addr_len;
  const int _fd;
  std::string _read_buffer;
  std::string _write_buffer;

public:
  SocketStream();
  SocketStream(const int server_fd);
  SocketStream(const SocketStream &src);
  SocketStream &operator=(const SocketStream &src);
  ~SocketStream();

  int get_fd() const;

  SocketStream &operator<<(const std::string &data);
  SocketStream &operator>>(std::string &data);

  // 템플릿 함수는 가급적 사용x
  template <typename T>
  SocketStream &operator<<(const T &data)
  {
    // 데이터를 소켓에 쓰기 - partial write 처리 필요
    send(_fd, &data, sizeof(data), 0);
    return *this;
  }
  // 템플릿 함수는 가급적 사용x
  template <typename T>
  SocketStream &operator>>(T &data)
  {
    // 데이터를 소켓에서 읽기 - partial read 처리 필요
    recv(_fd, &data, sizeof(data), 0);
    return *this;
  }
};

#endif // IOHANDLER_HPP
