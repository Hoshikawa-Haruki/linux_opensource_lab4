#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_CLIENTS 3       // 클라이언트 쓰레드 수
#define BUFFER_SIZE 256     // 메시지 버퍼 크기

pthread_mutex_t mutexes[NUM_CLIENTS];       // 클라이언트별 뮤텍스
pthread_cond_t conds[NUM_CLIENTS];          // 클라이언트별 조건변수
char client_messages[NUM_CLIENTS][BUFFER_SIZE]; // 클라이언트별 메시지 버퍼
int message_ready[NUM_CLIENTS];            // 메시지 준비 상태

char server_message[BUFFER_SIZE];          // 서버에서 받은 메시지
int server_ready = 0;                      // 서버가 메시지를 준비했는지 여부
pthread_mutex_t server_mutex;              // 서버 뮤텍스
pthread_cond_t server_cond;                // 서버 조건변수

// 서버 쓰레드 함수
void *server_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&server_mutex);

        // 클라이언트 메시지 요청 대기
        while (!server_ready) {
            pthread_cond_wait(&server_cond, &server_mutex);
        }

        // 모든 클라이언트에게 메시지 전송
        printf("[서버]: 메시지를 방송합니다: \"%s\"\n", server_message);
        for (int i = 0; i < NUM_CLIENTS; i++) {
            pthread_mutex_lock(&mutexes[i]);
            strncpy(client_messages[i], server_message, BUFFER_SIZE);
            message_ready[i] = 1; // 메시지 준비 완료
            pthread_cond_signal(&conds[i]); // 클라이언트에 신호
            pthread_mutex_unlock(&mutexes[i]);
        }

        // 서버 상태 초기화
        server_ready = 0;
        pthread_mutex_unlock(&server_mutex);

        sleep(1); // 방송 간격 조절
    }
    return NULL;
}

// 클라이언트 쓰레드 함수
void *client_thread(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutexes[id]);

        // 메시지가 준비될 때까지 대기
        while (!message_ready[id]) {
            pthread_cond_wait(&conds[id], &mutexes[id]);
        }

        // 메시지 수신
        printf("[클라이언트 %d]: 서버로부터 메시지 수신: \"%s\"\n", id + 1, client_messages[id]);

        // 메시지 상태 초기화
        message_ready[id] = 0;

        pthread_mutex_unlock(&mutexes[id]);

        sleep(1); // 메시지 처리 시간
    }
    return NULL;
}

// 클라이언트에서 서버로 메시지 요청
void *client_request(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "클라이언트 %d의 메시지입니다.", id + 1);

        pthread_mutex_lock(&server_mutex);

        // 서버로 메시지 설정
        strncpy(server_message, message, BUFFER_SIZE);
        server_ready = 1;
        printf("[클라이언트 %d]: 서버에 메시지 요청: \"%s\"\n", id + 1, message);

        // 서버에 메시지 요청 신호 전달
        pthread_cond_signal(&server_cond);
        pthread_mutex_unlock(&server_mutex);

        sleep(rand() % 3 + 1); // 랜덤 대기
    }
    return NULL;
}

int main() {
    pthread_t server, clients[NUM_CLIENTS], requests[NUM_CLIENTS];

    // 클라이언트별 뮤텍스 및 조건변수 초기화
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
        pthread_cond_init(&conds[i], NULL);
        message_ready[i] = 0; // 초기 메시지 상태는 준비되지 않음
    }

    // 서버 뮤텍스 및 조건변수 초기화
    pthread_mutex_init(&server_mutex, NULL);
    pthread_cond_init(&server_cond, NULL);

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < NUM_CLIENTS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&clients[i], NULL, client_thread, id);

        int *req_id = malloc(sizeof(int));
        *req_id = i;
        pthread_create(&requests[i], NULL, client_request, req_id);
    }

    // 쓰레드 종료 대기 (실제로는 종료되지 않음)
    pthread_join(server, NULL);
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
        pthread_join(requests[i], NULL);
    }

    // 뮤텍스 및 조건변수 제거
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_mutex_destroy(&mutexes[i]);
        pthread_cond_destroy(&conds[i]);
    }
    pthread_mutex_destroy(&server_mutex);
    pthread_cond_destroy(&server_cond);

    return 0;
}
