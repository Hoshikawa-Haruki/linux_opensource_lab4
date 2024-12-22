#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 3 // 쓰레드 수

pthread_mutex_t mutex; // 뮤텍스 선언
int sum = 0;           // 공유 변수

void *mutex_thread(void *arg) {
    pthread_mutex_lock(&mutex); // 뮤텍스 잠금
    sum += (int)arg;            // 공유 변수에 값 추가
    pthread_mutex_unlock(&mutex); // 뮤텍스 해제
    return arg;                 // 반환값 전달
}

int main(int argc, char *argv[]) {
    pthread_t tid[NUM_THREADS]; // 쓰레드 ID 배열
    int arg[NUM_THREADS], i;
    void *result;
    int status;

    // 명령줄 인수 확인
    if (argc < 4) {
        fprintf(stderr, "Usage: mutexthread number1 number2 number3\n");
        exit(1);
    }

    // 명령줄 인수를 정수로 변환하여 배열에 저장
    for (i = 0; i < NUM_THREADS; i++) {
        arg[i] = atoi(argv[i + 1]);
    }

    // 뮤텍스 초기화
    pthread_mutex_init(&mutex, NULL);

    // 쓰레드 생성
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&tid[i], NULL, mutex_thread, (void *)arg[i]);
        if (status != 0) {
            fprintf(stderr, "Create thread %d: %d\n", i, status);
            exit(1);
        }
    }

    // 쓰레드 종료 대기
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(tid[i], &result);
        if (status != 0) {
            fprintf(stderr, "Join thread %d: %d\n", i, status);
            exit(1);
        }
    }

    // 뮤텍스 파괴
    status = pthread_mutex_destroy(&mutex);
    if (status != 0) {
        perror("Destroy mutex");
    }

    // 최종 결과 출력
    printf("Sum is %d\n", sum);

    pthread_exit(result);
}
