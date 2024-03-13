#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
private:
	int _fd;
	struct sockaddr_in _client_addr;
	socklen_t _client_addr_len;
	// IoHandler &_io_handler;

	Client();
	Client(const Client &src);
	Client &operator=(const Client &src);

public:
	Client(int fd, struct sockaddr_in addr);
	~Client();

	int get_fd() const
	{
		return _fd;
	}
	void broadcast(const int origin_fd, const std::string &msg);
};

#endif // CLIENT_HPP
