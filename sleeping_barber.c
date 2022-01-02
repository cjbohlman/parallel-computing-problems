// classic inter-proces communication and synchronization problem

#define CUSTOMERS 20

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For sleep
#include<signal.h> // For signal handling
#include <stdlib.h> // exit

sem_t customerReady;// 0: mutex
sem_t barberReady; // 0: mutex
sem_t accessWRSeats; // 1: if 1, the amount of seats in the waiting room can be incremented or decremented

unsigned int availableWRSeats;

pthread_t barb;
pthread_t cust[CUSTOMERS];

void barber(void) {
	while (1) {
		// Try to acquire a customer. If none are available, go to sleep
		sem_wait(&customerReady);
		printf("Barber found a ready customer\n");
		// Try to modify the number of available waiting room seats.
		// If not available, go to sleep
		sem_wait(&accessWRSeats);
		availableWRSeats += 1;
		printf("Barber decremented waiting room seats\n");
		// Signal ready to cut
		sem_post(&barberReady);
		sem_post(&accessWRSeats);

		// Cut hair here
		printf("Barber is cutting hair\n");
		sleep(2);
		printf("Barber has finished haircut\n");
	}
}

void customer(void) {
		sem_wait(&accessWRSeats);
		if (availableWRSeats > 0) {
			printf("Customer found waiting room seat\n");
			availableWRSeats -= 1;
			// notify the barber that a customer is ready
			sem_post(&customerReady);
			sem_post(&accessWRSeats);
			sem_wait(&barberReady);
			printf("Customer is leaving\n");
		} else {
			// Leave without a haircut
			printf("Customer left without being seated\n");
			sem_post(&accessWRSeats);
		}
}

void sig_handler(int signum){

	printf("\nInside handler function\n");

	pthread_cancel(barb);

	sem_destroy(&customerReady);
	sem_destroy(&barberReady);
	sem_destroy(&accessWRSeats);
	exit(0);
}

int main(void) {
	signal(SIGINT,sig_handler); // Register signal handler

	availableWRSeats = 3;
	sem_init(&customerReady, 0, 0);
	sem_init(&barberReady, 0, 0);
	sem_init(&accessWRSeats, 0, 1);

	pthread_create(&barb, NULL, (void*) barber, NULL);
	for (int i = 0; i < CUSTOMERS; i++) {
		pthread_create(&cust[i], NULL, (void *) customer, NULL);
		sleep(1);
	}
	
	pthread_join(barb, NULL);

	sem_destroy(&customerReady);
	sem_destroy(&barberReady);
	sem_destroy(&accessWRSeats);

	return 0;
}
