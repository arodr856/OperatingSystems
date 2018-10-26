#include "passenger.h"
#include <stdlib.h>


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

void checkBaggage(passenger *p) {
	p->baggageChecked = true;
}

void screen(passenger *p) {
	p->screened = true;
}

void seat(passenger *p) {
	p->seated = true;
}

void relaxed(passenger *p) {
	if (p->baggageChecked == true && p->screened == true && p->seated == true) 
		p->waiting = false;
}
