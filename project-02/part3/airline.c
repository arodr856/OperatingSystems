#include <stdio.h>
#include "passenger.h"

int main(int argc, char *argv[]) {

	passenger p;
	p.waiting = true;
	bool waitStatus = isWaiting(&p);
	printf("%s\n", waitStatus ? "true" : "false");
	

	return 0;
}
