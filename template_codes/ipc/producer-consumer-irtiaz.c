#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_SIZE 3

#define PRODUCER_DELAY 1
#define CONSUMER_DELAY 5

struct thread_argument {
	sem_t empty;
	sem_t full;
	pthread_mutex_t mutex;
	int *items;
	int *p_length;
};

void *producer_thread_function(void *argument);
void *consumer_thread_function(void *argument);


int main(void) {
	int items[TOTAL_SIZE];
	int length = 0;

	pthread_t producer_thread;
	pthread_t consumer_thread;
	struct thread_argument thread_argument;

	sem_init(&thread_argument.empty, 0, TOTAL_SIZE);
	sem_init(&thread_argument.full, 0, 0);
	pthread_mutex_init(&thread_argument.mutex, NULL);

	thread_argument.items = items;
	thread_argument.p_length = &length;

	srand(time(NULL));

	pthread_create(&producer_thread, NULL, producer_thread_function, (void *)&thread_argument);
	pthread_create(&consumer_thread, NULL, consumer_thread_function, (void *)&thread_argument);

	pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);

	return 0;
}

void *producer_thread_function(void *argument) {
	struct thread_argument *thread_argument = (struct thread_argument *)argument;
	int item;

	while (1) {
		item = rand() % 100;

		sem_wait(&thread_argument->empty);

		pthread_mutex_lock(&thread_argument->mutex);
		thread_argument->items[(*thread_argument->p_length)++] = item;
		printf("Produced %d\n", item);
		pthread_mutex_unlock(&thread_argument->mutex);

		sem_post(&thread_argument->full);

		sleep(PRODUCER_DELAY);
	}

	return NULL;
}

void *consumer_thread_function(void *argument) {
	struct thread_argument *thread_argument = (struct thread_argument *)argument;
	int item;

	while (1) {
		sem_wait(&thread_argument->full);

		pthread_mutex_lock(&thread_argument->mutex);
		item = thread_argument->items[--*(thread_argument->p_length)];
		printf("Consumed %d\n", item);
		pthread_mutex_unlock(&thread_argument->mutex);

		sem_post(&thread_argument->empty);

		sleep(CONSUMER_DELAY);
	}

	return NULL;
}
