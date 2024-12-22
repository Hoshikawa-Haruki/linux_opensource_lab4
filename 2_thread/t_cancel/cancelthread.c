/* cancelthread.c */
/* pthread cancel example */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// 쓰레드 함수
void *cancel_thread(void *arg)
{
    int i, state;
    for (i = 0;; i++)
    {
        /* cancelability 비활성화 */
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
        printf("Thread: cancel state disabled\n");
        sleep(1);
        /* cancelability 복원 */
        pthread_setcancelstate(state, &state);
        printf("Thread: cancel state restored\n");
        if (i % 5 == 0)
        {
            pthread_testcancel(); // 취소 요청 확인
        }
    }
    return arg;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int arg, status;
    void *result;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: cancelthread time(sec)\n");
        exit(1);
    }

    arg = atoi(argv[1]); // 명령줄에서 대기 시간 입력

    // 쓰레드 생성
    status = pthread_create(&tid, NULL, cancel_thread, NULL);
    if (status != 0)
    {
        fprintf(stderr, "Create thread: %d\n", status);
        exit(1);
    }

    sleep(arg); // 지정된 시간만큼 대기

    // 쓰레드 취소 요청
    status = pthread_cancel(tid);
    if (status != 0)
    {
        fprintf(stderr, "Cancel thread: %d\n", status);
        exit(1);
    }

    // 쓰레드 종료 대기 및 결과 확인
    status = pthread_join(tid, &result);
    if (status != 0)
    {
        fprintf(stderr, "Join thread: %d\n", status);
        exit(1);
    }

    // 취소 여부 확인
    if (result == PTHREAD_CANCELED)
    {
        printf("Thread was canceled.\n");
    }
    else
    {
        printf("Thread was not canceled.\n");
    }

    return 0;
}
