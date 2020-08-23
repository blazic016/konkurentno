#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RING_SIZE  (16)
#define SLEEPING_TIME (400000)

char getch(void);

/*  Kruzni bafer - FIFO. */
struct RingBuffer
{
    unsigned int  tail;
    unsigned int  head;
    unsigned char data[RING_SIZE];
};

/* Operacije za rad sa kruznim baferom. */
char ringBufGetChar (struct RingBuffer *apBuffer)
{
    int index;
    index = apBuffer->head;
    apBuffer->head = (apBuffer->head + 1) % RING_SIZE;
    return apBuffer->data[index];
}

void ringBufPutChar (struct RingBuffer *apBuffer, const char c)
{
    apBuffer->data[apBuffer->tail] = c;
    apBuffer->tail = (apBuffer->tail + 1) % RING_SIZE;
}

/* Globalne promenljive. */

/* Kruzni bafer. */
static struct RingBuffer ringIn;
static struct RingBuffer ringOut;

static sem_t semEmptyIn;
static sem_t semEmptyOut;
static sem_t semFullIn;
static sem_t semFullOut;
static sem_t semFinishSignal;

static pthread_mutex_t bufferAccessIn;
static pthread_mutex_t bufferAccessOut;

/* Funkcija programske niti proizvodjaca. */
void* producer (void *param)
{
    char c;

    while (1)
    {
        if (sem_trywait(&semFinishSignal) == 0)
        {
            break;
        }

        if (sem_trywait(&semEmptyIn) == 0)
        {

            /* Funkcija za unos karaktera sa tastature. */
            c = getch();

            /* Ukoliko je unet karakter q ili Q signalizira se
               programskim nitima zavrsetak rada. */
            if (c == 'q' || c == 'Q')
            {
                /* Zaustavljanje niti; Semafor se oslobadja 2 puta
                da bi signaliziralo obema nitima.*/
                sem_post(&semFinishSignal);
                sem_post(&semFinishSignal);
                sem_post(&semFinishSignal);
            }

            /* Pristup kruznom baferu. */
            pthread_mutex_lock(&bufferAccessIn);
            ringBufPutChar(&ringIn, c);
            pthread_mutex_unlock(&bufferAccessIn);

            sem_post(&semFullIn);
        }
    }

    return 0;
}

/* Funkcija programske niti potrosaca.*/
void* consumer (void *param)
{
    char c;

    printf("Znakovi preuzeti iz kruznog bafera:\n");

    while (1)
    {
        if (sem_trywait(&semFinishSignal) == 0)
        {
            break;
        }

        if (sem_trywait(&semFullOut) == 0)
        {
            /* Pristup kruznom baferu. */
            pthread_mutex_lock(&bufferAccessOut);
            c = ringBufGetChar(&ringOut);
            pthread_mutex_unlock(&bufferAccessOut);

            /* Ispis na konzolu. */
            printf("%c", c);
            fflush(stdout);

            /* Cekanje da bi se ilustrovalo trajanje obrade. */
            usleep(SLEEPING_TIME);

            sem_post(&semEmptyOut);
        }
    }

    return 0;
}

/* Funkcija programske niti potrosaca.*/
void* processing (void *param)
{
    char c;

    printf("Znakovi preuzeti iz In kruznog bafera:\n");

    while (1)
    {
        if (sem_trywait(&semFinishSignal) == 0)
        {
            break;
        }

        if (sem_trywait(&semFullIn) == 0 && sem_trywait(&semEmptyOut) == 0)
        {

            pthread_mutex_lock(&bufferAccessIn);
            c = ringBufGetChar(&ringIn);
            pthread_mutex_unlock(&bufferAccessIn);

            /* Pristup kruznom baferu. */
            pthread_mutex_lock(&bufferAccessOut);
            c = (char)((int)c - 0x20);
            ringBufPutChar(&ringOut, c);
            pthread_mutex_unlock(&bufferAccessOut);


            /* Cekanje da bi se ilustrovalo trajanje obrade. */
            usleep(SLEEPING_TIME);

            sem_post(&semEmptyIn);
            sem_post(&semFullOut);
        }
    }

    return 0;
}






/* Glavna programska nit koja formira dve programske (proizvodjac i potrosac) niti i ceka njihovo gasenje. */
int main (void)
{
    /* Identifikatori niti. */
    pthread_t hProducer;
    pthread_t hConsumer;
    pthread_t hProcessing;

    /* Formiranje semEmpty, semFull i semFinishSignal semafora. */
    sem_init(&semEmptyIn, 0, RING_SIZE);
    sem_init(&semEmptyOut, 0, RING_SIZE);
    sem_init(&semFullIn, 0, 0);
    sem_init(&semFullOut, 0, 0);
    sem_init(&semFinishSignal, 0, 0);

    /* Inicijalizacija objekta iskljucivog pristupa. */
    pthread_mutex_init(&bufferAccessIn, NULL);
    pthread_mutex_init(&bufferAccessOut, NULL);

    /* Formiranje programskih niti: proizodjac i potrosac. */
    pthread_create(&hProducer, NULL, producer, 0);
    pthread_create(&hConsumer, NULL, consumer, 0);
    pthread_create(&hProcessing, NULL, processing, 0);

    /* Cekanje na zavrsetak formiranih niti. */
    pthread_join(hConsumer, NULL);
    pthread_join(hProducer, NULL);

    /* Oslobadjanje resursa. */
    sem_destroy(&semEmptyIn);
    sem_destroy(&semEmptyOut);
    sem_destroy(&semFullIn);
    sem_destroy(&semFullOut);
    sem_destroy(&semFinishSignal);
    pthread_mutex_destroy(&bufferAccessIn);
    pthread_mutex_destroy(&bufferAccessOut);

    printf("\n");

    return 0;
}

