#include "Channel.hpp"

Channel::Channel(const std::string& name, Client *client) : _name(name), _topic(""), _max_clients(0), _key("") {
	addClient(client);
	addOperator(client);
}

Channel::~Channel() {}

void Channel::addClient(Client* client) {
    // 중복 추가 방지
    if (!isClientInChannel(client)) {
        _clients[client->get_fd()] = client;
    }
}

void Channel::removeClient(Client* client) {
    _clients.erase(client->get_fd());
}

bool Channel::isClientInChannel(Client* client) const {
    return _clients.find(client->get_fd()) != _clients.end();
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

std::string Channel::getTopic() const {
    return _topic;
}

void Channel::setMode(char mode) {
    if (std::find(_mode.begin(), _mode.end(), mode) == _mode.end()) {
        _mode.push_back(mode);
    }
}

void Channel::unsetMode(char mode) {
    std::vector<char>::iterator it = std::find(_mode.begin(), _mode.end(), mode);
    if (it != _mode.end()) {
        _mode.erase(it);
    }
}

bool Channel::hasMode(char mode) const {
    return std::find(_mode.begin(), _mode.end(), mode) != _mode.end();
}

void Channel::addOperator(Client* client) {
    if (!isOperator(client)) {
        _operators.push_back(client);
    }
}

void Channel::removeOperator(Client* client) {
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client) {
            _operators.erase(it);
            break;
        }
    }
}

bool Channel::isOperator(Client* client) const {
    for (std::vector<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client) {
            return true;
        }
    }
    return false;
}

void Channel::inviteClient(Client* client) {
    if (std::find(_invited.begin(), _invited.end(), client) == _invited.end()) {
        _invited.push_back(client);
    }
}

bool Channel::isInvited(Client* client) const {
    return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void Channel::banClient(Client* client) {
    if (!isBanned(client)) {
        _banned.push_back(client);
    }
}

bool Channel::isBanned(Client* client) const {
    return std::find(_banned.begin(), _banned.end(), client) != _banned.end();
}

std::string Channel::getName() const {
    return _name;
}
