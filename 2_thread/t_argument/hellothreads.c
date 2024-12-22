/* hellothreads.c*/
/*pthread argument passing example*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 3
void *hello_thread(void *arg)
{
    printf("Thread %d:Hello,World!\n", arg);
    return arg;
}
int main()
{
    pthread_t tid[NUM_THREADS];
    int i, status;
    /* 쓰레드생성*/
    for (i = 0; i < NUM_THREADS; i++)
    {
        status = pthread_create(&tid[i], NULL, // 인자 전달
                                hello_thread, (void *)i);
        if (status != 0)
        {
            fprintf(stderr, "Createthread %d:%d", i, status);
            exit(1);
        }
    }
    pthread_exit(NULL);
}