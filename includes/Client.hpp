#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <bitset>
#include <vector>

// #include "Channel.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "SocketStream.hpp"
#include "ft_irc.hpp"

#ifndef CLIENT_STATUS_SIZE
#define CLIENT_STATUS_SIZE 10
#endif

class Channel;
class Message;
class SocketStream;

class Client {
 private:
  std::bitset<CLIENT_STATUS_SIZE> _status;
  std::string _nickname;
  std::string _username;
  std::string _hostname;
  std::string _servername;
  std::string _realname;
  std::vector<Channel *> _joined_channels;
  SocketStream &_stream;

  // 사용 X
  Client();                            // 사용하지 않는 생성자
  Client(__unused const Client &src);  // 사용하지 않는 생성자
  Client &operator=(__unused const Client &src);  // 사용하지 않는 연산자

 public:
  enum Status {
    CAP_NEGOTIATED = 0,
    IN_NEGOTIATION,
    PASS_CONFIRMED,
    NICK_SET,
    USER_SET,
    REGISTERED
  };

  Client(int server_fd);
  ~Client();

  // join, part channel
  void nick(const std::string &nickname);
  void user(const std::string &username, const std::string &hostname,
            const std::string &servername, const std::string &realname);
  void join_channel(Channel *channel);
  void part_channel(Channel *channel);

  // mode
  void mode();
  void mode(const std::string &param);

  // 소켓 입출력
  void recv();
  void send();

  // getter, setter
  const std::string &get_nickname() const;
  const std::string &get_username() const;
  const std::string &get_hostname() const;
  const std::string &get_servername() const;
  const std::string &get_realname() const;
  int get_fd() const;

  // status bitset
  bool is_cap_negotiated() const;
  bool is_in_negotiation() const;
  bool is_pass_confirmed() const;
  bool is_nick_set() const;
  bool is_user_set() const;
  bool is_registered() const;

  void set_cap_negotiated(bool cap_negotiated);
  void set_in_negotiation(bool in_negotiation);
  void set_pass_confirmed(bool pass_confirmed);
  void set_nick_set(bool nick_set);
  void set_user_set(bool user_set);
  void set_registered(bool registered);

  // 연산자 오버로딩
  bool operator==(const Client &other) const;
  bool operator!=(const Client &other) const;
  bool operator<(const Client &other) const;
  Client &operator<<(const std::string &data);
  Client &operator>>(std::string &data);
};

#endif  // CLIENT_HPP
