#pragma once
#ifndef SOCKETSTREAM_HPP
#define SOCKETSTREAM_HPP

#include <exception>
#include <string>
#include <vector>

#include "ft_irc.hpp"

class SocketStream {
 private:
  static const int BUFFER_SIZE = 1024;
  static const int LINE_SIZE_MAX = 512;

  const struct sockaddr_in _addr;
  const socklen_t _addr_len;
  const int _fd;
  std::string _read_buffer;
  std::string _write_buffer;
  char *_raw_buffer;

  SocketStream();                         // 사용하지 않는 생성자
  SocketStream(const SocketStream &src);  // 사용하지 않는 생성자
  SocketStream &operator=(const SocketStream &src);  // 사용하지 않는 연산자

 public:
  SocketStream(const int server_fd);
  ~SocketStream();

  void recv();
  void send();

  int get_fd() const;

  SocketStream &operator<<(const std::string &data);
  SocketStream &operator>>(std::string &data);

  class ConnectionClosedException : public std::exception {
   public:
    ConnectionClosedException();
    ConnectionClosedException(const ConnectionClosedException &src);
    ConnectionClosedException &operator=(const ConnectionClosedException &src);
    virtual ~ConnectionClosedException() throw();
    virtual const char *what() const throw();
  };
  class MessageTooLongException : public std::exception {
   public:
    MessageTooLongException();
    MessageTooLongException(const MessageTooLongException &src);
    MessageTooLongException &operator=(const MessageTooLongException &src);
    virtual ~MessageTooLongException() throw();
    virtual const char *what() const throw();
  };
};

#endif  // IOHANDLER_HPP
