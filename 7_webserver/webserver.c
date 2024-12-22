#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3490
#define QLEN 10
#define BUF_SIZE 1024

void handle_clnt(int client_sock);
void send_err(int client_sock);
void send_msg(int client_sock);

int main() {
    int sockfd, new_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t alen = sizeof(client_addr);
    fd_set readfds, activefds;
    int maxfd, i;

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    // 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 바인딩
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    // 리스닝
    if (listen(sockfd, QLEN) < 0) {
        perror("listen() failed");
        exit(1);
    }

    // 파일 디스크립터 집합 초기화
    FD_ZERO(&activefds);
    FD_SET(sockfd, &activefds);
    maxfd = sockfd;

    printf("Server running on port %d...\n", PORT);

    while (1) {
        readfds = activefds;

        // select 호출로 이벤트 감지
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select() failed");
            exit(1);
        }

        // 모든 파일 디스크립터 검사
        for (i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &readfds)) {
                if (i == sockfd) {
                    // 새로운 연결 수락
                    if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &alen)) < 0) {
                        perror("accept() failed");
                        continue;
                    }
                    FD_SET(new_fd, &activefds);
                    if (new_fd > maxfd) maxfd = new_fd;
                    printf("New connection accepted (fd: %d)\n", new_fd);
                } else {
                    // 클라이언트 요청 처리
                    handle_clnt(i);
                    close(i);
                    FD_CLR(i, &activefds);
                }
            }
        }
    }

    close(sockfd);
    return 0;
}

void handle_clnt(int client_sock) {
    char buf[BUF_SIZE];
    char method[10];

    // 요청 수신
    int str_len = read(client_sock, buf, BUF_SIZE);
    if (str_len < 0) {
        perror("read() failed");
        return;
    }

    buf[str_len] = '\0';
    sscanf(buf, "%s", method);

    // 요청 메소드 확인
    if (strcmp(method, "GET") == 0) {
        send_msg(client_sock);
    } else if (strcmp(method, "POST") == 0) {
        handle_post(client_sock, buf);
    } else {
        send_err(client_sock);
    }
}

void handle_post(int client_sock, const char *request) {
    // 요청 본문 추출
    char *body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4; // 헤더와 본문을 구분하는 "\r\n\r\n" 이후로 이동
    } else {
        body = "";
    }

    printf("POST 요청 본문: %s\n", body);

    // POST 응답 생성
    const char *html =
        "<html><head>Post Response</head><body><h1>POST 데이터 수신</h1></body></html>";
    int content_length = strlen(html);

    char response[BUF_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n\r\n%s",
             content_length, html);

    write(client_sock, response, strlen(response));
}

void send_err(int client_sock) {
    const char *response =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 50\r\n\r\n"
        "<html><body><h1>400 Bad Request</h1></body></html>";
    write(client_sock, response, strlen(response));
}

void send_msg(int client_sock) {
    const char *html =
        "<html><head>Hello World</head><body><h1>Hello World</h1></body></html>";
    int content_length = strlen(html); // 정확한 콘텐츠 길이 계산

    char response[BUF_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n\r\n%s",
             content_length, html);

    write(client_sock, response, strlen(response));
}

