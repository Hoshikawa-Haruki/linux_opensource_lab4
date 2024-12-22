#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // atoi, exit

// 쓰레드 함수
void *join_thread(void *arg) {
    pthread_exit(arg); // 쓰레드 종료 시 인수 반환
}

int main(int argc, char *argv[]) {
    pthread_t tid;       // 쓰레드 ID
    int arg, status;     // 입력값과 상태 변수
    void *result;        // 쓰레드 반환값

    // 명령줄 인수 확인
    if (argc < 2) {
        fprintf(stderr, "Usage: jointhread <number>\n");
        exit(1);
    }

    arg = atoi(argv[1]); // 명령줄 입력을 정수로 변환

    // 쓰레드 생성
    status = pthread_create(&tid, NULL, join_thread, (void *)arg);
    if (status != 0) {
        fprintf(stderr, "Create thread: %d\n", status);
        exit(1);
    }

    // 쓰레드 종료 대기 및 반환값 수신
    status = pthread_join(tid, &result);
    if (status != 0) {
        fprintf(stderr, "Join thread: %d\n", status);
        exit(1);
    }

    // 쓰레드 반환값 출력
    printf("Thread returned: %d\n", (int)result);

    return 0;
}
