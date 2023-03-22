#include <stdio.h>
#include<stdlib.h>
#include<time.h>

int main() {
    int upper=1000;
    int lower=500;

    srand(time(0));

    printf("Random integer: %d\n", (rand()%(upper-lower)+1)+lower);
    printf("Random float: %f", ((upper - lower)*((float)rand()/RAND_MAX))+lower);
    return 0;
}
