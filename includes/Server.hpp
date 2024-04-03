#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <algorithm>
#include <ctime>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <csignal>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "Utility.hpp"
#include "ft_irc.hpp"

class Client;
class Channel;

class Server
{
private:
  const int _port;
  const std::string _password;
  std::string _created_at;
  const int _server_fd;
  fd_set _master_fds;
  fd_set _read_fds;
  fd_set _write_fds;
  struct sockaddr_in _addr;

  std::map<int, Client *> _clients;
  std::map<std::string, Client *> _clients_by_nick;
  std::vector<Client *> _clients_to_disconnect;

  std::set<Channel *> _channels;
  std::map<std::string, Channel *> _channels_by_name;

  // 사용 X
  Server();
  Server(__unused const Server &src);
  Server &operator=(__unused const Server &src);

  // private utils
  void accept_new_client();
  // TODO: QUIT 이후에 클라이언트가 소켓을 닫는 경우와 갑작스러운 종료 구분
  void remove_client(Client *client);
  void disconnect_client(Client *client);
  void read_client(Client *client);

  // private command handlers
  void register_client(Client *client);
  void update_nick(Client *client, const std::string &new_nick);

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
  void QUIT(Client *client, const std::vector<std::string> &params);

  void JOIN(Client *client, const std::vector<std::string> &params);
  void PART(Client *client, const std::vector<std::string> &params);
  void TOPIC(Client *client, const std::vector<std::string> &params);
  void INVITE(Client *client, const std::vector<std::string> &params);
  void KICK(Client *client, const std::vector<std::string> &params) void PRIVMSG(Client *client, const std::vector<std::string> &params);

  // MODE 명령어 처리: ongoing...
  void MODE(Client *client, const std::vector<std::string> &params);

public:
  static bool _is_shutdown;
  Server(int port, std::string password);
  ~Server();

  void run();
  void run_by_map();

  Server &operator<<(const std::string &message);
};

#endif
