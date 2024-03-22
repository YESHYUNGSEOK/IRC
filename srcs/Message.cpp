#include "Message.hpp"

Message::Message(std::string &chunk)
    : _is_valid(true), _chunk(chunk), _prefix(""), _command(NONE), _params() {}

Message::~Message() {}

bool Message::parseMsg() {
  std::string::size_type pos = 0;
  std::string::size_type nextPos = 0;

  if (this->_chunk.empty()) return false;
  std::string::size_type spacePos = this->_chunk.find("\r\n");
  if (spacePos == std::string::npos) return false;
  std::string chunk_copy = this->_chunk.substr(0, spacePos);
  if (chunk_copy[0] == ':') {
    nextPos = chunk_copy.find(' ');
    if (nextPos == std::string::npos) return false;
    this->_prefix = chunk_copy.substr(1, nextPos - 1);
    pos = nextPos + 1;
    nextPos = pos;
  }
  if (std::isalpha(chunk_copy[pos])) {
    while (std::isalpha(chunk_copy[pos])) {
      if (chunk_copy[pos] >= 'a' && chunk_copy[pos] <= 'z') return false;
      pos++;
    }
    if (is_valid_command(chunk_copy.substr(nextPos, pos - nextPos)) == false)
      return false;
  } else
    return false;
  if (chunk_copy[pos] == ' ') {
    if (++pos == chunk_copy.size()) return true;
    std::string token;
    for (; pos < chunk_copy.size(); pos++) {
      if (chunk_copy[pos] == ' ') {
        if (!token.empty()) {
          this->_params.push_back(token);
          token.clear();
        }
      } else if (chunk_copy[pos] == ':') {
        while (++pos < chunk_copy.size()) token += chunk_copy[pos];
        this->_params.push_back(token);
        token.clear();
      } else {
        token += chunk_copy[pos];
        if (pos == chunk_copy.size() - 1) this->_params.push_back(token);
      }
    }
    return true;
  } else
    return false;
}

bool Message::is_valid_command(const std::string &command) {
  const std::string cmdArr[20] = {
      "PASS",  "NICK",  "USER",   "OPER", "QUIT",   "JOIN", "PART",
      "TOPIC", "MODE",  "NAMES",  "LIST", "INVITE", "KICK", "PRIVMSG",
      "WHO",   "WHOIS", "WHOWAS", "KILL", "PING",   "PONG"};
  if (command.empty()) return false;
  for (int i = 0; i < 20; i++) {
    if (command == cmdArr[i]) {
      this->_command = static_cast<e_cmd>(i);
      return true;
    }
  }
  return false;
}

bool Message::is_valid() const { return this->_is_valid; }

const std::string &Message::get_prefix() const { return this->_prefix; }

const e_cmd &Message::get_command() const { return this->_command; }

const std::vector<std::string> &Message::get_params() const {
  return this->_params;
}
