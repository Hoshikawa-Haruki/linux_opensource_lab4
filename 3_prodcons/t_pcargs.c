#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10 // 제한된 버퍼 크기

int buffer[BUFFER_SIZE];     // 공유 자원 (버퍼)
int count = 0;               // 현재 버퍼의 항목 수
int in = 0;                  // 버퍼의 삽입 위치
int out = 0;                 // 버퍼의 제거 위치

pthread_mutex_t mutex;       // 뮤텍스
pthread_cond_t cond_full;    // 버퍼가 가득 참 조건변수
pthread_cond_t cond_empty;   // 버퍼가 비어 있음 조건변수

// 생산자 함수
void *producer(void *arg) {
    int id = *(int *)arg;
    free(arg); // 동적 메모리 해제

    while (1) {
        int item = rand() % 100; // 생산할 데이터 생성

        pthread_mutex_lock(&mutex); // 뮤텍스 잠금

        // 버퍼가 가득 찬 경우 대기
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_full, &mutex);
        }

        // 버퍼에 데이터 삽입
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Producer %d: Produced %d | Buffer size: %d\n", id, item, count);

        pthread_cond_signal(&cond_empty); // 소비자에게 신호
        pthread_mutex_unlock(&mutex);    // 뮤텍스 해제

        sleep(1); // 생산 속도 조절
    }
    return NULL;
}

// 소비자 함수
void *consumer(void *arg) {
    int id = *(int *)arg;
    free(arg); // 동적 메모리 해제

    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금

        // 버퍼가 비어 있는 경우 대기
        while (count == 0) {
            pthread_cond_wait(&cond_empty, &mutex);
        }

        // 버퍼에서 데이터 제거
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer %d: Consumed %d | Buffer size: %d\n", id, item, count);

        pthread_cond_signal(&cond_full); // 생산자에게 신호
        pthread_mutex_unlock(&mutex);   // 뮤텍스 해제

        sleep(1); // 소비 속도 조절
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[1]); // 생산자 수
    int num_consumers = atoi(argv[2]); // 소비자 수

    if (num_producers <= 0 || num_consumers <= 0) {
        fprintf(stderr, "Error: Both producer and consumer counts must be greater than 0.\n");
        return 1;
    }

    pthread_t producers[num_producers], consumers[num_consumers];

    // 뮤텍스 및 조건변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_full, NULL);
    pthread_cond_init(&cond_empty, NULL);

    // 생산자 쓰레드 생성
    for (int i = 0; i < num_producers; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&producers[i], NULL, producer, id);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < num_consumers; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&consumers[i], NULL, consumer, id);
    }

    // 쓰레드 종료 대기 (무기한 실행되는 프로그램이므로 실제로는 종료되지 않음)
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    // 뮤텍스 및 조건변수 제거
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_full);
    pthread_cond_destroy(&cond_empty);

    return 0;
}
