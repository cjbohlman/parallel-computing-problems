// producer consumers problem
// any amount of producers and any amount of consumers

// for this driver program to work, PRODUCERS == CONSUMERS
#define PRODUCERS 5
#define CONSUMERS 5
#define BUFFER_SIZE 3

int buffer[BUFFER_SIZE];

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

sem_t emptyCount;
sem_t fillCount;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;


int produceItem() {
	return 1;
}

void consumeItem(int item) {
	printf("Consuming item\n");
}

void putItemIntoBuffer(int item) {
	printf("Putting item into buffer\n");
	int sem_val;
	if (sem_getvalue(&fillCount, &sem_val) == 0){
        buffer[sem_val] = item;
    }   
}

int removeItemFromBuffer() {
	printf("Removing item from buffer\n");
	int sem_val;
	if (sem_getvalue(&fillCount, &sem_val) == 0){
        return buffer[sem_val];
    } else {
    	return -1;
    }
}

void producer(void) {
	int item = produceItem();
	sem_wait(&emptyCount);
	pthread_mutex_lock(&buffer_mutex);
	putItemIntoBuffer(item);
	pthread_mutex_unlock(&buffer_mutex);
	sem_post(&fillCount);
}

void consumer(void) {
	sem_wait(&fillCount);
	pthread_mutex_lock(&buffer_mutex);
	int item = removeItemFromBuffer();
	pthread_mutex_unlock(&buffer_mutex);
	sem_post(&emptyCount);
	consumeItem(item);
}

int main(void) {
	sem_init(&emptyCount, 0, BUFFER_SIZE);
	sem_init(&fillCount, 0, 0);

	pthread_t producers[PRODUCERS];
	pthread_t consumers[CONSUMERS];


	pthread_create(&producers[0], NULL, (void *) producer, NULL);
	pthread_create(&consumers[0], NULL, (void *) consumer, NULL);
	pthread_create(&consumers[1], NULL, (void *) consumer, NULL);
	pthread_create(&producers[1], NULL, (void *) producer, NULL);
	pthread_create(&consumers[2], NULL, (void *) consumer, NULL);
	pthread_create(&consumers[3], NULL, (void *) consumer, NULL);
	pthread_create(&producers[2], NULL, (void *) producer, NULL);
	pthread_create(&producers[3], NULL, (void *) producer, NULL);
	pthread_create(&producers[4], NULL, (void *) producer, NULL);
	pthread_create(&consumers[4], NULL, (void *) consumer, NULL);

	for (int i = 0; i < PRODUCERS; i++) {
		pthread_join(producers[i], NULL);
		pthread_join(consumers[i], NULL);
	}

	sem_destroy(&emptyCount);
	sem_destroy(&fillCount);
	return 0;
}