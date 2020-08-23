#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


static pthread_mutex_t muteks1;
static pthread_mutex_t muteks2;
static pthread_mutex_t muteks3;


/* Telo niti 1. */
void* print1 (void *pParam)
{
    int i;


    for (i = 0 ; i < 100 ; i++)
    {
pthread_mutex_lock(&muteks1);
        printf("1");
        fflush(stdout);
pthread_mutex_unlock(&muteks2);
    }

    return 0;
}

/* Telo  niti 2. */
void* print2 (void *pParam)
{
    int i;


    for (i = 0 ; i < 100 ; i++)
    {
pthread_mutex_lock(&muteks2);
        printf("2");
        fflush(stdout);
pthread_mutex_unlock(&muteks3);	
    }
    return 0;
}

/* Telo niti 3. */
void* print3 (void *pParam)
{
    int i;

    for (i = 0 ; i < 100 ; i++)
    {
	pthread_mutex_lock(&muteks3);
        printf("3 );
        fflush(stdout);
	pthread_mutex_unlock(&muteks1);
    }

    return 0;
}

int main (void)
{
    pthread_t hPrint1;
    pthread_t hPrint2;
    pthread_t hPrint3;


    if (pthread_mutex_init(&muteks1, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    if (pthread_mutex_init(&muteks2, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }
    if (pthread_mutex_init(&muteks3, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }


    /* Formiranje niti. */

    pthread_create(&hPrint1, NULL, print1, 0);
    pthread_create(&hPrint2, NULL, print2, 0);
    pthread_create(&hPrint3, NULL, print3, 0);

    getchar();

    return 0;
}
