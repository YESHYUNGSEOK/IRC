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

	// fd_set 초기화
	FD_ZERO(&_master_fds);
	FD_ZERO(&_read_fds);
	FD_ZERO(&_write_fds);

	// 서버 소켓을 _master에 추가
	FD_SET(_server_fd, &_master_fds);
}

void Server::run()
{
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
		throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));

	if (listen(_server_fd, SOMAXCONN) < 0) // SOMAXCONN은 시스템에서 지정한 최대 연결 수
		throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));

	while (true)
	{
		// select()를 호출할 때마다 초기화
		_read_fds = _master_fds;
		_write_fds = _master_fds;

		// select()는 이벤트가 발생한 파일 디스크립터의 개수를 반환
		int event = select(FD_SETSIZE, &_read_fds, &_write_fds, NULL, NULL);
		if (event < 0)
			throw std::runtime_error("select() failed: " + std::string(strerror(errno)));
		else if (event > 0)
		{
			std::cout << "[Server] Event count: " << event << std::endl;
			for (int i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &_read_fds)) // read event
				{
					if (i == _server_fd) // 새로운 클라이언트 연결
						accept_new_client();
					else // 기존 클라이언트로부터 메시지를 받음
						read_client(_clients[i]);
				}
				else if (FD_ISSET(i, &_write_fds)) // write event
				{
					if (i != _server_fd) // 버퍼 내용을 소켓에 쓰기
						write_client(_clients[i]);
				}
			}
		}
	}
}

void Server::accept_new_client()
{
	Client *client = new Client(_server_fd);				   // 새로운 클라이언트 생성
	FD_SET(client->get_fd(), &_master_fds);					   // 새로운 클라이언트를 _master에 추가
	_clients.insert(std::make_pair(client->get_fd(), client)); // 새로운 클라이언트를 _clients에 추가
}

void Server::read_client(Client *client)
{
	// 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
	if (client->recv() < 0) // 연결이 끊어졌을 때
	{
		FD_CLR(client->get_fd(), &_master_fds);
		_clients.erase(client->get_fd());
		delete client;
	}

	// 클라이언트의 버퍼로부터 데이터를 읽어들임 - 개행 단위로 처리를 위해 벡터로 변경될 예정
	std::string msg = client->read_buffer();

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if (it->second != client)
			it->second->broadcast(client->get_fd(), msg);
}

void Server::write_client(Client *client)
{
	// 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
	client->send();
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
