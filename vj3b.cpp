#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <semaphore.h>
#include <csignal>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>


using namespace std;

struct memori
{
    int ULAZ, IZLAZ, PISI;
    int PUN, UKUPNO, PRAZAN;
    int M[5];
} *str;

struct memori *memorija;

int br_procesa_proizvodaca;
int br_brojeva;
int sem_ID;
int memId;

void kreiraj()
{
    memId = shmget(IPC_PRIVATE, sizeof(str), 0600);
    str = (memori *)shmat(memId, NULL, 0);
    sem_ID = semget(IPC_PRIVATE, 3, 0600);
}

int Opr_sem(int br_sem, int operacija)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = br_sem;
    sem_buf.sem_op = operacija;
    sem_buf.sem_flg = 0;
    return semop(sem_ID, &sem_buf, 1);
}

void kraj(int signal)
{
    semctl(sem_ID, IPC_RMID, NULL);
    shmdt((char *)memorija);
    shmctl(memId, IPC_RMID, NULL);
    exit(signal);
}

void potrosac()
{
    int i = 0;
    int zbroj = 0;
    while (i < br_procesa_proizvodaca * str->UKUPNO)
    {
        Opr_sem(str->PRAZAN, -1);
        
        zbroj += str->M[str->IZLAZ];
        printf("Potrocas prima '%i'\n", str->M[str->IZLAZ]);
        sleep(1);
        str->IZLAZ = ++str->IZLAZ % 5;
        Opr_sem(str->PUN, 1);
        i++;
    }
    printf("Potrosac - zbroj svih brojeva = %i\n", zbroj);
    exit(0);
}

void proizvodac(int i)
{

    int j = 0;
    while (j < str->UKUPNO)
    {

        Opr_sem(str->PUN, -1);
        Opr_sem(str->PISI, -1);
        srand(time(0));
        str->M[str->ULAZ] = rand() % (999 + 1);
        printf("Proizvodac %i salje '%i'\n", i, str->M[str->ULAZ]);
        sleep(1);
        str->ULAZ = (++str->ULAZ) % 5;
        Opr_sem(str->PISI, 1);
        Opr_sem(str->PRAZAN, 1);
        j++;
    }
    printf("Proizvodac %i je zavrsio sa proizvodnjom\n", i);
    exit(0);
}

int main(int argc, char *argv[])
{
    br_procesa_proizvodaca = atoi(argv[1]);
    br_brojeva = atoi(argv[2]);

    kreiraj();

    str->UKUPNO = br_brojeva;
    str->ULAZ = 0;
    str->IZLAZ = 0;
    str->PUN = 0;
    str->PISI = 1;
    str->PRAZAN = 2;
    semctl(sem_ID, 0, SETVAL, 5);
    semctl(sem_ID, 1, SETVAL, 1);
    semctl(sem_ID, 2, SETVAL, 0);

    sigset(SIGINT, kraj);

    for (int i = 1; i < br_procesa_proizvodaca + 1; i++)
    {
        if (fork() == 0)
        {
            proizvodac(i);
        }
    }
    if (fork() == 0)
    {
        potrosac();
    }

    for (int i = 0; i < br_procesa_proizvodaca + 1; i++)
    {
        wait(NULL);
    }
    kraj(0);
    return 0;
}
