#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <bitset>
#include <map>
#include <set>

#include "Client.hpp"

class Channel {
 public:
  Channel(std::string name);
  ~Channel();

  enum ModeFlag {
    CLIENT_LIMIT,
    INVITE_ONLY,
    KEY,
    PROTECTED_TOPIC,
    NO_EXTERNAL_MESSAGES,
    MODE_SIZE
  };

  void init(Client *host);
  void join(Client *client, const std::string &key);
  void part(Client *client, const std::string &message);
  void quit(Client *client);
  void topic(Client *client);
  void topic(Client *client, const std::string *topic);
  void privmsg(Client *client, const std::string &message);
  void mode(Client *client);
  void mode(Client *client, const std::string &mode);

  bool is_member(Client *client);
  bool is_op(Client *client);
  bool is_invited(Client *client);
  Client *find_member(const std::string &nickname);
  Client *find_op(const std::string &nickname);
  Client *find_invited(const std::string &nickname);

  // op methods
  void op_client(Client *client, Client *target);
  void deop_client(Client *client, Client *target);
  void set_key(Client *client, const std::string &key);
  void set_topic(Client *client, const std::string &topic);

  // manage client
  bool is_client_in_channel(Client *client) const;
  Client *find_client(const std::string &nickname) const;
  void update_client_nick(Client *client, const std::string &new_nick);

  // manage operator
  void add_operator(Client *client);  // TODO: 삭제하고 op_client로 대체
  void remove_operator(Client *client);  // TODO: 삭제하고 deop_client로 대체
  bool is_operator(Client *client) const;
  Client *find_operator(const std::string &nickname) const;

  // manage client invitation
  const std::set<Client *> &get_invited() const;
  void invite_client(Client *client);
  void uninvite_client(Client *client);
  bool is_invited(Client *client) const;
  Client *find_invited(const std::string &nickname) const;

  // manage mode
  bool is_mode_set(ModeFlag flag) const;
  void set_mode(ModeFlag flag, bool value);

  // manage key
  const std::string &get_key() const;
  void set_key(const std::string &key);

  // manage max clients
  bool full() const;
  bool empty() const;
  std::size_t get_max_clients() const;
  void set_max_clients(std::size_t max_clients);

  // manage topic
  void set_topic(const std::string &topic, Client *topic_set_by);
  const std::string &get_topic() const;
  Client *get_topic_set_by() const;
  const std::time_t &get_topic_set_time() const;

  // public getters && setters
  const std::string &get_name() const;

  bool operator==(const Channel &other) const;
  bool operator!=(const Channel &other) const;
  bool operator<(const Channel &other) const;
  Channel &operator<<(const std::string &message);

 private:
  const std::string _name;
  std::string _key;
  std::bitset<MODE_SIZE> _mode;
  std::size_t _max_clients;

  std::string _topic;
  Client *_topic_set_by;
  std::time_t _topic_set_time;

  std::set<Client *> _clients;
  std::map<std::string, Client *> _clients_by_nick;
  std::set<Client *> _operators;
  std::map<std::string, Client *> _operators_by_nick;
  std::set<Client *> _invited;
  std::map<std::string, Client *> _invited_by_nick;

  void add_client(Client *client);
  void remove_client(Client *client);

  // 사용 X
  Channel();
  Channel(__unused const Channel &src);
  Channel &operator=(__unused const Channel &src);
};

#endif
