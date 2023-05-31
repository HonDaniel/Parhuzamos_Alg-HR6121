#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const int N_THREADS = 10;
int numberOfPrimes[10];

int primeCheck(int start, int end);
void printArray(int array[], int length);

void *work(void *param)
{
    int i = (int)param;
    numberOfPrimes[i] = primeCheck(100*i, 100*(i+1));
    printf("Ready\n");
}

int main(int argc, char *argv[])
{
    pthread_t threads[N_THREADS];
    int i;
    clock_t start = clock();
    for(i = 0;i<N_THREADS;++i)
    {
        pthread_create(&threads[i], NULL, work, (void *)i);
    }

    for(i = 0;i<N_THREADS;++i)
    {
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();
    printArray(numberOfPrimes, 10);

    printf("\nReady Main\n");

    printf("Time: %lf s", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

int primeCheck(int start, int end)
{
    int counter = 0;
    int ok;
    for(int i = start; i<end; i++)
    {
        ok=1;
        if(i==0 || i==1)
        {
            ok=0;
        }

        for(int j=2;j<=i/2;j++)
        {
            if(i%j==0)
            {
                ok=0;
                break;
            }
        }

        if(ok)
        {
            counter++;
        }
    }
    return counter;
}

void printArray(int array[], int length)
{
    for (int i=0;i<length;i++)
    {
        printf("%d, ", array[i]);
    }
}
