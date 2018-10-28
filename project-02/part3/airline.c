#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "passenger.h"
int	numOfPas;
int	numOfBag;
int	numOfSec;
int	numOfAtt;	

passenger *pp;
 
sem_t p2b,b2p,p2s,s2p,p2a,a2p;
sem_t finmut,secmut,attmut;
pthread_mutex_t pasmut;
pthread_attr_t attr;

void init()
{
    sem_init(&p2b, 0, 0);//Semaphore for passenger to baggage
    sem_init(&b2p,0,numOfBag);//Semaphore for baggage to passenger
    sem_init(&p2s,0,0);//Semaphore for passenger to security
    sem_init(&s2p,0,numOfSec);//Semaphore for security to passenger
    sem_init(&p2a,0,0); //Semaphore for passenger to attendant
    sem_init(&a2p,0,numOfAtt);//Semaphore for attendant to passenger
    sem_init(&finmut,0,0); //Binary Semaphore to say the attendant is done
    sem_init(&secmut,0,0);//Binary Semaphore to say the security can be called
    sem_init(&attmut,0,0);//Binary semaphore to inform attendant can be called
    pthread_mutex_init(&pasmut,NULL);//Mutex to check when passenger number is added to the array
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

}

void *passenger_exp(passenger *p)
{
	static int a=0;
	static int b=0;
	static int c=0;

	//Adds the passenger to the array
	pthread_mutex_lock(&pasmut);
	//in++;
	pthread_mutex_unlock(&pasmut);

	//Waits for baggage and calls the baggage thread
	sem_wait(&b2p);
	printf("\nPassenger #%d arrived and waiting for baggage\n", pp->number);
	usleep(1000);
	sem_post(&p2b);

	sem_wait(&secmut);

	//Waits for security and calls the security thread
	sem_wait(&s2p);
	printf("\nPassenger #%d done with baggage and waiting for security\n", pp->number);
	usleep(1000);
	sem_post(&p2s);

	sem_wait(&attmut);
	//Waits for attendant and calls the attendant thread
	sem_wait(&a2p);
	printf("\nPassenger #%d done with security and waiting for attendant\n", pp->number);
	usleep(1000);
	sem_post(&p2a);
	sem_wait(&finmut);
	pthread_exit(NULL);

}

void *baggage(passenger *pp)
{
    while(1)
    {
        sem_wait(&p2b);//Waits for passenger to post
        //printf("\nPassenger is handled by baggage :\t%d",bagval);
        pthread_mutex_lock(&pasmut);
        printf("\nPassenger #%d by baggage handler\n",pp->number);
        pthread_mutex_unlock(&pasmut);
        usleep(1000);
        sem_post(&secmut);// Wakes up secmut to call security thread
        sem_post(&b2p);//Posts to accept next passenger
    }

}

 void *security(passenger *pp)
 {
     static int cs=0;
     while(1)
     {
         sem_wait(&p2s);//Waits for passenger to post
         //printf("\nPassenger is handled by security :\t%d",secval);
         pthread_mutex_lock(&pasmut);
         printf("\nPassenger #%d by security screen\n",pp->number);
         pthread_mutex_unlock(&pasmut);
         usleep(1000);
         sem_post(&attmut);// Wakes up attmut to call attendant thread
         sem_post(&s2p);//Post when it is free to process next passenger
     }
 }

 void *attendant(passenger *pp)
 {
    while(1)
     {
         sem_wait(&p2a);
         //printf("\nPassenger is handled by attendant :\t%d",attval);
         pthread_mutex_lock(&pasmut);
         printf("\nPassenger #%d by flight attendant\n",pp->number);
         pthread_mutex_unlock(&pasmut);
         usleep(1000);
         sem_post(&finmut);
         sem_post(&a2p);
     }
 }

int main(int argc, char **argv) {

	if (argc != 5) {
		printf("\nPlease use 5 arguments!");
		printf("Format: ./airline <# of Passengers>  <#of Baggage> <# of Security> <# of Flight Attendant>\n");
		return -1;
	}

	// parameters passed at exectution.
	numOfPas = atoi(argv[1]);
	numOfBag = atoi(argv[2]);
	numOfSec = atoi(argv[3]);
	numOfAtt = atoi(argv[4]);
	
	printf("\n Number of Passengers: %d\n", numOfPas);
	printf("\n Number of Baggage Handlers: %d\n", numOfBag);
	printf("\n Number of Security Screeners: %d\n", numOfSec);
	printf("\n Number of FLight Attendants: %d\n\n", numOfAtt);

	pp = createPassengers(numOfPas); // created struct of passengers

	printf("%d\n", numOfPas);


	// thread creation for bag handler
	int numBReturned[numOfBag]; // number returned for bag handler threads
	pthread_t *bThread =  (pthread_t *)malloc(sizeof(pthread_t) * numOfBag);; // create an array of bag handler threads
    for (int i = 0; i < numOfBag; i++) {
         numBReturned[i] = pthread_create(&bThread[i], NULL, baggage, &pp[i]);
         if (numBReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for secuirty screener
	int numSReturned[numOfSec]; // number returned for security screener threads
	pthread_t sThread[numOfSec]; // create an array of secuirty screener threads
    for (int i = 0; i < numOfSec; i++) {
         numSReturned[i] = pthread_create(&sThread[i], NULL, security, &pp[i]);
         if (numSReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for flight attendents
	int numAReturned[numOfAtt]; // number returned for security screener threads
	pthread_t aThread[numOfAtt]; // create an array of secuirty screener threads
    for (int i = 0; i < numOfAtt; i++) {
         numAReturned[i] = pthread_create(&aThread[i], NULL, attendant, &pp[i]);
         if (numAReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	// thread creation for passengers
	int numPReturned[numOfPas]; // number returned for passenger  threads
	pthread_t pThread[numOfPas]; // create an array of passenger threads
    for (int i = 0; i < numOfPas; i++) {
         numPReturned[i] = pthread_create(&pThread[i], NULL, passenger_exp, &pp[i]);
         if (numPReturned[i] != 0) {
            printf("Uh-oh!\n");
            return -1;
         }
    }

	for (int i = 0; i < numOfBag; i++) {
		pthread_join(pThread[i], NULL);

	}

	printf("\nAll the passengers have been seated\n");
			sem_destroy(&p2b);
			sem_destroy(&p2s);
			sem_destroy(&p2a);
			sem_destroy(&b2p);
			sem_destroy(&s2p);
			sem_destroy(&a2p);
			sem_destroy(&finmut);
			sem_destroy(&secmut);
			sem_destroy(&attmut);
			pthread_mutex_destroy(&pasmut);
/*
	bool resultBag = p[0].baggageChecked;
	printf("%s\n", resultBag ? "true" : "false");
	bool resultScreen = pp[0].screened; 
	printf("%s\n", resultScreen ? "true" : "false");
	bool resultSeated = pp[3].seated;
	printf("%s\n", resultSeated ? "true" : "false");
	int count = 0;
	for (int i = 0; i < numOfPas; i++) {
		count++;
		if (isWaiting(&pp[i]) == false) {
			count++;
		}
	}
	if (count == numOfPas) {
		printf("All passengers have been seated.\n");
	}
*/
	return 0;
}
