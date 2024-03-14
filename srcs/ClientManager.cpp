#include "../includes/ClientManager.hpp"

ClientManager::ClientManager() {
	std::cout << PINK << CLIENT_PREFIX << "ClientManager created" << RESET << std::endl;
}

ClientManager::~ClientManager() {
	std::cout << PINK << CLIENT_PREFIX << "ClientManager destroyed" << RESET << std::endl;
}

void ClientManager::add_client(int sockfd, struct sockaddr_in client_addr) {
	clients[sockfd].nickname = "Anonymous";
	clients[sockfd].client_addr = client_addr;
}

void ClientManager::set_nickname(int sockfd, std::string nickname) {
	clients[sockfd].nickname = nickname;

	std::cout << PINK << CLIENT_PREFIX << nickname << " entered the server" << RESET << std::endl;
}

std::string ClientManager::get_nickname(int sockfd) {
	return clients[sockfd].nickname;
}

void ClientManager::remove_client(int sockfd) {
	std::string nickname = clients[sockfd].nickname;
	clients.erase(sockfd);

	std::cout << PINK << CLIENT_PREFIX << nickname << " quit the server" << RESET << std::endl;
}
