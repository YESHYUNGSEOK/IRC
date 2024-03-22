#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "CommandHandler.hpp"

class Message {
 public:
  Message(std::string& chunk);
  virtual ~Message();

  bool parseMsg();

  const std::string& get_prefix() const;
  const e_cmd& get_command() const;
  const std::vector<std::string>& get_params() const;
  bool is_valid() const;

 private:
  bool _is_valid;
  std::string _chunk;
  std::string _prefix;
  e_cmd _command;
  std::vector<std::string> _params;

  bool is_valid_command(const std::string& command);
};

#endif
