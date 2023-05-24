#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <pthread.h>

using namespace std;

unsigned int l, m, n, trenutna = 0, kolicnik = 0;
int **A, **B, **C;

void alociraj_prostor () {
	A = new int *[l];
	for (int i = 0; i < l; i++) {
		A[i] = new int [m];
	}
	B = new int *[m];
	for (int i = 0; i < m; i++) {
		B[i] = new int [l];
	}
	C = new int *[l];
	for (int i = 0; i < l; i++) {
		C[i] = new int [l];
	}
}

void generiraj_brojeve () {
	srand(time(NULL));
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < m; j++) {
			A[i][j] = rand() % 10;
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < l; j++) {
			B[i][j] = rand() % 10;
		}
	}
}

void dealociraj_prostor () {
	for (int i = 0; i < l; i++) {
		delete [] A[i];
	}
	for (int i = 0; i < m; i++) {
		delete [] B[i];
	}
	for (int i = 0; i < l; i++) {
		delete [] C[i];
	}
	delete [] A;
	delete [] B;
	delete [] C;
}

void *thr (void *arg) {
	if (trenutna > 0) {
		kolicnik = trenutna + l/n;
	}
	if (trenutna + 1 == n && l%n) {
		kolicnik = l;
	}
	while (kolicnik > trenutna) {
		int i = 0;
		while (i < l) {
			int j = 0;
			C[trenutna][i] = 0;
			while (m > j) {
				C[trenutna][i] = C[trenutna][i] + A[trenutna][j] * B[j][i];
				j++;
			}
			i++;
		}
		trenutna++;
	}
	return arg;
}

void ispisi () {
	cout << "Mnozenik = " << endl;
	for (int i = 0; i < l; i++) {
		for(int j = 0; j < m; j++) {
			cout << A [i][j] << " ";
		}
		cout << endl;
	}
	cout << "Mnozitelj = " << endl;
	for (int i = 0; i < m; i++) {
		for(int j = 0; j < l; j++) {
			cout << B [i][j] << " ";
		}
		cout << endl;
	}
	cout << "Umnozak = " << endl;
	for (int i = 0; i < l; i++) {
		for(int j = 0; j < l; j++) {
			cout << C [i][j] << " ";
		}
		cout << endl;
	}
}

int main (int argc, char **argv) {
	if (argc < 4) {
		return -1;
	}
	stringstream (argv[1]) >> l;
	stringstream (argv[2]) >> m;
	stringstream (argv[3]) >> n;
	if (l < 1 || m < 1 || n < 1) {
		return -1;
	}
	if (l < n) {
		return -1;
	}
	kolicnik = l/n;
	if (l%n != 0) kolicnik++;
	alociraj_prostor ();
	generiraj_brojeve ();
	pthread_t thr_id[n];
	for (int i = 0; i < n; i++) {
		pthread_create(&thr_id[i], NULL, thr, NULL);
	}
	for (int i = 0; i < n; i++) {
		pthread_join(thr_id[i], NULL);
	}
	ispisi();
	dealociraj_prostor();
	return 0;
}
