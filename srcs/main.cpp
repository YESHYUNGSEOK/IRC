#include "Server.hpp"

int main(int argc, char **argv) {
    if (argc == 3) { // 인자의 개수가 정확히 3개인지 확인 (프로그램 이름, 포트, 패스워드)
        int port = std::stoi(argv[1]);
        std::string password = argv[2];

		Server server(port, password); // 서버 객체 생성 및 초기화

		server.run(); // 서버 실행

        return EXIT_SUCCESS;
    } else {
        std::cout << YELLOW << "[ERROR] Correct usage is ./ircserv [port] [password]" << RESET << std::endl;
        return EXIT_FAILURE;
    }
}
