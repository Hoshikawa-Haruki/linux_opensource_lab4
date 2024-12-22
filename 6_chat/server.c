#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h> // 추가: errno 및 EINTR 사용을 위해 필요

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS], max_sd, sd, activity;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    socklen_t addrlen; // 추가: addrlen 정의

    // 클라이언트 소켓 초기화
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 주소 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("소켓 바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 리스닝 시작
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버가 포트 %d에서 실행 중...\n", PORT);

    while (1) {
        // 파일 디스크립터 집합 초기화
        FD_ZERO(&readfds);

        // 서버 소켓 추가
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // 클라이언트 소켓 추가 
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i]; // 소켓 할당
            if (sd > 0) {
                FD_SET(sd, &readfds); // 읽기 집합에 추가
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // 활동 감지
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("select 실패");
        }

        // 새로운 연결 수락
        if (FD_ISSET(server_fd, &readfds)) {
            addrlen = sizeof(address); // addrlen 설정
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
                perror("연결 수락 실패");
                exit(EXIT_FAILURE);
            }

            printf("새로운 연결, 소켓 FD: %d, IP: %s, 포트: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // 클라이언트 목록에 추가
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("클라이언트가 소켓 %d에 추가됨\n", new_socket);
                    break;
                }
            }
        }

        // 클라이언트 소켓에서 활동 처리
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // 클라이언트 연결 종료
                    addrlen = sizeof(address); // addrlen 설정
                    getpeername(sd, (struct sockaddr *)&address, &addrlen);
                    printf("클라이언트가 연결 종료됨, IP: %s, 포트: %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    // 메시지 방송
                    buffer[valread] = '\0';
                    printf("클라이언트 메시지: %s", buffer);

                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sockets[j] != 0 && client_sockets[j] != sd) {
                            send(client_sockets[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
