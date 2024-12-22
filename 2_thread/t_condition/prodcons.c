#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE]; // 공유 자원 (버퍼)
int count = 0;           // 버퍼에 저장된 항목 수

pthread_mutex_t mutex;    // 뮤텍스
pthread_cond_t cond_full; // 버퍼 가득 참 조건변수
pthread_cond_t cond_empty; // 버퍼 비어 있음 조건변수

void *producer(void *arg) {
    int item;   

    while (1) {
        item = rand() % 100; // 임의의 데이터 생성

        pthread_mutex_lock(&mutex); // 뮤텍스 잠금

        while (count == BUFFER_SIZE) { // 버퍼가 가득 찬 경우
            pthread_cond_wait(&cond_full, &mutex); // 대기
        }

        // 데이터 생산
        buffer[count++] = item;
        printf("Produced: %d | Buffer size: %d\n", item, count);

        pthread_cond_signal(&cond_empty); // 소비자에게 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        sleep(1); // 생산 속도 조절
    }
    return NULL;
}

void *consumer(void *arg) {
    int item;

    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금

        while (count == 0) { // 버퍼가 비어 있는 경우
            pthread_cond_wait(&cond_empty, &mutex); // 대기
        }

        // 데이터 소비
        item = buffer[--count];
        printf("Consumed: %d | Buffer size: %d\n", item, count);

        pthread_cond_signal(&cond_full); // 생산자에게 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        sleep(1); // 소비 속도 조절
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    srand(time(NULL)); // 랜덤 시드 초기화

    // 뮤텍스 및 조건변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_full, NULL);
    pthread_cond_init(&cond_empty, NULL);

    // 생산자와 소비자 쓰레드 생성
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    // 쓰레드 종료 대기
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // 뮤텍스 및 조건변수 제거
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_full);
    pthread_cond_destroy(&cond_empty);

    return 0;
}
