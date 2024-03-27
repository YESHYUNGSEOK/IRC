#include "../includes/Channel.hpp"
// #include "Channel.hpp"

Channel::Channel(std::string name, std::string key, bool scope, Client *host)
    : _name(name), _key(key), _max_clients(-1) {
  add_client(host);
  add_operator(host);
  if (scope == false)  // public
    _mode[SCOPE] = false;
  else  // private
    _mode[SCOPE] = true;
  _mode[INVITE_ONLY] = false;
  //_max_clients = -1 means no limit
}

Channel::~Channel() {}

std::string Channel::get_name() const { return _name; }

std::string Channel::get_key() const { return _key; }

std::string Channel::get_topic() const { return _topic; }

bool Channel::get_mode(enum modeType mode) const { return _mode[mode]; }

bool Channel::is_channel_full() const { return _clients.size() >= static_cast<size_t>(_max_clients); }

int Channel::get_max_clients() const { return _max_clients; }

void Channel::set_key(const std::string &key) { _key = key; }

void Channel::set_max_clients(int max_clients) { _max_clients = max_clients; }

void Channel::set_topic(const std::string &topic) { _topic = topic; }

void Channel::add_client(Client *client) {
  if (_clients.size() >= static_cast<size_t>(_max_clients)) {
    std::cout << "Channel is full" << std::endl;
    return;
  }
  _clients.insert(client);
}

void Channel::remove_client(Client *client) {
  std::set<Client *>::iterator it = _clients.find(client);
  if (it != _clients.end()) {
    _clients.erase(it);
  } else {
    std::cout << "Client not in channel" << std::endl;
  }
}

bool Channel::is_client_in_channel(Client *client) const {
  return _clients.find(client) != _clients.end();
}

bool Channel::is_client_in_channel(std::string nickname) const {
  for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end();
       ++it) {
    if ((*it)->get_nickname() == nickname) {
      return true;
    }
  }
  return false;
}

std::set<Client *> &Channel::get_clients() { return _clients; }

void Channel::add_operator(Client *client) {
  if (is_client_in_channel(client) == false) {
    std::cout << "Client not in channel" << std::endl;
    return;
  }
  if (is_operator(client) == true) {
    std::cout << "Client already operator" << std::endl;
    return;
  }
  _operators.insert(client);
}

void Channel::remove_operator(Client *client) {
  std::set<Client *>::iterator it = _operators.find(client);
  if (it != _operators.end()) {
    _operators.erase(it);
  } else {
    std::cout << "Client not operator" << std::endl;
  }
}

bool Channel::is_operator(Client *client) const {
  return _operators.find(client) != _operators.end();
}

std::set<Client *> &Channel::get_operators() { return _operators; }

void Channel::invite_client(Client *client) {
  if (is_invited(client) == true) {
    std::cout << "Client already invited" << std::endl;
    return;
  }
  if (_clients.find(client) == _invited.end()) {
    _invited.insert(client);
  }
}

bool Channel::is_invited(Client *client) const {
  return _invited.find(client) != _invited.end();
}

void Channel::set_channel_mode(enum modeType mode, bool value) {
  _mode[mode] = value;
}

bool Channel::get_channel_mode(enum modeType mode) const { return _mode[mode]; }
