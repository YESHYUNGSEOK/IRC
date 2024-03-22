#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>

#include "Client.hpp"

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
  enum MODE {
    OPERATOR = 1 << 0,                   // 운영자
    VOICE = 1 << 1,                      // 발언권
    INVITE_ONLY = 1 << 2,                // 초대 전용
    MODERATED = 1 << 3,                  // 중재됨
    NO_EXTERNAL_MESSAGES = 1 << 4,       // 외부 메시지 금지
    TOPIC_SETTABLE_BY_OP_ONLY = 1 << 5,  // 토픽 보호
    SECRET = 1 << 6,                     // 비밀 채널
    PRIVATE = 1 << 7,                    // 개인 채널
    USER_LIMIT = 1 << 8,                 // 사용자 수 제한
    BAN_MASK = 1 << 9,                   // 밴 마스크
    KEY = 1 << 10                        // 채널 키 (비밀번호)
  };

  Channel(const std::string& name, Client* client);
  ~Channel();

  // 채널 이름
  std::string get_name() const;

  // 채널 키
  void set_key(const std::string& key);
  std::string get_key() const;

  // 최대 클라이언트 수
  int get_max_clients() const;
  void set_max_clients(int max_clients);

  // 클라이언트
  void add_client(Client* client);
  void remove_client(Client* client);
  bool is_client_in_channel(Client* client) const;

  // 토픽
  void set_topic(const std::string& topic);
  std::string get_topic() const;

  // 모드
  void set_mode(MODE mode);
  void unset_mode(MODE mode);
  bool has_mode(MODE mode) const;

  // 운영자
  void add_operator(Client* client);
  void remove_operator(Client* client);
  bool is_operator(Client* client) const;

  // 초대
  void invite_client(Client* client);
  bool is_invited(Client* client) const;

  // 밴
  void ban_client(Client* client);
  bool is_banned(Client* client) const;
};

#endif
