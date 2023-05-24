#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <csignal>
#include <cstring>
#include <semaphore.h>
using namespace std;

sem_t Sem3;
sem_t Sem4;
sem_t Sem5;
sem_t Sem6;
sem_t Sem7;
sem_t Sem8;
sem_t Sem9;

pthread_t broj_dretvi[7];

void kraj(int sig)
{

    sem_destroy(&Sem3);
    sem_destroy(&Sem4);
    sem_destroy(&Sem5);
    sem_destroy(&Sem6);
    sem_destroy(&Sem7);
    sem_destroy(&Sem8);
    sem_destroy(&Sem9);
    exit(sig);
}

void KO(int i)
{

    int vrijeme_obrade = rand() % 10;

    srand(time(0));

    for (int j = 1; j <= vrijeme_obrade; j++)
    {
        printf("Obrađujem zadatak %i (%i/%i) \n", i, j, vrijeme_obrade);
        sleep(1);
    }
}

void *zadatak(void *arg)
{
    int i = *((int *)arg);

    if (i == 1)
    {
        KO(i);
        sem_post(&Sem5);
    }
    if (i == 2)
    {
        KO(i);
        sem_post(&Sem3);
        sem_post(&Sem4);
    }
    if (i == 3)
    {
        sem_wait(&Sem3);
        KO(i);
        sem_post(&Sem6);
    }

    if (i == 4)
    {
        sem_wait(&Sem4);
        KO(i);
        sem_post(&Sem8);
    }
    if (i == 5)
    {
        sem_wait(&Sem5);
        KO(i);
        sem_post(&Sem9);
    }

    if (i == 6)
    {
        sem_wait(&Sem6);
        KO(i);
        sem_post(&Sem7);
    }
    if (i == 7)
    {
        sem_wait(&Sem7);
        sem_wait(&Sem8);
        sem_wait(&Sem9);
        KO(i);
    }
}

int main()
{

    sem_init(&Sem3, 0, 0);
    sem_init(&Sem4, 0, 0);
    sem_init(&Sem5, 0, 0);
    sem_init(&Sem6, 0, 0);
    sem_init(&Sem7, 0, 0);
    sem_init(&Sem8, 0, 0);
    sem_init(&Sem9, 0, 0);

    int polje_i[8];

    sigset(SIGINT, kraj);

    for (int i = 1; i <= 7; i++)
    {
        polje_i[i] = i;
        pthread_create(&broj_dretvi[i], NULL, zadatak, &polje_i[i]);
    }
    for (int i = 1; i <= 7; i++)
    {
        pthread_join(broj_dretvi[i], NULL);
    }

    kraj(0);
}