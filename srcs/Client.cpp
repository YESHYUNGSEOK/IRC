#include "Client.hpp"

Client::Client(int server_fd) : _stream(*new SocketStream(server_fd))
{
	std::cout << "[Client] " << _stream.get_fd() << " connected" << std::endl;
}

Client::~Client()
{
	std::cout << "[Client] " << _stream.get_fd() << " disconnected" << std::endl;
	delete &_stream;
}

// 사용하지 않는 생성자
Client::Client() : _stream(*new SocketStream())
{
	std::cout << "[Client] default constructer called - need to fix" << std::endl;
}

// 사용하지 않는 복사 생성자
Client::Client(const Client &src) : _stream(*new SocketStream(src._stream))
{
	std::cout << "[Client] copy constructer called - need to fix" << std::endl;
}

// 사용하지 않는 대입 연산자
Client &Client::operator=(const Client &src)
{
	std::cout << "[Client] assign operater called - need to fix" << std::endl;
	_stream = *new SocketStream(src._stream);
	return *this;
}

int Client::get_fd() const
{
	return _stream.get_fd();
}

std::string Client::get_msg()
{
	std::string msg;
	_stream >> msg;
	return msg;
}

void Client::broadcast(const int origin_fd, const std::string &msg)
{
	// 클라이언트에게 메시지를 전송
	_stream << "Broadcast from Client " << std::to_string(origin_fd) << ": " << msg;
}
