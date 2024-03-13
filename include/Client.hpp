#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "IoHandler.hpp"

class Client
{
public:
	Client(int fd, IoHandler &io_handler);
	~Client();

	void handle_input();
	void handle_output();
};

#endif // CLIENT_HPP