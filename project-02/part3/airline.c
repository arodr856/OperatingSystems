#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "passenger.h"

int main(int argc, char *argv[]) {
	int numOfPass = *argv[1] - '0';
//	int numOfBag = *argv[2] - '0';
//	int numOfSec = *argv[3] - '0';
//	int numOfAtten = *argv[4] - '0';
	passenger *p = (passenger *) malloc(sizeof(passenger) * 10);
	p = createPassengers(numOfPass);
	int passNum = p[0].number;		
	p[0].number =699;
	printf("Passenger #%d\n", passNum);
	passNum = p[0].number;
	printf("Passenger #%d\n", passNum);


	int numPReturned[numOfPass];
	pthread_t pThread[numOfPass];

//  int numBReturned[numOfBag];
//  pthread_t bThread[numOfBag];
  
// int numSReturned[numOfSec];
//  pthread_t sThread[numOfSec];

//int numAReturned[numOfAtten];
//  pthread_t aThread[numOfAtten];

	return 0;
}
