#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <semaphore.h>
#include <csignal>
#include <cstdio>

using namespace std;
int m;

sem_t *sem_generiraj, *sem_procitaj;
long sluc_broj;

void *generiraj (void *arg) {
	cout << "Dretva koja generira zadatke pocela je s radom. Broj zadataka=" << m+1 << endl;
	while (m > 0) {
		sluc_broj = rand()%1000000000+1;
		cout << "Generiran broj " << sluc_broj << endl;
		sem_post(sem_generiraj);
		sem_wait(sem_procitaj);
		m--;
	}
	return NULL;
}

void *racunaj (void *arg) {
	int redni_broj_dretve = *((int*)arg);
	long zbroj = 0;
	long ulaz;
	bool flag = true;
	while (m) {
		zbroj = 0;
		sem_wait(sem_generiraj);
		ulaz = sluc_broj;
		sem_post(sem_procitaj);
		if (flag == true) {
			cout << "Dretva " << redni_broj_dretve << " pocela je s radom." << endl;
			flag = false;
		}
		cout << "Dretva " << redni_broj_dretve << ". preuzela zadatak " << ulaz << endl;
		for (long j = 1; j < ulaz; j++) {
			zbroj += j;
		}
		cout << "Dretva " << redni_broj_dretve << ". zadatak=" << ulaz << " zbroj=" << zbroj << endl;
	}
	return NULL;
}

void brisi (int sig) {
	sem_destroy(sem_generiraj);
	sem_destroy(sem_procitaj);
	exit(0);
}

int main (int argc, char *argv[]) {
	if (argc < 3) {
		cout << "Ulazni parametri neispravni!" << endl;
		return 0;
	}
	
	int n = atoi(argv[1]);
	m = atoi(argv[2]);
	
	sem_generiraj = new sem_t;
	sem_procitaj = new sem_t;
	sem_init(sem_generiraj, 0, 0);
	sem_init(sem_procitaj, 0, 0);
	
	sigset (SIGINT, brisi);
	
	srand(time(0));
	pthread_t dretva_generiraj;
	pthread_create (&dretva_generiraj, NULL, generiraj, NULL);
	
	pthread_t *dretve = new pthread_t[n];
	int redni_broj_dretvi[n];
	
	for (int i = 0; i < n; i++) {
		redni_broj_dretvi[i] = i+1;
		pthread_create (dretve+i, NULL, racunaj, &redni_broj_dretvi[i]);
	}
	pthread_join(dretva_generiraj, NULL);
	for (int i = 0; i < n; i++) {
		pthread_join (dretve[i], NULL);
	}
	brisi (0);
	return 0;
}
