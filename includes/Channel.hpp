#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

enum MODE {
    OPERATOR = 1 << 0,      // 운영자
    VOICE = 1 << 1,         // 발언권
    INVITE_ONLY = 1 << 2,   // 초대 전용
    MODERATED = 1 << 3,     // 중재됨
    NO_EXTERNAL_MESSAGES = 1 << 4, // 외부 메시지 금지
    TOPIC_SETTABLE_BY_OP_ONLY = 1 << 5, // 토픽 보호
    SECRET = 1 << 6,        // 비밀 채널
    PRIVATE = 1 << 7,       // 개인 채널
    USER_LIMIT = 1 << 8,    // 사용자 수 제한
    BAN_MASK = 1 << 9,      // 밴 마스크
    KEY = 1 << 10           // 채널 키 (비밀번호)
};

class Channel {
private:
    std::string _name;
    std::string _topic;
    std::string _key;
    
	int _mode;
    int _max_clients;
    
	std::map<int, Client*> _clients;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;
    std::vector<Client*> _banned;

public:
    Channel(const std::string& name, Client *client);
    ~Channel();

	// 채널 이름
    std::string getName() const;

	// 클라이언트
    void addClient(Client* client);
    void removeClient(Client* client);
    bool isClientInChannel(Client* client) const;

	// 토픽
    void setTopic(const std::string& topic);
    std::string getTopic() const;

	// 모드
	void setMode(MODE mode);
    void unsetMode(MODE mode);
    bool hasMode(MODE mode) const;

	// 운영자
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;

	// 초대
    void inviteClient(Client* client);
    bool isInvited(Client* client) const;

	// 밴
    void banClient(Client* client);
    bool isBanned(Client* client) const;
};

#endif
