#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* print (void *pParam)
{ 
	/* Sredi kao prethodni primer! */
    char c = *(char*)pParam;
    int i;

    for(i = 0; i < 1000; i++)
    {
        printf("%c", c);
        fflush(stdout);
    }

    return 0;
}

int main (void)
{
    pthread_t hPrint1;
    pthread_t hPrint2;
    pthread_t hPrint3;


	/* Ovako ne treba raditi. */
/*  char c;
    c = '1';
    pthread_create(&hPrint1, NULL, print, (void*)&c);
    c = '2';
    pthread_create(&hPrint2, NULL, print, (void*)&c);
    c = '3';
    pthread_create(&hPrint3, NULL, print, (void*)&c);
*/


	/* Ovako treba, c se prenosi po referenci. */
    char c1='1', c2='2', c3='3';
    pthread_create(&hPrint1, NULL, print, (void*)&c1);
    pthread_create(&hPrint2, NULL, print, (void*)&c2);
    pthread_create(&hPrint3, NULL, print, (void*)&c3);



    getchar();

    return 0;
}
