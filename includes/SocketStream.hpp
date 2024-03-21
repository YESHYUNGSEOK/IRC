#pragma once
#ifndef SOCKETSTREAM_HPP
#define SOCKETSTREAM_HPP

#include <string>

#include "ft_irc.hpp"

class SocketStream {
 private:
  static const int BUFFER_SIZE = 1024;

  const struct sockaddr_in _addr;
  const socklen_t _addr_len;
  const int _fd;
  std::string _read_buffer;
  std::string _write_buffer;
  char *_raw_read_buffer;
  char *_raw_write_buffer;

 public:
  SocketStream();
  SocketStream(const int server_fd);
  SocketStream(const SocketStream &src);
  SocketStream &operator=(const SocketStream &src);
  ~SocketStream();

  int get_fd() const;
  void recv();
  void send();

  SocketStream &operator<<(const std::string &data);
  SocketStream &operator>>(std::string &data);

  void flush_write_buffer();
};

#endif  // IOHANDLER_HPP
