#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>

#include "Message.hpp"
#include "NumericReply.hpp"
#include "SocketStream.hpp"
#include "ft_irc.hpp"

#define IS_CAP_NEGOTIATED(client) (((client)._status & 0x01) == 0x01)
#define IS_IN_NEGOTIATION(client) (((client)._status & 0x02) == 0x02)
#define IS_PASS_CONFIRMED(client) (((client)._status & 0x04) == 0x04)
#define IS_NICK_SET(client) (((client)._status & 0x08) == 0x08)
#define IS_USER_SET(client) (((client)._status & 0x10) == 0x10)
#define IS_REGISTERED(client) (((client)._status & 0x1E) == 0x1E)

#define SET_CAP_NEGOTIATED(client) ((client)._status |= 0x01)
#define SET_IN_NEGOTIATION(client) ((client)._status |= 0x02)
#define SET_PASS_CONFIRMED(client) ((client)._status |= 0x04)
#define SET_NICK_SET(client) ((client)._status |= 0x08)
#define SET_USER_SET(client) ((client)._status |= 0x10)
#define SET_REGISTERED(client) ((client)._status |= 0x1E)

#define UNSET_CAP_NEGOTIATED(client) ((client)._status &= ~0x01)
#define UNSET_IN_NEGOTIATION(client) ((client)._status &= ~0x02)
#define UNSET_PASS_CONFIRMED(client) ((client)._status &= ~0x04)
#define UNSET_NICK_SET(client) ((client)._status &= ~0x08)
#define UNSET_USER_SET(client) ((client)._status &= ~0x10)
#define UNSET_REGISTERED(client) ((client)._status &= ~0x1E)

class Client {
 private:
  std::string _nickname;
  std::string _username;
  std::string _hostname;
  std::string _realname;
  SocketStream &_stream;

  Client();
  Client(const Client &src);
  Client &operator=(const Client &src);

 public:
  unsigned int _status;  // 수동으로 조작 금지
  Client(int server_fd);
  ~Client();

  // 시스템콜 관련 함수
  std::string read_buffer();  // 연산자 오버로딩으로 변경해도 될 듯
  void recv();
  void send();

  // 기능 함수
  Message get_msg();
  void start_negotiation();
  void finish_negotiation();
  void confirm_password();
  void set_user(const std::string &username, const std::string &hostname,
                const std::string &servername, const std::string &realname);
  void register_client();

  // getter, setter
  unsigned int get_status() const;  // 가능하면 private으로 변경하는게 좋을 듯
  bool is_registered() const;
  const std::string &get_nickname() const;
  const std::string &get_username() const;
  const std::string &get_realname() const;
  const std::string &get_hostname() const;
  int get_fd() const;

  void set_nickname(const std::string &nickname);
  void set_username(const std::string &username);
  void set_realname(const std::string &realname);

  // 연산자 오버로딩
  bool operator==(const Client &other);
  bool operator!=(const Client &other);
  bool operator<(const Client &other);
  bool operator>(const Client &other);
  bool operator<=(const Client &other);
  bool operator>=(const Client &other);
  Client &operator<<(const std::string &data);
  Client &operator>>(std::string &data);
  Client &operator>>(std::vector<std::string> &vec);
};

#endif  // CLIENT_HPP
