#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // 표준 입력/출력 스트림을 위한 헤더 파일
#include <cstring> // 메모리 조작 함수를 위한 헤더 파일
#include <sys/socket.h> // 소켓 프로그래밍을 위한 헤더 파일
#include <netinet/in.h> // 인터넷 프로토콜을 위한 헤더 파일
#include <unistd.h> // 유닉스 표준 함수를 위한 헤더 파일
#include <fcntl.h> // 파일 제어 함수를 위한 헤더 파일

#include "ClientManager.hpp"

#define ERROR -1 // 에러 상수를 정의
#define BUF_SIZE 1024 // 버퍼 크기를 정의

#define CYAN "\033[36m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define SERVER_PREFIX "[SERVER] "

class Server {
	private:
		int server_sockfd;
		int port;
		std::string password;
		struct sockaddr_in serv_addr;
		fd_set current_sockets, ready_sockets;

		ClientManager client_manager;

		void serv_addr_init(); // 서버 주소 정보 초기화 함수
		void socket_init(); // 소켓 초기화 함수. listen까지 수행
		void fd_set_init(); // 파일 디스크립터 세트 초기화 함수
	public:
		Server(int port, std::string password);
		~Server();
		int get_sockfd();
		int get_port();
		std::string get_password();
		sockaddr_in get_serv_addr();
		void run(); // 멀티플렉싱을 이용한 서버 실행 함수

		// 클라이언트 관리
		void add_client(int sockfd, int *max_fd);
		void remove_client(int sockfd);
		void set_client_nickname(int sockfd, std::string nickname);
};

#endif