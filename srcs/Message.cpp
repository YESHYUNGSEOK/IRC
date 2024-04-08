#include "Message.hpp"

const std::string Message::_command_arr[22] = {
    "NONE", "CAP",   "PASS",   "NICK",  "USER", "OPER",   "QUIT", "JOIN",
    "PART", "TOPIC", "MODE",   "NAMES", "LIST", "INVITE", "KICK", "PRIVMSG",
    "WHO",  "WHOIS", "WHOWAS", "KILL",  "PING", "PONG"};

Message::Message(std::string &line)
    : _command(NONE), _params(std::vector<std::string>()) {
  // 중간에 \r이 있을 경우 오류
  if (line.find('\r') != std::string::npos) {
    _command = NONE;
    return;
  }

  std::string::size_type pos = line.find_first_not_of(" \t");

  // 공백만 있는 경우 오류
  if (pos == std::string::npos) {
    _command = NONE;
    return;
  }

  std::string::size_type cmd_end = line.find_first_of(" \t", pos);
  if (cmd_end == std::string::npos) {
    cmd_end = line.size();
  }

  std::string cmd;
  cmd.reserve(cmd_end - pos);

  // 커맨드 추출 - 알파벳, 숫자로 이루어진 문자열만 추출
  for (std::string::size_type i = 0; i < cmd_end - pos; i++) {
    if (!std::isalnum(line[pos + i])) {
      _command = NONE;
      return;
    }
    cmd.push_back(std::toupper(line[pos + i]));
  }

  // 커맨드 종류 판별
  for (int i = 1; i < 22; i++) {
    if (cmd == _command_arr[i]) {
      _command = static_cast<e_cmd>(i);
      break;
    }
  }

  // 커맨드가 잘못됐거나, 뒤에 공백이 아닌 문자가 있을 경우
  if (_command == NONE) {
    _params.push_back(cmd);
    return;
  }

  while (pos < line.size()) {
    std::pair<std::string, std::string::size_type> param =
        get_first_param(line, pos);
    if (param.first.empty()) {
      break;
    } else if (param.first.at(0) == ':') {
      _params.push_back(line.substr(pos));
      break;
    }
    _params.push_back(param.first);
    pos = param.second;
    pos = line.find_first_not_of(" \t", pos);
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
    if (!isalnum(*it) && *it != '-' && *it != '[' && *it != ']' &&
        *it != '\\' && *it != '_')
      return false;
  }
  return true;
}

bool Message::is_valid_channel_name(const std::string &name) {
  if (name.size() < 2 || name.size() > CHANNELLEN) return false;

  std::string::const_iterator it = name.begin();
  if (*it != '#' && *it != '&') return false;
  it++;
  for (; it != name.end(); it++) {
    if (!isalnum(*it) && *it != '-' && *it != '.' && *it != '-' && *it != '_')
      return false;
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

std::istream &operator>>(std::istream &is, Message &msg) {
  std::string line;

  std::getline(is, line, ' ');
  msg = Message(line);
  return is;
}

// Path: srcs/Message.cpp
