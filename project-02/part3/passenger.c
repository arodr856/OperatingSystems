#include "passenger.h"


passenger *numberOfPassengers(passenger *p, int numberOfPassengers) {
	for (int i = 0; i < numberOfPassengers; i++) {
		passenger passengers;
		p[i] = passengers;
	}
	return p;
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
