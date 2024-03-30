#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "ft_irc.hpp"

class Client;
class Channel;

class Server {
 private:
  const int _port;
  const std::string _password;
  std::string _created_at;
  const int _server_fd;
  fd_set _master_fds;
  fd_set _read_fds;
  struct sockaddr_in _addr;

  std::set<Client *> _clients;
  std::map<std::string, Client *> _clients_by_nick;
  std::set<Channel *> _channels;
  std::map<std::string, Channel *> _channels_by_name;

  // 사용 X
  Server();
  Server(__unused const Server &src);
  Server &operator=(__unused const Server &src);

  // private utils
  void accept_new_client();
  // std::size_t remove_client(Client *client);
  std::set<Client *>::iterator remove_client(
      std::set<Client *>::iterator client_it);
  void read_client(Client *client);
  void write_client(Client *client);

  // private command handlers
  void register_client(Client *client);
  void update_client_nick(Client *client, const std::string &new_nick);

  // private utils
  Client *find_client_by_nick(const std::string &nickname);
  Channel *find_channel_by_name(const std::string &channel_name);

  // 명령어 처리
  void CAP(Client *client, const std::vector<std::string> &params);
  void PASS(Client *client, const std::vector<std::string> &params);
  void NICK(Client *client, const std::vector<std::string> &params);
  void USER(Client *client, const std::vector<std::string> &params);
  void PING(Client *client, const std::vector<std::string> &params);
  void PONG(Client *client, const std::vector<std::string> &params);

  void JOIN(Client *client, const std::vector<std::string> &params);
  void PART(Client *client, const std::vector<std::string> &params);
  void TOPIC(Client *client, const std::vector<std::string> &params);
  void INVITE(Client *client, const std::vector<std::string> &params);

  // MODE 명령어 처리: ongoing...
  void MODE(Client *client, const std::vector<std::string> &params);

 public:
  Server(int port, std::string password);
  ~Server();

  void run();

  Server &operator<<(const std::string &message);
};

#endif
