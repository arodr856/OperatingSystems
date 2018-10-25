#ifndef PASSENGER 
#define PASSENGER
#include <stdio.h>
#include <stdbool.h>

typedef struct airplane_passenger {
	int number;
	bool waiting; 
	bool baggageChecked;
	bool screened;
	bool seated;
} passenger;

passenger *createPassengers( int numberOfPassengers);
bool isWaiting(passenger *p);
void checkBaggage(passenger *p);
void screen(passenger *p);
void seat(passenger *p);
void relaxed(passenger *p);
#endif
