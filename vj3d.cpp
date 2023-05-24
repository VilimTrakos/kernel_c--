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

struct struk {
  int ULAZ;
  int IZLAZ;
  int PISI;
  int PUN;
  int UKUPNO;
  int PRAZAN;
  int M[5];
}*str;

struct struk* memorija;

int Sem_ID;
int shm_ID;

int SemSetVal (int SemNum, int SemVal) {
	return semctl(Sem_ID,SemNum,SETVAL,SemVal);
}

int SemOp (int SemNum, int SemOp) {
	struct sembuf SemBuf;
    SemBuf.sem_num = SemNum;
    SemBuf.sem_op = SemOp;
    SemBuf.sem_flg = 0;
    return semop(Sem_ID,&SemBuf,1);
}

void brisi (int signal) {
    shmdt((char*) memorija);
    shmctl(shm_ID, IPC_RMID, NULL);
    semctl(Sem_ID, IPC_RMID, 0);
}

int main (int argc, char*argv[]) {
    int br_procesa_proizvodaca = atoi(argv[1]);
    int ukupno = atoi(argv[2]);
    Sem_ID = semget(IPC_PRIVATE, 3, 0600);

    if (Sem_ID == -1) {
    	cout << "Nema semafora!" << endl;
        exit(1);
    }
    
    int shm_ID = shmget(IPC_PRIVATE, sizeof(str),0600);
    str = (struk*) shmat(shm_ID,NULL,0);
    if (shm_ID == -1) {
        cout << "Ne postoji proces!" << endl;
        exit(1);
    }

    str->UKUPNO = ukupno;
    str->ULAZ = 0;
    str->IZLAZ = 0;
	str->PUN=0;
    str->PISI=1;
    str->PRAZAN=2;
    SemSetVal(0,5);
    SemSetVal(1,1);
    SemSetVal(2,0);
    
    sigset(SIGINT, brisi);
    
    for( int i = 1; i < br_procesa_proizvodaca+1; i++) {
    	if (fork()==0) {
            for (int j = 0; j < str->UKUPNO; j++) {
                srand(time(0));
                rand()%999+1;
                SemOp(str->PUN,-1);
                SemOp(str->PISI,-1);
                str->M[str->ULAZ]=rand()%(999 + 1);
                cout << "Proizvodac " << i << " salje " << str->M[str->ULAZ] << endl;
                sleep(1);
                str->ULAZ=(++str->ULAZ)%5;
                SemOp(str->PISI,1);
                SemOp(str->PRAZAN,1);
            }
            cout << "Proizvodac " << i << " je zavrsio sa slanjem" << endl;
            exit(0);
        }
    }
    if (fork()==0) {
        int zbroj = 0;
        for (int i = 0; i < (br_procesa_proizvodaca*str->UKUPNO); i++) {
            SemOp(str->PRAZAN,-1);
            cout << "Potrosac prima " << str->M[str->IZLAZ] << endl;
            sleep(1);
            zbroj += str->M[str->IZLAZ];
            str->IZLAZ= ++str->IZLAZ%5;
            SemOp(str->PUN, 1);
        }
        cout << "Potrosac - zbroj primljenih brojeva=" << zbroj << endl;
        exit(0);
    }

    for (int i = 0; i < br_procesa_proizvodaca+1; i++) {
		wait(NULL);
	}
	brisi(0);
    return 0;
}
