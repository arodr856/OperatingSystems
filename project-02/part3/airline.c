#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "passenger.h"



sem_t *air_sem; // semaphore

int main(int argc, char **argv) {

	if (argc != 5) {
		printf("\nPlease use 5 arguments!");
		printf("Format: ./airline <# of Passengers>  <#of Baggage> <# of Security> <# of Flight Attendant>\n");
		return -1;
	}

	// parameters passed at exectution.
	int numOfPas = atoi(argv[1]);
	int numOfBag = atoi(argv[2]);
	int numOfSec = atoi(argv[3]);
	int numOfAtt = atoi(argv[4]);
	
	printf("\n Number of Passengers: %d\n", numOfPas);
	printf("\n Number of Baggage Handlers: %d\n", numOfBag);
	printf("\n Number of Security Screeners: %d\n", numOfSec);
	printf("\n Number of FLight Attendants: %d\n\n", numOfAtt);

	passenger *b = (passenger *) malloc(sizeof(passenger)); // create pointer to passenger 
	passenger *s = (passenger *) malloc(sizeof(passenger)); // create pointer to passenger 
	passenger *a = (passenger *) malloc(sizeof(passenger)); // create pointer to passenger 
	passenger *p = (passenger *) malloc(sizeof(passenger)); // create pointer to passenger 
	p = createPassengers(numOfPas); // created struct of passengers

	int passNum = p[0].number;	// test to we can get number element from passenger	
	printf("%d\n", passNum);

	// thread creation for bag handler
	int numBReturned[numOfBag]; // number returned for bag handler threads
	pthread_t bThread[numOfBag]; // create an array of bag handler threads
    for (int i = 0; i < numOfBag; i++) {
         numBReturned[i] = pthread_create(&bThread[i], NULL, checkBag, &p[i]);
         if (numBReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	for (int i = 0; i < numOfBag; i++) {
		pthread_join(bthread[i], NULL);
	}

	bool resultBag = p[1].baggageChecked;
	
	printf("%s\n", resultBag ? "true" : "false");

	

	/*
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
*/
	return 0;
}
