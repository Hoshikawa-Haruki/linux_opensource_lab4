#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화
sigset_t sigset;                                   // 시그널 집합
int completed = 0;                                 // 완료 플래그

void *signal_thread(void *arg)
{
    int signal;
    int count = 0;

    while (1)
    {
        sigwait(&sigset, &signal); // 시그널 대기
        if (signal == SIGINT)
        {
            printf("Signal thread: Received SIGINT (%d/3)\n", ++count);
            if (count >= 3)
            {
                pthread_mutex_lock(&mutex);
                completed = 1; // 종료 플래그 설정
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int arg;
    int status;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s time(sec)\n", argv[0]);
        exit(1);
    }

    arg = atoi(argv[1]); // 명령줄 입력값을 정수로 변환

    // SIGINT 시그널 설정
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    status = pthread_sigmask(SIG_BLOCK, &sigset, NULL); // 시그널 블록 설정
    if (status != 0)
    {
        fprintf(stderr, "Error setting signal mask\n");
        exit(1);
    }

    // 시그널 처리 스레드 생성
    status = pthread_create(&tid, NULL, signal_thread, NULL);
    if (status != 0)
    {
        fprintf(stderr, "Error creating thread: %d\n", status);
        exit(1);
    }

    while (1)
    {
        sleep(arg); // 지정된 시간 간격으로 대기
        pthread_mutex_lock(&mutex);
        printf("Main thread: Checking completed status\n");
        if (completed)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }

    // 메인 스레드 종료
    printf("Main thread: Exiting program\n");
    pthread_exit(NULL);
}
