#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFSIZE 10 // assuming buffer size of 10
void *producer(void *id);
void *consumer(void *id);


int buffer[BUFSIZE];
int counter = 0; // counter to store number of elements in buffer

// number of times producer and consumer will produce and consume
// respectively
int i = 100; 
int j = 100;


sem_t MUTEX;
sem_t FULL;
sem_t EMPTY;

int	main	(int	argc,	char	*argv[])	{

	// initializing the mutex and semaphores
	sem_init(&MUTEX,0,1);
   	sem_init(&FULL,0,0);
	sem_init(&EMPTY,0,10);

	pthread_t prod, cons;	
	// creating threads for procuder and consumer
	pthread_create(&prod, NULL, producer, NULL);
	pthread_create(&cons, NULL, consumer, NULL);
	
	pthread_exit(NULL);
	return	0;
}

/* Add an item to the buffer */
int insert_item(int item) {
   /* When the buffer is not full add the item
      and increment the counter*/
   if(counter < BUFSIZE) {
      buffer[counter] = item;
      counter++;
      return 0;
   }
   else { /* Error the buffer is full */
      return -1;
   }
}

/* Remove an item from the buffer */
int remove_item(int *item) {
   /* When the buffer is not empty remove the item
      and decrement the counter */
   if(counter > 0) {
      *item = buffer[(counter-1)];
      counter--;
      return 0;
   }
   else { /* Error buffer empty */
      return -1;
   }
}

void *producer(void *param) {

	while(i--) {

		// produer is producing the item here, the item is equal to the 
		// index of the loop. After producing it sleeps for some random duration
		// to simulate production time
		int item = i;
		//float r = ((double) rand() / (RAND_MAX));
		//usleep(r*1000000);

		int val;
		sem_getvalue(&EMPTY,&val);
		// producer will wait often if the consumer is slow, in ideal scenario
		// it will mostly not wait and will keep on producing
		if(val == 0){
			printf("Buffer is full, producer waiting..\n");
		}
		else{
			printf("Producer will produce...\n");
		}

		sem_wait(&EMPTY);
		sem_wait(&MUTEX);

		/* critical section begins*/

		// put the item into the buffer
		if(insert_item(item)) {
         fprintf(stderr, " Producer report error condition\n");
      	}
      	else {
         printf("Producer produced %d\n", item);
      	}

		/* critical section ends*/


		sem_post(&MUTEX);
		sem_post(&FULL);

	}
	pthread_exit(NULL);
}

void *consumer(void *param) {

	while(j--)
	{

		int val;
		sem_getvalue(&FULL,&val);
		// cosumer will wait often when producer is slow, in ideal scenario
		// it would keep consuming and producer will keep producing
		if(val == 0){
			printf("Buffer is empty, consumer waiting..\n");
		}
		else{
			printf("Consumer will consume...\n");
		}

		sem_wait(&FULL);
		sem_wait(&MUTEX);
		/* critical section */

		int item;

		if(remove_item(&item)) {
			fprintf(stderr, "Consumer report error condition\n");
	     }
		else {
			printf("Consumer consumed %d\n", item);
		}
		/*critical section ends*/
		sem_post(&MUTEX);
		sem_post(&EMPTY);
		

	}	
	
	pthread_exit(NULL);
}
