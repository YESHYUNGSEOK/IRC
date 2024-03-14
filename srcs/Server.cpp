#include "../includes/Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password) {
    std::cout << CYAN << SERVER_PREFIX << "Server created with port: " << port << ", password: " << password << RESET << std::endl;

    serv_addr_init();
	socket_init();
	fd_set_init();
}

Server::~Server() {
	close(sockfd); // 서버 소켓 닫기

    std::cout << CYAN << SERVER_PREFIX << "Server is closed" << RESET << std::endl;
}

int Server::get_sockfd() {
	return sockfd;
}

int Server::get_port() {
    return port;
}

std::string Server::get_password() {
    return password;
}

sockaddr_in Server::get_serv_addr() {
    return serv_addr;
}

void Server::serv_addr_init() {
    std::memset(&serv_addr, 0, sizeof(serv_addr)); // 구조체를 0으로 초기화
    serv_addr.sin_family = AF_INET; // 주소 체계를 IPv4로 설정
    serv_addr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스의 주소를 사용
    serv_addr.sin_port = htons(port); // 호스트 바이트 순서를 네트워크 바이트 순서로 변환

	std::cout << CYAN << SERVER_PREFIX << "Server address initialized" << RESET << std::endl;
}

void Server::socket_init() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성

	if (sockfd == ERROR) { // 소켓 생성 실패 시
		std::cerr << RED << SERVER_PREFIX << "Error opening socket" << RESET << std::endl;
		exit(EXIT_FAILURE); // 오류 메시지 출력 후 프로그램 종료
	}

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == ERROR) { // 소켓에 주소 할당
		std::cerr << RED << SERVER_PREFIX << "Error on binding" << RESET << std::endl;
		exit(EXIT_FAILURE); // 바인딩 실패 시 오류 메시지 출력 후 종료
	}

	if (listen(sockfd, MAX_CLIENTS) == ERROR) { // 연결 요청 대기열 설정
		std::cerr << RED << SERVER_PREFIX << "Error on listening" << RESET << std::endl;
		exit(EXIT_FAILURE); // 리스닝 실패 시 오류 메시지 출력 후 종료
	}

	std::cout << CYAN << SERVER_PREFIX << "Server socket initialized" << RESET << std::endl;
}

void Server::fd_set_init() {
	FD_ZERO(&current_sockets); // current_sockets 세트를 0으로 초기화
	FD_SET(sockfd, &current_sockets); // 소켓 파일 디스크립터를 current_sockets 세트에 추가

	std::cout << CYAN << SERVER_PREFIX << "File descriptor set initialized" << RESET << std::endl;
}

void Server::run() {
	std::cout << CYAN << SERVER_PREFIX << "Server is running..." << RESET << std::endl;

	int max_fd = sockfd; // 최대 파일 디스크립터 번호 초기화

 	while (true) { // 무한 루프
        ready_sockets = current_sockets; // ready_sockets를 current_sockets로 복사
        if (select(max_fd + 1, &ready_sockets, nullptr, nullptr, nullptr) == ERROR) { // 준비된 소켓 검사. 변화 없으면 블로킹
            std::cerr << "Error on select" << std::endl;
            exit(EXIT_FAILURE); // select 실패 시 오류 메시지 출력 후 종료
        }

        for (int i = 0; i <= max_fd; i++) { // 준비된 모든 파일 디스크립터 확인
            if (FD_ISSET(i, &ready_sockets)) { // 파일 디스크립터가 준비되었는지 확인
                if (i == sockfd) { // 새로운 연결 요청이면
					std::cout << YELLOW << SERVER_PREFIX << "New connection request :-)" << RESET << std::endl;
                    struct sockaddr_in cli_addr; // 클라이언트 주소 정보 구조체
                    socklen_t clilen = sizeof(cli_addr); // 주소 정보의 크기
                    int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // 연결 수락
                    if (newsockfd == ERROR) { // 연결 수락 실패 시
                        std::cerr << "Error on accept" << std::endl;
                        continue; // 다음 반복으로 넘어감
                    }
                    FD_SET(newsockfd, &current_sockets); // 새 소켓을 파일 디스크립터 세트에 추가
                    max_fd = std::max(max_fd, newsockfd); // 최대 파일 디스크립터 번호 업데이트
                } else { // 기존 연결에서 데이터가 도착한 경우
                    char buffer[1024]; // 데이터 수신을 위한 버퍼
                    int nbytes = read(i, buffer, sizeof(buffer)); // 데이터 읽기
                    if (nbytes <= 0) { // 읽기 실패 또는 연결 종료
                        close(i); // 소켓 닫기
                        FD_CLR(i, &current_sockets); // 세트에서 소켓 제거
                    } else {
						for (int j = 0; j <= max_fd; j++) {
							if (j != i && j != sockfd && FD_ISSET(j, &current_sockets)) { 
                        		if (write(j, buffer, nbytes) == ERROR) { // 받은 데이터를 다른 소켓에 전송
									std::cerr << "Error on write" << std::endl;
								}	
							}
						}
                    }
                }
            }
        }
    }
}