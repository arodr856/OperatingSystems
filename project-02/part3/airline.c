#include <stdio.h>
#include <stdlib.h>
#include "passenger.h"

int main(int argc, char *argv[]) {

	passenger *p = (passenger *) malloc(sizeof(passenger) * 10);
	p = numberOfPassengers(10);
	int passengerNumber = p[0].number;		
	printf("Passenger #%d\n", passengerNumber);

	return 0;
}
