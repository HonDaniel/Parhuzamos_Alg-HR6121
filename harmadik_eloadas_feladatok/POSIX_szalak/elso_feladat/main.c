#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const int N_THREADS = 1;

void *work(void *_)
{
    Sleep(4);
    printf("Ready\n");
}

int main(int argc, char *argv[])
{
    pthread_t threads[N_THREADS];
    int i;
    for(i = 0; i<N_THREADS; ++i)
    {
        pthread_create(&threads[i], NULL, work, NULL);
    }

    for(i=0;i<N_THREADS;++i)
    {
        pthread_join(threads[i], NULL);
    }

    Sleep(8);
    printf("Ready Main\n");
    return 0;
}
