#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

using namespace std;

#define kolicina_procesa 2

int memorija, *PRAVO, *ZASTAVICA;

void udi_u_kriticni_odsjecak (int i, int j) {
	ZASTAVICA[i] = 1;
	while (ZASTAVICA[j] != 0) {
		if (PRAVO[0] == j) {
			ZASTAVICA[i] = 0;
		while (PRAVO[0] == j) {}
		ZASTAVICA[i] = 1;
		}
	}
}

void izadi_iz_kriticnog_odsjecka (int i, int j) {
	PRAVO[0] = j;
	ZASTAVICA[i] = 0;
}

void proc (int i) {
	int j;
	if (i == 1) j = 0;
	else if (i != 1) j = 1;
		for (int k = 1; k <= 5; k++) {
			sleep(1);
			udi_u_kriticni_odsjecak (i,j);
			for (int m = 1; m <= 5; m++) {
				cout << "Proces: " << i+1 << ", K.O. br: " << k << " (" << m << "/5)" << endl;
				sleep(1);
			}
			izadi_iz_kriticnog_odsjecka (i,j);
		}
}

void ukloni (int signal) {
	shmdt(PRAVO);
	shmctl(memorija, IPC_RMID, NULL);
	exit(0);
}

int main(void) {
	memorija = shmget(IPC_PRIVATE, sizeof(int)*10, 0600);
	
	if (mjesto == -1) {
		cout << "Greska u stvaranju zajednickog prostora!" << endl;
		exit(1);
	}
	
	sigset (SIGINT, ukloni);
	PRAVO = (int*) shmat(mjesto, NULL, 0);
	ZASTAVICA = PRAVO + sizeof(PRAVO);
	
	for (int i = 0; i < 2; i++) {
		switch (fork()) {
			case 0: 
				proc(i);
				exit(0);
				break;
			case -1:
				cout << "Greska u stvaranju novog procesa!" << endl;
				exit(1);
				break;
		}
	}
	
	for (int i = 0; i < kolicina_procesa; i++)
		wait(NULL);
	ukloni(0);
	return 0;
}
