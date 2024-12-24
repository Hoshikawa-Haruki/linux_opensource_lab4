#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

// 서버가 사용할 포트와 백로그 및 최대 버퍼 크기 정의
#define MYPORT 5000  // 서버 포트 번호
#define BACKLOG 10   // 연결 대기열 크기 (UDP에서는 사용되지 않음)
#define MAXBUF 100   // 버퍼 크기

int main(){
	int sock;
	struct sockaddr_in serv_addr;  // 서버 주소 정보 구조체
	struct sockaddr_in clnt_addr;  // 클라이언트 주소 정보 구조체
	int clnt_addr_len;
	char buf[MAXBUF];
	int sin_size;

	// 서버 소켓 생성 (UDP)
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	// 서버 주소 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;  // IPv4
	serv_addr.sin_port = htons(MYPORT);  // 포트 설정 (호스트 바이트 순서를 네트워크 바이트 순서로)
	serv_addr.sin_addr.s_addr = INADDR_ANY;  // 모든 로컬 인터페이스에 바인드

	// 소켓에 주소를 바인드
	if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) == -1){
		perror("bind");
		close(sock);  // 소켓을 닫아 자원 해제
		exit(1);
	}

	// 클라이언트 요청을 처리하는 루프
	while(1){
		// 메시지 수신
		clnt_addr_len = sizeof(clnt_addr);
		memset(buf, 0, MAXBUF);
		if(recvfrom(sock, buf, MAXBUF, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_len) == -1){
			perror("recvfrom");
			close(sock);  // 소켓을 닫아 자원 해제
			exit(1);
		}

		printf("server : got connection from %s\n", inet_ntoa(clnt_addr.sin_addr));

		// 메시지 송신 (에코)
		if(sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr)) == -1){
			perror("sendto");
			close(sock);  // 소켓을 닫아 자원 해제
			exit(1);
		}
	}

	// 루프를 벗어나면 소켓 종료
	close(sock);
	return 0;
}