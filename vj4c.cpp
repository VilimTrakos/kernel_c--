#include <iostream>
#include <thread>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int URED = 0;
int ULAZ = 1;
int IZLAZ = 2;

int ID_programera;
int N;

pthread_mutex_t mon;
pthread_cond_t red[2];
int Microsoft = 0, Linux = 1;
int broj_u_restoranu[2] = {0,0};
int broj_koliko_ceka[2] = {0,0};
int siti;

void ispisi_redove (int vrsta, int radnja) {
	int i = 0;
	cout << "Red Linux: ";
	while (i++<broj_koliko_ceka[Linux]) 
		cout << "L";
	while (i++<=ID_programera) 
		cout << "-";
	
	i = 0;
	cout << " Red Microsoft: ";
	while (i++<broj_koliko_ceka[Microsoft]) 
		cout << "M";
	while (i++<=ID_programera) 
		cout << "-";
	
	i = 0;
	cout << " Restoran: ";
	int brojac;
	if(broj_u_restoranu[Microsoft]!=0)
		brojac = broj_u_restoranu[Microsoft];
	else brojac = broj_u_restoranu[Linux];
	
	while (i++<brojac) 
		if (broj_u_restoranu[Microsoft]!=0)
			cout << "M";
		else
			cout << "L";
	
	cout << "-->";
	if (vrsta == Microsoft) 
		cout << "M";
	else 
		cout << "L";
	cout << " ";
	if (radnja == URED) 
		cout << "Ulazi u red" << endl;
	else if (radnja == ULAZ) 
		cout << "Ulazi u restoran" << endl;
	else if (radnja == IZLAZ)
		cout << "Izlazi iz restorana" << endl;
}

void udji (int vrsta) {
	pthread_mutex_lock(&mon);
	broj_koliko_ceka[vrsta]++;
	bool bio_u_redu = false;
	while (broj_u_restoranu[1-vrsta]>0 || (siti>=N && broj_koliko_ceka[1-vrsta]>0)) {
		if (bio_u_redu == false) {
			ispisi_redove (vrsta, URED);
			bio_u_redu = true;
		}
		pthread_cond_wait(&red[vrsta], &mon);
	}
	
	broj_u_restoranu[vrsta]++;
	broj_koliko_ceka[vrsta]--;
	ispisi_redove (vrsta, ULAZ);
	if (broj_koliko_ceka[1-vrsta]>0) siti++;
	pthread_mutex_unlock(&mon);
}

void izadji(int vrsta) {
	pthread_mutex_lock(&mon);
	broj_u_restoranu[vrsta]--;
	ispisi_redove (vrsta, IZLAZ);
	if (broj_u_restoranu[vrsta] == 0) {
		siti = 0;
		pthread_cond_broadcast(&red[1-vrsta]);
	}
	pthread_mutex_unlock(&M);
}

void *rucak (void *arg) {
	int vrsta = *(int*)arg;
	usleep(rand()%200);
	udji(vrsta);
	usleep(rand()%200+300);
	izadji(vrsta);
}

int main (int argc, char *argv[]) {
	if(argv[1] == NULL || argv[2] == NULL) {
		cout << "Unesite broj sitosti i programera! Program se zavrsava!" << endl;
		return 0;
	}
	
	srand(time(0));
	N = strtol(argv[1], NULL, 0);
	ID_programera = strtol(argv[2], NULL, 0);
	
	pthread_mutex_init(&mon, NULL); 
	pthread_cond_init(&red[Linux], NULL);
	pthread_cond_init(&red[Microsoft], NULL);
	
	pthread_t *lin = new pthread_t[ID_programera];
	pthread_t *micro = new pthread_t[ID_programera];
	
	for (int i = 0, micro_br = 0, lin_br = 0; i<ID_programera*2; i++ ){
		if (i%2 == 0)
			pthread_create(&lin[lin_br++], NULL, rucak, &Linux);
		else 
			pthread_create(&micro[micro_br++], NULL, rucak, &Microsoft);
	}
	
	for (int i = 0; i < ID_programera; i++)
		pthread_join(lin[i], NULL);
	for (int i = 0; i < ID_programera; i++)
		pthread_join(micro[i], NULL);
	
	pthread_mutex_destroy(&mon);
	pthread_cond_destroy(&red[Linux]);
	pthread_cond_destroy(&red[Microsoft]);
	
	return 0;
}
