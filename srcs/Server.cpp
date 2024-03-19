#include "Server.hpp"

Server::Server(int port, int password) : _port(port), _password(password)
{
	// 소켓 생성 (IPv4, TCP)
	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd < 0)
		throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));

	_addr.sin_family = AF_INET;			// IPv4
	_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
	_addr.sin_port = htons(_port);		// network byte order

	// 소켓 바인딩
	if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
		throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
}

void Server::run()
{
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
		throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
	// 소켓 리스닝 SOMAXCONN은 시스템에서 지정한 최대 연결 수
	if (listen(_server_fd, SOMAXCONN) < 0) // SOMAXCONN은 시스템에서 지정한 최대 연결 수
		throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));

	// 모든 파일 디스크립터를 0으로 초기화
	FD_ZERO(&_master);
	FD_SET(_server_fd, &_master);

	while (true)
	{
		fd_set read_fds = _master; // select()를 호출할 때마다 초기화해야 함
		// select()는 이벤트가 발생한 파일 디스크립터의 개수를 반환
		int event = select(FD_SETSIZE, &read_fds, NULL, NULL, NULL);
		if (event < 0)
			throw std::runtime_error("select() failed: " + std::string(strerror(errno)));
		else if (event > 0)
		{
			std::cout << "[Server] Event count: " << event << std::endl;
			for (int i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &read_fds)) // 이벤트가 발생한 파일 디스크립터
				{
					if (i == _server_fd) // 새로운 클라이언트가 연결되었을 때
						accept_new_client();
					else // 기존 클라이언트로부터 메시지를 받았을 때
						handle_client(_clients[i]);
				}
			}
		}
	}
}

void Server::accept_new_client()
{
	Client *client = new Client(_server_fd);				   // 새로운 클라이언트 생성
	FD_SET(client->get_fd(), &_master);						   // 새로운 클라이언트를 _master에 추가
	_clients.insert(std::make_pair(client->get_fd(), client)); // 새로운 클라이언트를 _clients에 추가
}

void Server::handle_client(Client *client)
{
	// 클라이언트로부터 메시지를 받음
	std::string msg = client->get_msg();

	if (msg.length() == 0) // 클라이언트 소켓이 끊어졌을 때의 처리
	{
		FD_CLR(client->get_fd(), &_master);
		_clients.erase(client->get_fd());
		delete client;
	}
	else
	{
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
			if (it->second != client)
				it->second->broadcast(client->get_fd(), msg);
	}
}

Server::Server() : _port(0), _password(0), _server_fd(0)
{
}

Server::Server(const Server &src) : _port(src._port), _password(src._password), _server_fd(src._server_fd)
{
}

Server &Server::operator=(const Server &src)
{
	_port = src._port;
	_password = src._password;
	_server_fd = src._server_fd;
	return *this;
}

Server::~Server()
{
	close(_server_fd);
}

// Path: src/Server.cpp
