#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>

#define ANONYMOUS "Anonymous"

#define PINK "\033[35m"
#define RESET "\033[0m"
#define CLIENT_PREFIX "[CLIENT MANAGER] "

struct client_info
{
	std::string nickname;
	struct sockaddr_in client_addr;
};

class ClientManager
{
private:
	std::map<int, client_info> clients;

public:
	ClientManager();
	~ClientManager();
	void add_client(int sockfd, sockaddr_in client_addr);
	void set_nickname(int sockfd, std::string nickname);
	std::string get_nickname(int sockfd);
	void remove_client(int sockfd);
};

#endif
