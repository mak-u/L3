#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TEMPS_PAR_PRODUCTION 1

int capacite_max_entrepot();

typedef struct thread_data_t {
	int pid;
	int *stock;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} thread_data_t ;


void *activite_usine(void *pdata){
  
  while(1){
	thread_data_t *stockL = (thread_data_t *)(pdata);
  	printf("Usine : \t");
  	if(*stockL->stock < capacite_max_entrepot()) {
  		stockL->stock--;
    	printf("Production d'une voiture\n");
  }
  else {
  	printf("Pas de production d'une voiture\n");
  }
  sleep(TEMPS_PAR_PRODUCTION);
  	}
}


int creation_usine(pthread_t *t,thread_data_t *stockL){
  /* Creation de la usine */

  return pthread_create(t,
			NULL,
			activite_usine,
			stockL);
}