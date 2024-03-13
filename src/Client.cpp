#include "Client.hpp"

Client::Client(int fd, sockaddr_in addr) : _fd(fd), _client_addr(addr), _client_addr_len(sizeof(addr))
{
}

Client::~Client()
{
	close(_fd);
}

Client::Client()
{
	// 사용하지 않는 생성자
	std::memset(this, 0, sizeof(Client));
}

Client::Client(const Client &src)
{
	*this = src;
}

Client &Client::operator=(const Client &src)
{
	_fd = src._fd;
	_client_addr = src._client_addr;
	_client_addr_len = src._client_addr_len;
	return *this;
}

void Client::broadcast(const int origin_fd, const std::string &msg)
{
	// 클라이언트에게 메시지를 전송
	if (send(_fd, "Broadcast from Client ", 22, 0) < 0)
	{
		throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
	}
	if (send(_fd, std::to_string(origin_fd).c_str(), std::to_string(origin_fd).length(), 0) < 0)
	{
		throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
	}
	if (send(_fd, ": ", 2, 0) < 0)
	{
		throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
	}
	if (send(_fd, msg.c_str(), msg.length(), 0) < 0)
	{
		throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
	}
}
