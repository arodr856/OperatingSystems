#include "passenger.h"
#include <stdlib.h>
#include <semaphore.h>

passenger *createPassengers(int numberOfPassengers) {
	passenger *passengers = (passenger *) malloc(sizeof(passenger) * numberOfPassengers);
	for (int i = 0; i < numberOfPassengers; i++) {
		passenger p;
		p.number = i;
		p.waiting = true;
		p.baggageChecked = false;
		p.screened = false;
		p.seated = false;
		passengers[i] =  p;
		}	
	return passengers;
}

bool isWaiting(passenger *p) {
	if (p->waiting)
		return true;
	else 
		return false;
}

void checkBag(passenger *p) {
	printf("Passenger #%d is waiting at baggage processing for a handler.\n", p->number);
	p->baggageChecked = true;
}

void screen(passenger *p) {
	p->screened = true;
}

void seat(passenger *p) {
	printf("Passenger #%d has been seated and relaxes.\n", p->number);
	p->seated = true;
}

void relaxed(passenger *p) {
	if (p->baggageChecked == true && p->screened == true && p->seated == true) 
		p->waiting = false;
}
