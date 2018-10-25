#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "passenger.h"

int main(int argc, char *argv[]) {
	int numberOfPassengers = *argv[1] - '0';
//	int numberOfBaggageHandlers = *argv[2] - '0';
//	int numberOfSecurityScreeeners = *argv[3] - '0';
//	int numberOfFlightAttendents = *argv[4] - '0';
	passenger *p = (passenger *) malloc(sizeof(passenger) * 10);
	p = createPassengers(numberOfPassengers);
	int passengerNumber = p[0].number;		
	printf("Passenger #%d\n", passengerNumber);

	int numberReturned[numberOfPassengers];
	pthread_t passengerThread[numberOfPassengers];

//  int numberReturned[numberOfBaggageHandlers];
//  pthread_t baggageHandlerThread[numberOfBaggageHandlers];
  
// int numberReturned[numberOfSecurityScreeners];
//  pthread_t securityScreenerThread[numberOfSecurityScreeners];

//int numberReturned[numberOfFlightAttendents];
//  pthread_t flightAttendentThread[numberOfFlightAttendents];

	return 0;
}
