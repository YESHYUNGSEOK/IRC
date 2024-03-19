#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"
#include "SocketStream.hpp"

class Client
{
private:
	// const int _fd;
	// const struct sockaddr_in _client_addr;
	// const socklen_t _client_addr_len;
	SocketStream &_stream;

	Client();
	Client(const Client &src);
	Client &operator=(const Client &src);

public:
	Client(int server_fd);
	~Client();

	int get_fd() const;
	std::string read_buffer();				   // 연산자 오버로딩으로 변경해도 될 듯
	void write_buffer(const std::string &msg); // 연산자 오버로딩으로 변경해도 될 듯
	ssize_t recv();
	ssize_t send();
	void broadcast(const int origin_fd, const std::string &msg);
};

#endif // CLIENT_HPP
