#include <stdio.h>
#include <stdlib.h>
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

	return 0;
}
