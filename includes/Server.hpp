#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "ft_irc.hpp"

class Client;

class Server {
 private:
  int _port;
  int _password;
  int _server_fd;
  fd_set _master_fds;
  fd_set _read_fds;
  // fd_set _write_fds; - 임시 주석처리
  struct sockaddr_in _addr;

  // std::map<int, Client *> _clients;
  std::set<Client *> _clients;
  // std::vector<struct sockaddr_in> _client_addrs;

  Server();
  Server(const Server &src);
  Server &operator=(const Server &src);

 public:
  Server(int port, int password);
  ~Server();

  void run();

  void accept_new_client();
  void read_client(Client *client);
  void write_client(Client *client);

  // CAP 명령어 처리
  void capability(Client *client, std::stringstream &cap_params);

  // PASS 명령어 처리
  void confirm_password(Client *client, const std::string &password);

  // NICK 명령어 처리
  //   void set_nickname(Client *client, const std::string &nickname);
};

#endif
