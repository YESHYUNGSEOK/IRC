#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "CommandHandler.hpp"

class Message {
 public:
  typedef enum CMD {
    PASS,
    NICK,
    USER,
    OPER,  // 0-3
    QUIT,
    JOIN,
    PART,
    TOPIC,
    MODE,  // 4-8
    NAMES,
    LIST,
    INVITE,
    KICK,  // 9-12
    PRIVMSG,
    WHO,
    WHOIS,
    WHOWAS,  // 13-16
    KILL,
    PING,
    PONG,  // 17-19
    CAP,   // 20
    NONE   // 21
  } e_cmd;

 private:
  e_cmd _command;
  std::vector<std::string> _params;

  Message();

 public:
  Message(std::string& line);
  Message(const Message& src);
  Message& operator=(const Message& other);
  ~Message();

  class InvalidCommandException : public std::exception {
    virtual const char* what() const throw() { return "Invalid command"; }
  };

  e_cmd get_command() const;
  const std::vector<std::string>& get_params() const;
};

#endif
