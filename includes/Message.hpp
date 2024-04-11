#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "ft_irc.hpp"

class Message {
 public:
  typedef enum CMD {
    NONE,  // 0
    CAP,
    PASS,
    NICK,
    USER,
    // OPER,
    QUIT,
    JOIN,
    PART,
    TOPIC,
    MODE,
    // NAMES,
    // LIST,
    INVITE,
    KICK,
    PRIVMSG,
    // WHO,
    // WHOIS,
    // WHOWAS,
    // KILL,
    PING,
    // PONG  // 21
    CMD_SIZE
  } e_cmd;

 private:
  static const std::string _command_arr[CMD_SIZE + 1];

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

  // static methods
  static std::vector<std::string> split_tokens(const std::string& str,
                                               const char delim);

  // 파라미터 검증
  static std::pair<std::string, std::string::size_type> get_first_param(
      const std::string& str, const std::string::size_type pos);
  static bool is_valid_nick(const std::string& nick);
  static bool is_valid_channel_name(const std::string& name);
  static int is_valid_mode_flag(const std::string& flag);
};

#endif
