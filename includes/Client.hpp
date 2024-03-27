#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>

#include "Channel.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "SocketStream.hpp"
#include "ft_irc.hpp"

class Channel;
class Message;
class SocketStream;

#define IS_CAP_NEGOTIATED(client) (((client)._status & 0x01) == 0x01)
#define IS_IN_NEGOTIATION(client) (((client)._status & 0x02) == 0x02)
#define IS_PASS_CONFIRMED(client) (((client)._status & 0x04) == 0x04)
#define IS_NICK_SET(client) (((client)._status & 0x08) == 0x08)
#define IS_USER_SET(client) (((client)._status & 0x10) == 0x10)
#define IS_REGISTERED(client) (((client)._status & 0x20) == 0x20)

#define SET_CAP_NEGOTIATED(client) ((client)._status |= 0x01)
#define SET_IN_NEGOTIATION(client) ((client)._status |= 0x02)
#define SET_PASS_CONFIRMED(client) ((client)._status |= 0x04)
#define SET_NICK_SET(client) ((client)._status |= 0x08)
#define SET_USER_SET(client) ((client)._status |= 0x10)
#define SET_REGISTERED(client) ((client)._status |= 0x20)

#define UNSET_CAP_NEGOTIATED(client) ((client)._status &= ~0x01)
#define UNSET_IN_NEGOTIATION(client) ((client)._status &= ~0x02)
#define UNSET_PASS_CONFIRMED(client) ((client)._status &= ~0x04)
#define UNSET_NICK_SET(client) ((client)._status &= ~0x08)
#define UNSET_USER_SET(client) ((client)._status &= ~0x10)
#define UNSET_REGISTERED(client) ((client)._status &= ~0x20)

class Client {
 private:
  std::string _nickname;
  std::string _username;
  std::string _hostname;
  std::string _servername;
  std::string _realname;
  SocketStream &_stream;
  std::vector<Channel *> _channels;

  Client();                              // 사용하지 않는 생성자
  Client(const Client &src);             // 사용하지 않는 생성자
  Client &operator=(const Client &src);  // 사용하지 않는 연산자

 public:
  unsigned int _status;  // 수동으로 조작 금지
  Client(int server_fd);
  ~Client();

  // 소켓 입출력
  void recv();
  void send();

  // 커맨드 처리
  void set_register();
  void set_userinfo(const std::string &username, const std::string &hostname,
                    const std::string &servername, const std::string &realname);
  Message get_msg();
  void start_negotiation();
  void finish_negotiation();
  void confirm_password();
  void register_client();

  // getter, setter
  const std::string &get_nickname() const;
  const std::string &get_username() const;
  const std::string &get_hostname() const;
  const std::string &get_servername() const;
  const std::string &get_realname() const;
  int get_fd() const;

  void set_nickname(const std::string &nickname);
  void set_username(const std::string &username);
  void set_hostname(const std::string &hostname);
  void set_servername(const std::string &servername);
  void set_realname(const std::string &realname);

  std::vector<Channel *> &get_channels();
  void join_channel(Channel *channel);
  void part_channel(Channel *channel);

  // 연산자 오버로딩
  bool operator==(const Client &other);
  bool operator!=(const Client &other);
  bool operator<(const Client &other);
  bool operator>(const Client &other);
  bool operator<=(const Client &other);
  bool operator>=(const Client &other);
  Client &operator<<(const std::string &data);
  Client &operator>>(std::string &data);
  Client &operator>>(std::vector<Message> &vec);
};

#endif  // CLIENT_HPP
