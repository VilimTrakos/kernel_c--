#include<iostream>
#include<pthread.h>
#include<signal.h>
#include<semaphore.h>
using namespace std;

pthread_mutex_t monitor;
pthread_cond_t cond;
sem_t semafor;
int br_dretvi, uneseno_br =0;

void ispis(int br_dretve,int vrijednost_ispisa,int uneseno_br){

    pthread_mutex_lock(&monitor);
    
    if(uneseno_br!=br_dretvi)
    {   
        pthread_cond_wait(&cond,&monitor);
    }
    else
    {
        pthread_cond_broadcast(&cond);
        
    }
    printf("Dretva %i. - uneseni broj: %i\n",br_dretve,vrijednost_ispisa);
    pthread_mutex_unlock(&monitor);

}

void*dretva(void*br_dretve){

    int br_t =*(int*)br_dretve;
    int unesena_vrijednost = 0;
    sem_wait(&semafor);
    printf("Dretva %i. unesite broj:\n",br_t);
    cin>>unesena_vrijednost;
    uneseno_br++;
    sem_post(&semafor);
        
    ispis(br_t,unesena_vrijednost,uneseno_br);

}
void kraj(int sig){

    pthread_mutex_destroy(&monitor);
    pthread_cond_destroy(&cond);
    sem_destroy(&semafor);
    exit(0);
}

int main (int argc, char*argv[]) {
    
    sigset(SIGINT,kraj);
    if(argc!=2){
        printf("Previse / nedovoljno unjetih argumenata!\n");
        return 0;
    }
    else
        br_dretvi = atoi(argv[1]);
    
    pthread_mutex_init(&monitor,NULL);
    pthread_cond_init(&cond,NULL);
    sem_init(&semafor,0,1);
    pthread_t dretve[br_dretvi];
    
    int zapis[br_dretvi];
    for(int i=1;i<br_dretvi+1;i++)
    {
        zapis[i]=i;
        pthread_create(dretve+i,NULL,dretva,&zapis[i]);
        
    }
    for(int i=1;i<br_dretvi+1;i++)
    {
        pthread_join(dretve[i],NULL);
    }
    
    kraj(0);

}