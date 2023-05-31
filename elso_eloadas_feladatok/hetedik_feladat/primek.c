#include <stdio.h>
#include <conio.h>
#include <time.h>

int main()
{

	int minimum, maximum, flag, count, i, j;
	FILE *count_primes;
	count_primes = fopen("primek_szama.txt", "w");

	double total_time;
	clock_t start, end;

	for (maximum = 50000; maximum <= 1000000; maximum = maximum + 50000)
	{

		count = 0, minimum = 1;
		start = clock();

		for (i = minimum; i <= maximum; i++)
		{
			flag = 0;

			for (j = 2; j <= i / 2; j++)
			{
				if (i % j == 0)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 0 && i >= 2)
			{
				// printf("%d\t",i);
				count++;
			}
		}
		printf("\n Primek eddig: %d = %d", maximum, count);

		end = clock();

		total_time = ((double)(end - start)) / CLK_TCK;

		fprintf(count_primes, "%d %f\n", maximum, total_time);
	}
	fclose(count_primes);
	return 0;
}
