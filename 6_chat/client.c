#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *sock_fd) {
    int sock = *(int *)sock_fd;
    char buffer[BUFFER_SIZE];

    while (1) {
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("서버로부터 메시지: %s", buffer);
        }
    }

    return NULL;
}

int main() {
    int sock;
    struct sockaddr_in server_address;
    char message[BUFFER_SIZE];
    pthread_t recv_thread;

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // 서버 주소 변환
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("잘못된 주소");
        exit(EXIT_FAILURE);
    }

    // 서버 연결
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결됨\n");

    // 수신 스레드 시작
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&sock);

    // 메시지 전송
    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}
