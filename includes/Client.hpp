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
  std::string _hostname;
  std::string _servername;
  std::string _realname;
  SocketStream &_stream;

  Client();
  Client(const Client &src);
  Client &operator=(const Client &src);

 public:
  Client(int server_fd);
  ~Client();

  bool get_registraion() const;
  int get_fd() const;
  const std::string &get_nickname() const;
  const std::string &get_username() const;

  void set_register();
  void set_nickname(const std::string &nickname);
  void set_userinfo(const std::string &username, const std::string &hostname, const std::string &servername, const std::string &realname);
  
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
