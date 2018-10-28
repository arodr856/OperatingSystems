#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "passenger.h"

passenger *p = (passenger *) malloc(sizeof(passenger)); // global

sem_t *air_sem; // semaphore

int main(int argc, char **argv) {

	// parameters passed at exectution.
	int numOfPas = atoi(argv[1]);
	int numOfBag = atoi(argv[2]);
	int numOfSec = atoi(argv[3]);
	int numOfAtt = atoi(argv[4]);

	p = createPassengers(numOfPass); // created struct of passengers

//	int passNum = p[0].number;	// test to we can get number element from passenger	

	// thread creationg for bag handler
	int numBReturned[numOfBag]; // number returned for bag handler threads
	pthread_t bThread[numOfBag]; // create an array of bag handler threads
    for (int i = 0; i < numOfBag; i++) {
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_create(&tids[i], &attr, sum_runner, &args[i]);
         if (numBReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for secuirty screener
	int numSReturned[numOfSec]; // number returned for security screener threads
	pthread_t sThread[numOfPass]; // create an array of secuirty screener threads
    for (int i = 0; i < numOfSec; i++) {
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_create(&tids[i], &attr, sum_runner, &args[i]);
         if (numSReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for flight attendents
	int numAReturned[numOfAtt]; // number returned for security screener threads
	pthread_t aThread[numOfAtt]; // create an array of secuirty screener threads
    for (int i = 0; i < numOfAtt; i++) {
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_create(&tids[i], &attr, sum_runner, &args[i]);
         if (numAReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for passengers
	int numPReturned[numOfPas]; // number returned for passenger  threads
	pthread_t pThread[numOfPas]; // create an array of passenger threads
    for (int i = 0; i < numOfPas; i++) {
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_create(&tids[i], &attr, sum_runner, &args[i]);
         if (numPReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }



//  int numBReturned[numOfBag];
//  pthread_t bThread[numOfBag];
  
// int numSReturned[numOfSec];
//  pthread_t sThread[numOfSec];

//int numAReturned[numOfAtten];
//  pthread_t aThread[numOfAtten];

	return 0;
}
