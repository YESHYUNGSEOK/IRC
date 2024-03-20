#include "Channel.hpp"

Channel::Channel(const std::string& name, Client* client)
    : _name(name), _topic(""), _max_clients(0), _key("") {
  add_client(client);
  add_operator(client);
}

Channel::~Channel() {}

std::string Channel::get_name() const { return _name; }

void Channel::set_key(const std::string& key) { _key = key; }

std::string Channel::get_key() const { return _key; }

int Channel::get_max_clients() const { return _max_clients; }

void Channel::set_max_clients(int max_clients) { _max_clients = max_clients; }

void Channel::add_client(Client* client) {
  if (!is_client_in_channel(client)) {
    _clients[client->get_fd()] = client;
  }
}

void Channel::remove_client(Client* client) {
  _clients.erase(client->get_fd());
}

bool Channel::is_client_in_channel(Client* client) const {
  return _clients.find(client->get_fd()) != _clients.end();
}

void Channel::set_topic(const std::string& topic) { _topic = topic; }

std::string Channel::get_topic() const { return _topic; }

void Channel::set_mode(MODE mode) { _mode |= mode; }

void Channel::unset_mode(MODE mode) { _mode &= ~mode; }

bool Channel::has_mode(MODE mode) const { return (_mode & mode) != 0; }

void Channel::add_operator(Client* client) {
  if (!is_operator(client)) {
    _operators.push_back(client);
  }
}

void Channel::remove_operator(Client* client) {
  for (auto it = _operators.begin(); it != _operators.end(); ++it) {
    if (*it == client) {
      _operators.erase(it);
      break;
    }
  }
}

bool Channel::is_operator(Client* client) const {
  for (const auto& operator_client : _operators) {
    if (operator_client == client) {
      return true;
    }
  }
  return false;
}

void Channel::invite_client(Client* client) {
  if (std::find(_invited.begin(), _invited.end(), client) == _invited.end()) {
    _invited.push_back(client);
  }
}

bool Channel::is_invited(Client* client) const {
  return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void Channel::ban_client(Client* client) {
  if (!is_banned(client)) {
    _banned.push_back(client);
  }
}

bool Channel::is_banned(Client* client) const {
  return std::find(_banned.begin(), _banned.end(), client) != _banned.end();
}
