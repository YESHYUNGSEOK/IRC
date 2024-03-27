#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>

#include "Client.hpp"

class Client;

enum modeType
{
  SCOPE, // private must have key
  INVITE_ONLY,
  TOPIC_RESTRICTED
};

class Channel
{
public: // constructor and destructor
  Channel(std::string name, std::string key, bool scope, Client *host);
  ~Channel();

public: // public getters && setters
  std::string get_name() const;
  std::string get_key() const;
  std::string get_topic() const;
  int get_max_clients() const;

  void set_key(const std::string &key);
  void set_topic(const std::string &topic);
  void set_max_clients(int max_clients);

public: // public methods
  // manage client
  void add_client(Client *client);
  void remove_client(Client *client);
  bool is_client_in_channel(Client *client) const;
  std::set<Client *> &get_clients();

  // manage operator
  void add_operator(Client *client);
  void remove_operator(Client *client);
  bool is_operator(Client *client) const;
  std::set<Client *> &get_operators();

  // manage client invitation
  void invite_client(Client *client);
  bool is_invited(Client *client) const;

  // set channel mode
  void set_channel_mode(enum modeType mode, bool value);

private: // private members
  std::string _name;
  std::string _key;
  std::string _topic;

  bool _mode[3];
  int _max_clients;

  std::set<Client *> _clients;
  std::set<Client *> _operators;
  std::set<Client *> _invited;
};

#endif
