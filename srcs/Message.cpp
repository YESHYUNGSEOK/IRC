#include "Message.hpp"

const std::string Message::_command_arr[22] = {
    "NONE", "CAP",   "PASS",   "NICK",  "USER", "OPER",   "QUIT", "JOIN",
    "PART", "TOPIC", "MODE",   "NAMES", "LIST", "INVITE", "KICK", "PRIVMSG",
    "WHO",  "WHOIS", "WHOWAS", "KILL",  "PING", "PONG"};

Message::Message(std::string &line)
    : _command(NONE), _params(std::vector<std::string>()) {
  const std::string::size_type clrf_pos = line.length() - 2;
  std::string::size_type pos = 0;
  std::string cmd;

  // 커맨드 추출 - 알파벳, 숫자로 이루어진 문자열만 추출
  for (; pos < line.size(); pos++) {
    if (!isalnum(line[pos])) break;
    cmd += toupper(line[pos]);
  }
  pos = cmd.size();

  // 커맨드 종류 판별
  for (int i = 1; i < 22; i++) {
    if (cmd == _command_arr[i]) {
      _command = static_cast<e_cmd>(i);
      break;
    }
  }

  // 커맨드가 잘못됐거나, 뒤에 공백이 아닌 문자가 있을 경우
  if (_command == NONE || (pos != clrf_pos && line[pos] != ' ')) {
    _params.push_back(cmd);
    return;
  }

  while (pos != clrf_pos) {
    pos = line.find_first_not_of(" ", pos);
    if (pos == clrf_pos) break;

    std::string::size_type param_end = line.find_first_of("\r\n ", pos);
    if (line[pos] == ':') {
      std::string::size_type param_end = line.find_first_of("\r\n", pos);
      if (param_end != clrf_pos) {
        _command = NONE;
        return;
      }
      _params.push_back(line.substr(pos + 1, param_end - pos - 1));
      break;
    }
    _params.push_back(line.substr(pos, param_end - pos));
    pos = param_end;
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

// gpt로 생성된 함수라 검증이 필요함
bool Message::is_valid_nick(const std::string &nick) {
  if (nick.size() > NICKNAME_MAX_LEN) return false;
  for (std::string::const_iterator it = nick.begin(); it != nick.end(); it++) {
    if (!isalnum(*it) && *it != '-' && *it != '[' && *it != ']' && *it != '\\')
      return false;
  }
  return true;
}
