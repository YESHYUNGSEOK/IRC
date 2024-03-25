#include "../includes/Server.hpp"
#include <string>

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    // 소켓 생성 (IPv4, TCP)
    _server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_server_fd < 0)
        throw std::runtime_error("socket() failed: " +
                                 std::string(strerror(errno)));

    _addr.sin_family = AF_INET;         // IPv4
    _addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    _addr.sin_port = htons(_port);      // network byte order

    unsigned int opt = 1; // 소켓 옵션 설정 - 재사용 허용
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() failed: " +
                                 std::string(strerror(errno)));

    // 소켓 바인딩
    if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
        throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));

    // fd_set 초기화
    FD_ZERO(&_master_fds);
    FD_ZERO(&_read_fds);
    // FD_ZERO(&_write_fds);

    // 서버 소켓을 _master에 추가
    FD_SET(_server_fd, &_master_fds);

#ifdef __APPLE__                                    // macOS
    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) // 소켓을 논블로킹으로 설정
        throw std::runtime_error("[Server::run()]: fcntl() failed: " +
                                 std::string(strerror(errno)));
#endif

    // SOMAXCONN은 시스템에서 지정한 최대 연결 요청 대기 큐의 크기
    if (listen(_server_fd, SOMAXCONN) < 0)
        throw std::runtime_error("[Server::run()]: listen() failed: " +
                                 std::string(strerror(errno)));
}

void Server::run()
{
    while (true)
    {
        _read_fds = _master_fds; // select()를 호출할 때마다 초기화
        // _write_fds = _master_fds;

        const int max_sock_fd = FD_SETSIZE;
        // _clients.empty() ? _server_fd : (*_clients.rbegin())->get_fd();

        // select()는 이벤트가 발생한 파일 디스크립터의 개수를 반환
        // int event = select(FD_SETSIZE, &_read_fds, &_write_fds, NULL, NULL);
        const int event = select(max_sock_fd, &_read_fds, NULL, NULL, NULL);

        if (event < 0)
            throw std::runtime_error("select() failed: " +
                                     std::string(strerror(errno)));
        else if (event > 0)
        {
            std::set<Client *>::iterator it = _clients.begin();

            while (it != _clients.end())
            {
                try
                {
                    if (FD_ISSET((*it)->get_fd(), &_read_fds)) // read event
                    {
                        read_client(*it);
                        write_client(*it);
                    }
                    it++;
                }
                catch (SocketStream::SystemCallException &e)
                { // 소켓 시스템 콜 예외 - 복구 불가능
                    std::cerr << "Error: " << e.what() << std::endl;
                    FD_CLR((*it)->get_fd(), &_master_fds);
                    delete *it;
                    it = _clients.erase(it);
                }
                catch (SocketStream::ConnectionClosedException &e)
                { // 클라이언트 연결 종료 예외 - 복구 불가능
                    FD_CLR((*it)->get_fd(), &_master_fds);
                    delete *it;
                    it = _clients.erase(it);
                }
                catch (...)
                { // 그 외 예외 - 일단 에러 메시지 출력 후 클라이언트 연결 종료
                    FD_CLR((*it)->get_fd(), &_master_fds);
                    delete *it;
                    it = _clients.erase(it);
                }
            }
            try
            {
                if (FD_ISSET(_server_fd, &_read_fds)) // accept event
                    accept_new_client();
            }
            catch (SocketStream::SystemCallException &e)
            { // 소켓 시스템 콜 예외 - 복구 불가능
                std::cerr << "Error: " << e.what() << std::endl;
            }
            catch (...)
            { // 그 외 예외 - 일단 에러 메시지 출력 후 클라이언트 연결 종료
                std::cerr << "Unkown Error: " << strerror(errno) << std::endl;
            }
        }
    }
}

void Server::accept_new_client()
{
    Client *client = new Client(_server_fd); // 새로운 클라이언트 생성
    FD_SET(client->get_fd(),
           &_master_fds);    // 새로운 클라이언트를 _master에 추가
    _clients.insert(client); // 새로운 클라이언트를 _clients에 추가
}

void Server::read_client(Client *client)
{
    try
    {
        // 클라이언트 내부의 소켓으로부터 데이터를 버퍼로 읽어들임
        client->recv();

        // 클라이언트의 버퍼로부터 데이터를 읽어들임
        // 개행 단위로 처리를 위해 컨테이너로 반환하도록 구현 필요
        std::string msg = client->read_buffer();

        { // 메시지 처리 임시 하드코딩
            std::stringstream ss(msg);

            std::string command;

            ss >> command;

            if (command == "CAP")
            {
                capability(client, ss);
            }
            else if (command == "PASS")
            {
                int password;
                ss >> password;
                register_client(client, password);
            }
            else if (command == "NICK")
            {
                std::string nickname;
                ss >> nickname;
                set_nickname(client, nickname);
            }
            else if (command == "JOIN")
            {
                *client << "461 * JOIN :Not enough parameters\r\n";
            }
            else
            {
                // *client << ERR_UNKNOWNCOMMAND_STR;
            }
        }

        std::cout << "from " << client->get_fd() << ": [" << msg << "]" << std::endl;
    }
    catch (SocketStream::NoNewlineException &e)
    { // 개행 문자 예외 - 무시
        return;
    }
    catch (SocketStream::MessageTooLongException &e)
    { // 메시지 길이 예외 - 무시
        *client << ERR_MSGTOOLONG_STR;
        return;
    }
}

void Server::write_client(Client *client)
{
    // 클라이언트의 버퍼에 있는 데이터를 소켓으로 전송
    client->send();
}

void Server::capability(Client *client, std::stringstream &ss)
{
    std::string cap_cmd;

    ss >> cap_cmd;

    if (cap_cmd == "LS")
    {
        std::cout << "ls" << std::endl;

        std::string cap_version;
        ss >> cap_version;

        std::cout << cap_version << std::endl;
        if (cap_version == "302")
        {
            // *client << "CAP * LS :multi-prefix\r\n";
            *client << "CAP * LS :\r\n";
        }
        else
        {
            // *client << ERR_UNKNOWNCAPVERSION_STR;
        }
    }
    else if (cap_cmd == "REQ")
    {
        // *client << RPL_CAP_REQ_STR;
    }
    else if (cap_cmd == "END")
    {
        // *client << RPL_CAP_END_STR;
    }
    else
    {
        // *client << ERR_UNKNOWNCAPCMD_STR;
    }
}

std::vector<std::string> Server::split_tokens(const std::string &str, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void Server::register_client(Client *client, std::string msg)
{
    std::vector<std::string> tokens = split_tokens(msg, ' ');
    if (tokens.size() != 2)
    {
        *client << ERR_NEEDMOREPARAMS_STR;
        return;
    }
    if (client->get_registraion())
    {
        *client << ERR_ALREADYREGISTRED_STR;
        return;
    }
    if (tokens[1] == _password)
    {
        client->set_register();
        std::cout << "Client " << client->get_fd() << " registered" << std::endl;
    }
}

void Server::set_nickname(Client *client, std::string msg)
{
    std::vector<std::string> tokens = split_tokens(msg, ' ');

    if (tokens.size() == 1)
    {
        *client << ERR_NONICKNAMEGIVEN;
        return;
    }
    else if (tokens.size() == 2){
        for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
        {
            if ((*it)->get_nickname() == tokens[1])
            {
                *client << ERR_NICKNAMEINUSE;
                return;
            }
        }
        client->set_nickname(tokens[1]);
    }
}

void Server::set_userinfo(Client *client, std::string msg)
{
    std::vector<std::string> tokens = split_tokens(msg, ' ');

    if (tokens.size() != 5)
    {
        *client << ERR_NEEDMOREPARAMS_STR;
        return;
    }
    if (client->get_registraion())
    {
        for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
        {
            if ((*it)->get_username() == tokens[1])
            {
                *client << ERR_ALREADYREGISTRED;
                return;
            }
        }
        client->set_userinfo(tokens[1], tokens[2], tokens[3], tokens[4]);
        return;
    }
}

void Server::join_channel(Client *client, std::string msg)
{
    std::vector<std::string> tokens = split_tokens(msg, ' ');

    if (tokens.size() == 1)
    {
        *client << ERR_NEEDMOREPARAMS;
        return;
    }
    if (tokens.size() == 2 || tokens.size() == 3)
    {
        std::vector<std::string> channel_tokens = split_tokens(tokens[1], ',');
        // if vector size is more than the limit ERR_TOOMANYCHANNELS
        // 따로 참여할 수 있는 최대 채널 수를 정해야 할 듯
        if (tokens.size() == 3)
            std::vector<std::string> key_tokens = split_tokens(tokens[2], ',');
        for (std::vector<std::string>::iterator it = channel_tokens.begin(); it != channel_tokens.end(); it++)
        {
            if ((*it).size() < 2 || ((*it)[0] != '#' && (*it)[0] != '&'))
            {
                *client << ERR_NOSUCHCHANNEL;
                return;
            }
            // if channel is not exist or empty, create new channel
                // if there is a key, create channel with key
                // else create channel
            // if channel exists
                // if channel is invite only ERR_INVITEONLYCHAN
                // if channel is full ERR_CHANNELISFULL
                // if there is a key, check key
                    // if key is wrong ERR_BADCHANNELKEY
                    // else join channel
                // else join channel
        }
    }
}

Server::Server() : _port(0), _password(""), _server_fd(0) {}

Server::Server(const Server &src)
    : _port(src._port), _password(src._password), _server_fd(src._server_fd) {}

Server &Server::operator=(const Server &src)
{
    _port = src._port;
    _password = src._password;
    _server_fd = src._server_fd;
    return *this;
}

Server::~Server() { close(_server_fd); }

// Path: srcs/Server.cpp
