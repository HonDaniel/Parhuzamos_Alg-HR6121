#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int is_unique(int arr[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (arr[i] == arr[j])
            {
                return 0;
            }
        }
    }
    return 1;
}

int main()
{
    srand(time(NULL));
    int size = 5;
    int arr[size];

    for (int i = 0; i < size; i++)
    {
        arr[i] = i + 1;
    }

    for (int i = 0; i < size; i++)
    {
        if (rand() % 2 == 0)
        {
            int j = rand() % size;
            arr[i] = arr[j];
        }
    }

    if (is_unique(arr, size))
    {
        printf("Minden elem egyedi.\n");
    }
    else
    {
        printf("Nem egyediek az elemek.\n");
    }

    return 0;
}
