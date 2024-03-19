#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

enum MODE {
    OPERATOR = 'o',      // 운영자
    VOICE = 'v',         // 발언권
    INVITE_ONLY = 'i',   // 초대 전용
    MODERATED = 'm',     // 중재됨
    NO_EXTERNAL_MESSAGES = 'n', // 외부 메시지 금지
    TOPIC_SETTABLE_BY_OP_ONLY = 't', // 토픽 보호
    SECRET = 's',        // 비밀 채널
    PRIVATE = 'p',       // 개인 채널
    USER_LIMIT = 'l',    // 사용자 수 제한
    BAN_MASK = 'b',      // 밴 마스크
    KEY = 'k'            // 채널 키 (비밀번호)
};

class Channel {
private:
    int _max_clients;
    std::string _name;
    std::string _topic;
    std::string _key;
    std::vector<char> _mode;
    std::map<int, Client*> _clients;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;
    std::vector<Client*> _banned;

public:
    Channel(const std::string& name, Client *client);
    ~Channel();

    void addClient(Client* client);
    void removeClient(Client* client);
    bool isClientInChannel(Client* client) const;
    void setTopic(const std::string& topic);
    std::string getTopic() const;
    void setMode(char mode);
    void unsetMode(char mode);
    bool hasMode(char mode) const;
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;
    void inviteClient(Client* client);
    bool isInvited(Client* client) const;
    void banClient(Client* client);
    bool isBanned(Client* client) const;
    std::string getName() const;
};

#endif
