#include <iostream>
#include <cstdlib>
#include <ctime>
#include <wait.h>
#include <csignal>
#include <pthread.h>
#include <stdio.h>

#define broj_dretvi 2

using namespace std;

int PRAVO, *ZASTAVICA;
pthread_t *th_polje;

void udi_u_kriticni_odsjecak(int i, int j)
{
    ZASTAVICA[i] = 1;
    while (ZASTAVICA[j] != 0)
    {
        if (PRAVO== j)
        {
            ZASTAVICA[i] = 0;
            while (PRAVO == j)
            {
                //nista
            }
            ZASTAVICA[i] = 1;
        }
    }
}

void izadi_iz_kriticnog_odsjecka(int i, int j)
{
    PRAVO = j;
    ZASTAVICA[i] = 0;
}

void *dretva(void *arg)
{
    int i = *((int *)arg); // i = dretva
    //cout << "Dretva(" << i << ")" << endl;
    int j;
    if (i == 1)
        j = 0;
    else if (i != 1)
        j = 1;
    for (int k = 1; k <= 5; k++)
    {
        sleep(1);
        udi_u_kriticni_odsjecak(i, j);
        for (int m = 1; m <= 5; m++)
        {
            cout << "Dretva: " << i + 1 << ", K.O. br: " << k << " (" << m << "/5)" << endl;
            sleep(1);
        }
        izadi_iz_kriticnog_odsjecka(i, j);
    }
}

void ukloni(int sig)
{

    if (!sig)
    {
        for (int i = 0; i < broj_dretvi; i++)
        {
            pthread_join(th_polje[i], NULL);
        }
    }
    else
    {
        for (int i = 0; i < broj_dretvi; i++)
        {
            pthread_kill(th_polje[i], SIGKILL);
        }
    }
    exit(0);
}

int main()
{

    ZASTAVICA = new int[broj_dretvi];

    sigset(SIGINT, ukloni);

    th_polje = new pthread_t[broj_dretvi];

    int *polje_i = new int[broj_dretvi];

    for (int i = 0; i < broj_dretvi; i++)
    {
        polje_i[i] = i;
        //cout << "Dretva " << i << " je kreirana" << endl;
        pthread_create(&th_polje[i], NULL, dretva, &polje_i[i]);
    }
    ukloni(0);
}