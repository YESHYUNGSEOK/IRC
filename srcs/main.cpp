#include <iostream> // 표준 입력/출력 스트림을 위한 헤더 파일
#include <cstring> // 메모리 조작 함수를 위한 헤더 파일
#include <sys/socket.h> // 소켓 프로그래밍을 위한 헤더 파일
#include <netinet/in.h> // 인터넷 프로토콜을 위한 헤더 파일
#include <unistd.h> // 유닉스 표준 함수를 위한 헤더 파일

#define ERROR -1 // 에러 상수를 정의

int main(int argc, char **argv) { // 메인 함수 시작
    if (argc == 3) { // 인자의 개수가 정확히 3개인지 확인 (프로그램 이름, 포트, 패스워드)
        int port = std::stoi(argv[1]); // 첫 번째 인자(포트 번호)를 정수로 변환
        std::string password = argv[2]; // 두 번째 인자(패스워드)를 문자열로 저장

        struct sockaddr_in serv_addr; // 서버 주소 정보를 담을 구조체 선언
        std::memset(&serv_addr, 0, sizeof(serv_addr)); // 구조체를 0으로 초기화
        serv_addr.sin_family = AF_INET; // 주소 체계를 IPv4로 설정
        serv_addr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스의 주소를 사용
        serv_addr.sin_port = htons(port); // 호스트 바이트 순서를 네트워크 바이트 순서로 변환

        int sockfd = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
        if (sockfd == ERROR) { // 소켓 생성 실패 시
            std::cerr << "Error opening socket" << std::endl;
            return EXIT_FAILURE; // 오류 메시지 출력 후 프로그램 종료
        }

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == ERROR) { // 소켓에 주소 할당
            std::cerr << "Error on binding" << std::endl;
            return EXIT_FAILURE; // 바인딩 실패 시 오류 메시지 출력 후 종료
        }

        if (listen(sockfd, 5) == ERROR) { // 연결 요청 대기열 설정
            std::cerr << "Error on listening" << std::endl;
            return EXIT_FAILURE; // 리스닝 실패 시 오류 메시지 출력 후 종료
        }

        fd_set current_sockets, ready_sockets; // 파일 디스크립터 세트 초기화
        FD_ZERO(&current_sockets); // current_sockets 세트를 0으로 초기화
        FD_SET(sockfd, &current_sockets); // 소켓 파일 디스크립터를 current_sockets 세트에 추가
        int max_fd = sockfd; // 최대 파일 디스크립터 번호 초기화

        while (true) { // 무한 루프
            ready_sockets = current_sockets; // ready_sockets를 current_sockets로 복사
            if (select(max_fd + 1, &ready_sockets, nullptr, nullptr, nullptr) == ERROR) { // 준비된 소켓 검사
                std::cerr << "Error on select" << std::endl;
                return EXIT_FAILURE; // select 실패 시 오류 메시지 출력 후 종료
            }

            for (int i = 0; i <= max_fd; i++) { // 준비된 모든 파일 디스크립터 확인
                if (FD_ISSET(i, &ready_sockets)) { // 파일 디스크립터가 준비되었는지 확인
                    if (i == sockfd) { // 새로운 연결 요청이면
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
                            write(i, buffer, nbytes); // 에코: 받은 데이터를 클라이언트에게 다시 보냄
                        }
                    }
                }
            }
        }

        close(sockfd); // 서버 소켓 닫기
        return EXIT_SUCCESS; // 성공적으로 프로그램 종료
    } else {
        std::cout << "Correct usage is ./ircserv [port] [password]" << std::endl; // 올바른 사용법 안내
        return EXIT_FAILURE; // 인자가 부족할 때 프로그램 종료
    }
}
