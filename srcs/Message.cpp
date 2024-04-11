#include "Message.hpp"

const std::string Message::_command_arr[Message::CMD_SIZE + 1] = {
    "NONE", "CAP", "PASS", "NICK", "USER",
    // "OPER",
    "QUIT", "JOIN", "PART", "TOPIC", "MODE",
    //  "NAMES", "LIST",
    "INVITE", "KICK", "PRIVMSG",
    // "WHO", "WHOIS", "WHOWAS", "KILL",
    "PING",
    // "PONG"
};

Message::Message(std::string &line)
    : _command(NONE), _params(std::vector<std::string>()) {
  // 중간에 \r이 있을 경우 오류
  if (line.find('\r') != std::string::npos) {
    _command = NONE;
    return;
  }

  std::string::size_type colon_pos = line.find(':');

  std::stringstream line_stream(line);
  std::string prev_param;

  std::getline(line_stream, prev_param, ':');

  std::stringstream prev_stream(prev_param);
  std::string cmd;

  std::getline(prev_stream, cmd, ' ');

  // 커맨드 추출 - 알파벳, 숫자로 이루어진 문자열만 추출
  for (std::string::size_type i = 0; i < cmd.size(); i++) {
    if (!std::isalnum(cmd.at(i))) {
      _command = NONE;
      return;
    }
    cmd[i] = std::toupper(cmd[i]);
  }

  // 커맨드 종류 판별
  for (int i = 1; i < Message::CMD_SIZE; i++) {
    if (cmd == _command_arr[i]) {
      _command = static_cast<e_cmd>(i);
      break;
    }
  }

  // 커맨드가 잘못된 경우
  if (_command == NONE) {
    _params.push_back(cmd);
    return;
  }

  while (prev_stream) {
    std::string param;
    std::getline(prev_stream, param, ' ');
    if (param.empty()) break;
    _params.push_back(param);
  }

  if (colon_pos != std::string::npos) {
    std::string last_param;
    std::getline(line_stream, last_param);
    _params.push_back(last_param);
  }
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

std::vector<std::string> Message::split_tokens(const std::string &str,
                                               char delim) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);
  while (std::getline(tokenStream, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::pair<std::string, std::string::size_type> Message::get_first_param(
    const std::string &str, const std::string::size_type pos) {
  std::string::size_type param_end = str.find_first_of(" \t", pos);
  if (param_end == std::string::npos) {
    param_end = str.size();
  }
  return std::make_pair(str.substr(pos, param_end - pos), param_end);
}

// gpt로 생성된 함수라 검증이 필요함
bool Message::is_valid_nick(const std::string &nick) {
  if (nick.size() > NICKLEN) return false;
  for (std::string::const_iterator it = nick.begin(); it != nick.end(); it++) {
    if (!isalnum(*it) && *it != '-' && *it != '_') return false;
  }
  return true;
}

bool Message::is_valid_channel_name(const std::string &name) {
  if (name.size() < 2 || name.size() > CHANNELLEN) return false;

  std::string::const_iterator it = name.begin();
  if (*it != '#' && *it != '&') return false;
  it++;
  for (; it != name.end(); it++) {
    if (!isalnum(*it) && *it != '-' && *it != '_') return false;
  }

  return true;
}

int Message::is_valid_mode_flag(const std::string &flag) {
  if (flag.size() != 2 || (flag[0] != '+' && flag[0] != '-')) {
    return -1;
  }
  if (flag[1] == 'i')
    return 0;
  else if (flag[1] == 't')
    return 1;
  else if (flag[1] == 'k')
    return 2;
  else if (flag[1] == 'o')
    return 3;
  else if (flag[1] == 'l')
    return 4;
  return -1;
}

// Path: srcs/Message.cpp
