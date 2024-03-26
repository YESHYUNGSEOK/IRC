#pragma once
#include "Channel.hpp"
#ifndef SERVER_HPP
#define SERVER_HPP

#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"
#include "ft_irc.hpp"

class Server
{
private:
	int _port;
	std::string _password;
	int _server_fd;
	fd_set _master_fds;
	fd_set _read_fds;
	// fd_set _write_fds; - 임시 주석처리
	struct sockaddr_in _addr;

	// std::map<int, Client *> _clients;
	std::set<Client *> _clients;
	// std::vector<struct sockaddr_in> _client_addrs;

	std::set<Channel *> _channels;

	Server();
	Server(const Server &src);
	Server &operator=(const Server &src);
	// private utils
	std::vector<std::string> split_tokens(const std::string &msg, const char delim);
	bool check_channel_name(const std::vector<std::string> &channel_tokens) const;

public:
	Server(int port, std::string password);
	~Server();

	void run();

	void accept_new_client();
	void read_client(Client *client);
	void write_client(Client *client);

	// CAP 명령어 처리
	void capability(Client *client, std::stringstream &cap_params);

	// PASS 명령어 처리
	void register_client(Client *client, std::string msg);

	// NICK 명령어 처리
	void set_nickname(Client *client, std::string msg);

	// USER 명령어 처리
	void set_userinfo(Client *client, std::string msg);

	// JOIN 명령어 처리
	void join_channel(Client *client, std::string msg);
};

#endif
