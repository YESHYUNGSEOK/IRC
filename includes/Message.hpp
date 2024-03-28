#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "CommandHandler.hpp"

class Message {
 public:
  typedef enum CMD {
    NONE,  // 0
    CAP,
    PASS,
    NICK,
    USER,
    OPER,
    QUIT,
    JOIN,
    PART,
    TOPIC,
    MODE,
    NAMES,
    LIST,
    INVITE,
    KICK,
    PRIVMSG,
    WHO,
    WHOIS,
    WHOWAS,
    KILL,
    PING,
    PONG  // 21
  } e_cmd;

 private:
  static const std::string _command_arr[22];

  e_cmd _command;
  std::vector<std::string> _params;

  Message();

 public:
  Message(std::string& line);
  Message(const Message& src);
  Message& operator=(const Message& other);
  ~Message();

  e_cmd get_command() const;
  const std::vector<std::string>& get_params() const;

  // 파라미터 검증
  static bool is_valid_nick(const std::string& nick);
};

#endif
