#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Message.hpp"
#include "SocketStream.hpp"
#include "ft_irc.hpp"

class Client {
 private:
  bool _is_registered;
  std::string _nickname;
  std::string _username;
  SocketStream &_stream;

  Client();
  Client(const Client &src);
  Client &operator=(const Client &src);

 public:
  Client(int server_fd);
  ~Client();

  void set_register();
  bool get_registraion() const;

  void set_nickname(const std::string &nickname);
  const std::string &get_nickname() const;
  int get_fd() const;
  std::string read_buffer();  // 연산자 오버로딩으로 변경해도 될 듯
  void recv();
  void send();

  Message get_msg();

  bool operator==(const Client &other);
  bool operator!=(const Client &other);
  bool operator<(const Client &other);
  bool operator>(const Client &other);
  bool operator<=(const Client &other);
  bool operator>=(const Client &other);
  Client &operator<<(const std::string &data);
  Client &operator>>(std::string &data);
};

#endif  // CLIENT_HPP
