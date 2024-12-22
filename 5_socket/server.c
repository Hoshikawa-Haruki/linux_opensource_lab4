// tcp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3490
#define BUF_SIZE 100

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buf[BUF_SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Server running on port %d...\n", PORT);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);

    memset(buf, 0, BUF_SIZE);
    recv(client_sock, buf, BUF_SIZE - 1, 0);
    printf("Received from client: %s\n", buf);

    send(client_sock, "Hello, Client!", 15, 0);

    close(client_sock);
    close(server_sock);
    return 0;
}
