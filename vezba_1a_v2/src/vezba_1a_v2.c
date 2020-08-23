#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int flag = 1;
static pthread_mutex_t lock;


/* Telo niti. */
void* print (void *pParam)
{
    int c = *(int*)pParam;
    int i;


    for(i = 0; i < 100;)
    {
	pthread_mutex_lock(&lock);
	if (flag == c) {
	  i++;

          printf("%d", c);
	  fflush(stdout);


	if (flag == 1)
	   flag = 2;
	else if (flag == 2)
	   flag = 3;
	else if (flag == 3)
	   flag = 1;


	}

	pthread_mutex_unlock(&lock);
    }

    return 0;
}

int main (void)
{
    pthread_t hPrint1;
    pthread_t hPrint2;
    pthread_t hPrint3;

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 


    /* Argumenti niti. */
    int c1 = 1;
    int c2 = 2;
    int c3 = 3;


    pthread_create(&hPrint1, NULL, print, (void*)&c1);
    pthread_create(&hPrint2, NULL, print, (void*)&c2);
    pthread_create(&hPrint3, NULL, print, (void*)&c3);

    getchar();

    return 0;
}
