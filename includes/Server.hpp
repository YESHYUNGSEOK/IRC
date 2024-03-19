#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include "ft_irc.hpp"
#include "Client.hpp"

class Server
{

private:
	int _port;
	int _password;
	int _server_fd;
	fd_set _master;
	struct sockaddr_in _addr;

	std::map<int, Client *> _clients;
	// std::vector<struct sockaddr_in> _client_addrs;

	Server();
	Server(const Server &src);
	Server &operator=(const Server &src);

public:
	Server(int port, int password);
	~Server();

	void run();
	void accept_new_client();
	void handle_client(Client *client);
};

#endif
