#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <cstdlib>
#include <time.h>
using namespace std;


pthread_mutex_t monitor;
pthread_cond_t cekaj_potpis;
pthread_cond_t cekaj_proizvod;
pthread_t nakupci[3];

int proizvedeni_br;

bool proizvedeno = false;
bool potpisano = true;

void *nakupac(void *br)
{
    int br_nakupca = *(int *)br;
    while (1)
    {
        //ulazak U KO
        pthread_mutex_lock(&monitor);
        while(proizvedeno == false || proizvedeni_br!=br_nakupca)
            pthread_cond_wait(&cekaj_proizvod,&monitor);
        proizvedeno = false;
        pthread_mutex_unlock(&monitor);

        //KO
        if (proizvedeni_br == 0)
            printf("Nakupac s tipkovnicom uzima monitor i racunalo\n");
        if (proizvedeni_br == 1) 
            printf("Nakupac s racunalom uzima monitor i tipkovnicu\n");
        if (proizvedeni_br == 2)
            printf("Nakupac s monitorom uzima tipkovnicu i racunalo\n");
        sleep(1);

        //izlazak iz KO
        pthread_mutex_lock(&monitor);
        potpisano = true; 
        pthread_cond_broadcast(&cekaj_potpis);
        pthread_mutex_unlock(&monitor);
    }
}

void veletrgovac()
{
    while (1)
    {
        //ulazak U KO
        pthread_mutex_lock(&monitor);
        while(potpisano == false)
            pthread_cond_wait(&cekaj_potpis,&monitor);
        potpisano = false;
        pthread_mutex_unlock(&monitor);
        
        //KO
        srand(time(0));
        proizvedeni_br = rand() % 3;
        if (proizvedeni_br == 0)
            printf("Veletrgovac stavio monitor i racunalo\n");
        if (proizvedeni_br == 1)
            printf("Veletrgovac stavio monitor i tipkovnicu\n");
        if (proizvedeni_br == 2)
            printf("Veletrgovac stavio tipkovnicu i racunalo\n");
        sleep(1);
        
        //izlazak iz KO
        pthread_mutex_lock(&monitor);
        proizvedeno = true;
        pthread_cond_broadcast(&cekaj_proizvod);
        pthread_mutex_unlock(&monitor);
    }
}

void kraj(int sig)
{
    
    exit(sig);
}

int main()
{
    pthread_mutex_init(&monitor, NULL);
    pthread_cond_init(&cekaj_potpis, NULL);
    pthread_cond_init(&cekaj_proizvod, NULL);

    int br_nakupca_p[3];

    sigset(SIGINT, kraj);

    for (int i = 0; i < 3; i++)
    {
        br_nakupca_p[i] = i;
        pthread_create(nakupci + i, NULL, nakupac, &br_nakupca_p[i]);
    }
    veletrgovac();
    for (int i = 0; i < 3; i++)
    {
        pthread_join(nakupci[i], NULL);
    }
    kraj(0);
}