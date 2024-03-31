#include "Channel.hpp"

Channel::Channel(std::string name)
    : _name(name),
      _mode(std::bitset<4>(0)),
      _max_clients(0),
      _topic_set_by(NULL),
      _topic_set_time(0) {}
Channel::~Channel() {}

void Channel::init(Client *host) {
  _clients.insert(host);
  _clients_by_nick[host->get_nickname()] = host;
  _operators.insert(host);
  _operators_by_nick[host->get_nickname()] = host;

  *host << RPL_BRDCAST_JOIN(*host, *this);
  *host << RPL_DEFAULTCHANMODE(*host, *this);
  *host << RPL_NAMREPLY(*host, *this, "@" + host->get_nickname());
  *host << RPL_ENDOFNAMES(*host, *this);
}

void Channel::join(Client *client, const std::string &key = "") {
  if (is_client_in_channel(client)) {
    *client << ERR_USERONCHANNEL_443(*client, *client, *this);
  } else if (is_mode_set(Channel::NEED_KEY) && key != _key) {
    *client << ERR_BADCHANNELKEY_475(*client, *this);
  } else if (is_mode_set(Channel::INVITE_ONLY) && !is_invited(client)) {
    *client << ERR_INVITEONLYCHAN_473(*client, *this);
  } else if (full()) {
    *client << ERR_CHANNELISFULL_471(*client, *this);
  } else {
    add_client(client);
  }
}

void Channel::part(Client *client, const std::string &message = "") {
  if (!is_client_in_channel(client)) {
    *client << ERR_NOTONCHANNEL_442(*client, *this);
  } else {
    remove_client(client);
    *client << RPL_BRDCAST_PART(*client, *this, message);
    *this << RPL_BRDCAST_PART(*client, *this, message);
  }
}

void Channel::quit(Client *client) { remove_client(client); }

void Channel::privmsg(Client *client, const std::string &message) {
  std::set<Client *>::iterator it = _clients.begin();
  for (; it != _clients.end(); ++it) {
    if (*it != client) **it << RPL_PRIVMSG(*client, this->_name, message);
  }
}

void Channel::op_client(Client *client, Client *target) {
  if (!is_client_in_channel(client)) {
    *client << ERR_NOTONCHANNEL_442(*client, *this);
  } else if (!is_operator(client)) {
    *client << ERR_CHANOPRIVSNEEDED_482(*client, *this);
  } else if (!is_client_in_channel(target)) {
    *client << ERR_USERNOTINCHANNEL_441(*client, *target, *this);
  } else if (is_operator(target)) {
    *client << ERR_CHANOPRIVSNEEDED_482(*client, *this);
  } else {
    add_operator(target);
    *this << RPL_BRDCAST_MODE(*client, *this, "+o", target->get_nickname());
  }
}

void Channel::deop_client(Client *client, Client *target) {
  if (!is_client_in_channel(client)) {
    *client << ERR_NOTONCHANNEL_442(*client, *this);
  } else if (!is_operator(client)) {
    *client << ERR_CHANOPRIVSNEEDED_482(*client, *this);
  } else if (!is_client_in_channel(target)) {
    *client << ERR_USERNOTINCHANNEL_441(*client, *target, *this);
  } else if (!is_operator(target)) {
    *client << ERR_CHANOPRIVSNEEDED_482(*client, *this);
  } else {
    remove_operator(target);
    *this << RPL_BRDCAST_MODE(*client, *this, "-o", target->get_nickname());
  }
}

void Channel::add_client(Client *client) {
  _clients.insert(client);
  _clients_by_nick[client->get_nickname()] = client;
  _invited.erase(client);
  _invited_by_nick.erase(client->get_nickname());
  client->join_channel(this);

  std::string user_list;

  std::set<Client *>::iterator it = _clients.begin();
  for (; it != _clients.end(); ++it) {
    if (_operators.find(*it) != _operators.end())
      user_list += "@" + (*it)->get_nickname();
    else
      user_list += " " + (*it)->get_nickname();
  }

  *this << RPL_BRDCAST_JOIN(*client, *this);
  // *client << TOPIC message
  *client << RPL_NAMREPLY(*client, *this, user_list);
  *client << RPL_ENDOFNAMES(*client, *this);
}
void Channel::remove_client(Client *client) {
  _clients.erase(client);
  _clients_by_nick.erase(client->get_nickname());
  _operators.erase(client);
  _operators_by_nick.erase(client->get_nickname());
  _invited.erase(client);
  _invited_by_nick.erase(client->get_nickname());
  client->part_channel(this);
}
bool Channel::is_client_in_channel(Client *client) const {
  return _clients.find(client) != _clients.end();
}
Client *Channel::find_client(const std::string &nickname) const {
  std::map<std::string, Client *>::const_iterator it =
      _clients_by_nick.find(nickname);

  if (it == _clients_by_nick.end()) return NULL;
  return it->second;
}
void Channel::update_client_nick(Client *client, const std::string &new_nick) {
  _clients_by_nick.erase(client->get_nickname());
  _clients_by_nick[new_nick] = client;
}

void Channel::add_operator(Client *client) {
  _operators_by_nick[client->get_nickname()] = client;
  _operators.insert(client);
}
void Channel::remove_operator(Client *client) {
  _operators_by_nick.erase(client->get_nickname());
  _operators.erase(client);
}
bool Channel::is_operator(Client *client) const {
  return _operators.find(client) != _operators.end();
}
Client *Channel::find_operator(const std::string &nickname) const {
  std::map<std::string, Client *>::const_iterator it =
      _operators_by_nick.find(nickname);

  if (it == _operators_by_nick.end()) return NULL;
  return it->second;
}

const std::set<Client *> &Channel::get_invited() const { return _invited; }
void Channel::invite_client(Client *client) {
  _invited_by_nick[client->get_nickname()] = client;
  _invited.insert(client);
}
void Channel::uninvite_client(Client *client) {
  _invited_by_nick.erase(client->get_nickname());
  _invited.erase(client);
}
bool Channel::is_invited(Client *client) const {
  return _invited.find(client) != _invited.end();
}
Client *Channel::find_invited(const std::string &nickname) const {
  std::map<std::string, Client *>::const_iterator it =
      _invited_by_nick.find(nickname);

  if (it == _invited_by_nick.end()) return NULL;
  return it->second;
}

bool Channel::is_mode_set(ModeFlag flag) const { return _mode.test(flag); }
void Channel::set_mode(ModeFlag flag, bool value) { _mode.set(flag, value); }

const std::string &Channel::get_key() const { return _key; }
void Channel::set_key(const std::string &key) { _key = key; }

bool Channel::full() const {
  if (_mode.test(CLIENT_LIMIT_SET) == false) {
    return false;
  }
  return _max_clients <= _clients.size();
}
bool Channel::empty() const { return _clients.empty(); }
std::size_t Channel::get_max_clients() const {
  if (_mode.test(CLIENT_LIMIT_SET) == false) {
    return static_cast<std::size_t>(-1);  // unlimited
  }
  return _max_clients;
}
void Channel::set_max_clients(std::size_t max_clients) {
  _mode.set(CLIENT_LIMIT_SET);
  _max_clients = max_clients;
}

void Channel::set_topic(const std::string &topic, Client *topic_set_by) {
  _topic = topic;
  _topic_set_by = topic_set_by;
  _topic_set_time = std::time(NULL);
}
const std::string &Channel::get_topic() const { return _topic; }
Client *Channel::get_topic_set_by() const { return _topic_set_by; }
const std::time_t &Channel::get_topic_set_time() const {
  return _topic_set_time;
}

const std::string &Channel::get_name() const { return _name; }

bool Channel::operator==(const Channel &other) const {
  return _name == other._name;
}
bool Channel::operator!=(const Channel &other) const {
  return _name != other._name;
}
bool Channel::operator<(const Channel &other) const {
  return _name < other._name;
}
Channel &Channel::operator<<(const std::string &message) {
  std::set<Client *>::iterator it = _clients.begin();
  for (; it != _clients.end(); ++it) **it << message;
  return *this;
}

// 사용 X
Channel::Channel() {}
Channel::Channel(__unused const Channel &src) { *this = src; }
Channel &Channel::operator=(__unused const Channel &src) { return *this; }
