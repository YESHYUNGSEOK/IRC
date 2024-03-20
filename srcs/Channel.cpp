#include "Channel.hpp"

Channel::Channel(const std::string& name, Client *client) : _name(name), _topic(""), _max_clients(0), _key("") {
	addClient(client);
	addOperator(client);
}

Channel::~Channel() {}

std::string Channel::getName() const {
    return _name;
}

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

void Channel::setMode(MODE mode) {
    _mode |= mode;
}

void Channel::unsetMode(MODE mode) {
    _mode &= ~mode;
}

bool Channel::hasMode(MODE mode) const {
    return (_mode & mode) != 0;
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
