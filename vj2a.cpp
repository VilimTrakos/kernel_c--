#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <wait.h>
#include <csignal>

using namespace std;

int **A;
int **B;
int(*C);

int l, n, m;
int id_mem;

int broj_pod_procesa;
int izvr;

void gasenje_programa(int i)
{

    //cout << "Gasenje programa!" << endl;
    for (int i = 0; i < l; i++)
    {
        delete[] A[i];
    }
    for (int i = 0; i < m; i++)
    {
        delete[] B[i];
    }
    delete[] A;
    delete[] B;

    shmdt(C);
    shmctl(id_mem, IPC_RMID, NULL);

    exit(0);
}

void racunaj(int i)
{
    int start = (((l / n)) * i);
    int kraj = start + ((l / n));

    izvr = start;
    while (izvr < kraj)
    {
        int j = 0;
        while (j < l)
        {

            int k = 0;
            int suma = 0;
            while (m > k)
            {
                suma += A[izvr][k] * B[k][j];
                k++;
            }
            C[izvr * l + j] = suma;
            //cout << "Proces(" << i << ") -"
            //     << "C[" << izvr * l + j << "]=" << C[izvr * l + j] << endl;

            j++;
        }
        izvr++;
    }
    usleep(25);
}

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        cout << "Programu treba proslijediti 3 argumenta!(l,m,n)" << endl;
        return 0;
    }
    l = atoi(argv[1]);
    m = atoi(argv[2]);
    n = atoi(argv[3]);

    if (l < 2)
    {
        cout << "l je manji od 2 - nije moguće stvoriti 2d matricu!" << endl;
        return 0;
    }
    if (m < 2)
    {
        cout << "m je manji od 2 - nije moguće stvoriti 2d matricu!" << endl;
        return 0;
    }
    if (n < 1)
    {
        cout << "n je manji od 1 - nema podprocesa za daljnju obradu!" << endl;
        return 0;
    }

    A = new int *[l];
    for (int i = 0; i < l; i++)
    {
        A[i] = new int[m];
    }

    B = new int *[m];
    for (int i = 0; i < m; i++)
    {
        B[i] = new int[l];
    }

    id_mem = shmget(IPC_PRIVATE, sizeof(l * l * sizeof(int)), 0600);
    

    C = (int *)shmat(id_mem, NULL, 0);

    sigset(SIGINT, gasenje_programa);

    srand(time(0));
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < m; j++)
        {
            A[i][j] = rand() % 10;
        }
    }
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < l; j++)
        {
            B[i][j] = rand() % 10;
        }
    }
     //A[0][0] = 8;
     //A[0][1] = 7;
     //A[0][2] = 8;
     //A[1][0] = 7;
     //A[1][1] = 1;
     //A[1][2] = 0;
     //A[2][0] = 5;
     //A[2][1] = 6;
     //A[2][2] = 2;
     //A[3][0] = 2;
     //A[3][1] = 3;
     //A[3][2] = 2;
     //
     //B[0][0] = 6;
     //B[0][1] = 3;
     //B[0][2] = 8;
     //B[0][3] = 0;
     //B[1][0] = 7;
     //B[1][1] = 9;
     //B[1][2] = 4;
     //B[1][3] = 6;
     //B[2][0] = 6;
     //B[2][1] = 2;
     //B[2][2] = 6;
     //B[2][3] = 9;


    cout << "A = " << endl;
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    cout << "B = " << endl;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < l; j++)
        {
            cout << B[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    broj_pod_procesa = l / n;

    if (l % n == 1)
    {
        broj_pod_procesa = l / n + 1;
    }

    for (int i = 0; i <= broj_pod_procesa; i++)
    {

        switch (fork())
        {
        case 0:
        {
            racunaj(i);
            exit(0);
        }
        case -1:
        {
            cout << "Greska kod stvaranja podprocesa!" << endl;
            exit(1);
        }
        }
    }
    for (int i = 0; i < broj_pod_procesa; i++)
        wait(NULL);
    cout << "C = " << endl;
    for (int i = 1; i <= l * l; i++)
    {
        cout << C[i - 1] << " ";
        if (i % l == 0)
        {
            cout << endl;
        }
    }
    cout << endl;

    gasenje_programa(0);
    return 0;
}