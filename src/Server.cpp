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
			std::cout << "Event: " << event << std::endl;
			if (FD_ISSET(_server_fd, &read_fds))
				accept_new_client();
			// 2중 for문을 사용하지 않고 클라이언트를 순회하면서 이벤트를 처리하고싶은데 몰?루
			for (int client_fd = _server_fd + 1; client_fd < FD_SETSIZE; client_fd++)
				if (FD_ISSET(client_fd, &read_fds))
				{
					std::list<Client *>::iterator it = _clients.begin();
					while (it != _clients.end())
					{
						if ((*it)->get_fd() == client_fd)
						{
							handle_client(*it);
							break;
						}
						it++;
					}
				}
			// for (std::list<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
			// {
			// 	if (FD_ISSET((*it)->get_fd(), &read_fds))
			// 		handle_client(*it);
			// }
		}
	}
}

void Server::accept_new_client()
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd < 0)
		throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
		throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
	// 새로운 클라이언트를 _master에 추가
	_clients.push_back(new Client(client_fd, client_addr));
	FD_SET(client_fd, &_master);
	std::cout << "Client " << client_fd << " connected" << std::endl;
}

void Server::handle_client(Client *client)
{
	char buf[1024];
	int len = recv(client->get_fd(), buf, sizeof(buf), 0);
	if (len < 0)
	{
		throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
	}
	else if (len == 0) // 클라이언트 소켓이 끊어졌을 때의 처리
	{
		FD_CLR(client->get_fd(), &_master);
		std::cout << "Client " << client->get_fd() << " disconnected" << std::endl;
		std::list<Client *>::iterator it = _clients.begin();
		while (it != _clients.end())
		{
			if (*it == client)
			{
				_clients.erase(it);
				break;
			}
		}
		delete client;
	}
	else
	{
		buf[len] = '\0';
		std::string msg(buf);
		std::cout << "Received from Client " << client->get_fd() << ": " << msg;

		for (std::list<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
			if (*it != client)
				(*it)->broadcast(client->get_fd(), msg);
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
