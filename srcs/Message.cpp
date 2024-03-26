#include "Message.hpp"

Message::Message(std::string &line)
    : _command(NONE), _params(std::vector<std::string>()) {
  std::string::size_type cmd_size = line.find(" ");
  if (cmd_size == std::string::npos) return;

  std::string cmd = line.substr(0, cmd_size);
  for (std::string::size_type i = 0; i < cmd.size(); i++) {
    if (!isalpha(cmd[i])) return;
    if (cmd[i] >= 'a' && cmd[i] <= 'z') cmd[i] = toupper(cmd[i]);
  }
  const std::string cmd_arr[21] = {
      "PASS",  "NICK",  "USER",   "OPER", "QUIT",   "JOIN", "PART",
      "TOPIC", "MODE",  "NAMES",  "LIST", "INVITE", "KICK", "PRIVMSG",
      "WHO",   "WHOIS", "WHOWAS", "KILL", "PING",   "PONG", "CAP"};
  for (int i = 0; i < 21; i++) {
    if (cmd == cmd_arr[i]) {
      _command = static_cast<e_cmd>(i);
      break;
    }
  }
  if (_command == NONE) return;

  line = line.substr(line.find_first_not_of(" ", cmd_size));

  std::string *colon_param = NULL;
  std::string::size_type colon_pos = line.find(":");
  if (colon_pos != std::string::npos) {
    colon_param = new std::string(line.substr(colon_pos + 1, line.size()));
    line = line.substr(0, colon_pos);
  }

  while (line.length()) {
    std::string::size_type space_pos = line.find(" ");
    if (space_pos == std::string::npos) {
      _params.push_back(line.substr(0, line.size()));
      break;
    }
    _params.push_back(line.substr(0, space_pos));
    line = line.substr(line.find_first_not_of(" ", space_pos));
  }
  if (colon_param) {
    _params.push_back(*colon_param);
    delete colon_param;
  }
  *_params.rbegin() = _params.rbegin()->substr(0, _params.rbegin()->size() - 2);
}

Message::Message(const Message &src)
    : _command(src._command), _params(src._params) {}
Message &Message::operator=(const Message &other) {
  this->_command = other._command;
  this->_params = other._params;
  return *this;
}
Message::~Message() {}

Message::e_cmd Message::get_command() const { return this->_command; }
const std::vector<std::string> &Message::get_params() const {
  return this->_params;
}
