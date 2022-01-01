// readers writers problem
// No reader shall wait if the share is currently opened for reading

#define WRITERS 5
#define READERS 10

// for this driver program to work, readers >= writers

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For sleep

sem_t resource;// 1: mutex
sem_t readerMutex; // 1: mutex
int readCount = 0;

void writer(void) {
	sem_wait(&resource);

	printf("Writer entering critical section\n");
	sleep(3);
	printf("Writer exiting critical section\n");

	sem_post(&resource);
}

void reader(void) {
	
	// Enter reader critical section
	sem_wait(&readerMutex);
	readCount++;
	if (readCount == 1) {
		// if first reader, lock the resource from writers
		sem_wait(&resource);
	}
	// Exit reader critical section
	sem_post(&readerMutex);

	printf("Reader entering critical section\n");
	// Do the reading
	sleep(2);
	printf("Reader exiting critical section\n");

	// Enter reader critical section
	sem_wait(&readerMutex);
	readCount--;
	if (readCount == 0) {
		// if last reader active, unlock the resource for writers
		sem_post(&resource);
	}
	// Exit reader critical section
	sem_post(&readerMutex);
}

int main(void) {
	sem_init(&resource, 0, 1);
	sem_init(&readerMutex, 0, 1);

	pthread_t writers[WRITERS];
	pthread_t readers[READERS];

	for (int i = 0; i < WRITERS; i++) {
		pthread_create(&writers[i], NULL, (void *) writer, NULL);
		pthread_create(&readers[i], NULL, (void *) reader, NULL);
	}
	for (int i = READERS - WRITERS; i < READERS; i++) {
		pthread_create(&readers[i], NULL, (void *) reader, NULL);
	}

	for (int i = 0; i < WRITERS; i++) {
		pthread_join(writers[i], NULL);
	}
	for (int i = 0; i < READERS; i++) {
		pthread_join(readers[i], NULL);
	}

	sem_destroy(&resource);
	sem_destroy(&readerMutex);
	return 0;
}