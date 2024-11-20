#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define PHILOSOPHER_COUNT 4
#define THINK_TIME 2
#define EAT_TIME 3

#define LEFT(i) ((i + PHILOSOPHER_COUNT - 1) % PHILOSOPHER_COUNT)
#define RIGHT(i) ((i + 1) % PHILOSOPHER_COUNT)

enum state {
	THINKING,
	HUNGRY,
	EATING
};

struct thread_argument {
	int index;
	enum state *states;
	pthread_mutex_t *locks;
	pthread_mutex_t *mutex;
};

void *philosopher(void *argument);
void test(int i, enum state *states, pthread_mutex_t *locks);
void take_forks(int i, pthread_mutex_t *mutex, enum state *states, pthread_mutex_t *locks);
void put_forks(int i, pthread_mutex_t *mutex, enum state *states, pthread_mutex_t *locks);

int main(void) {

	pthread_t philosopher_threads[PHILOSOPHER_COUNT];
	struct thread_argument thread_arguments[PHILOSOPHER_COUNT];
	enum state states[PHILOSOPHER_COUNT];
	pthread_mutex_t locks[PHILOSOPHER_COUNT];
	pthread_mutex_t mutex;

	int i;

	for (i = 0; i < PHILOSOPHER_COUNT; ++i) {
		pthread_mutex_init(locks + i, NULL);
		pthread_mutex_lock(locks + i);
	}

	pthread_mutex_init(&mutex, NULL);

	for (i = 0; i < PHILOSOPHER_COUNT; ++i) {
		thread_arguments[i].index = i;
		thread_arguments[i].states = states;
		thread_arguments[i].locks = locks;
		thread_arguments[i].mutex = &mutex;
		thread_arguments[i].locks = locks;

		pthread_create(philosopher_threads + i, NULL, philosopher, (void *)(thread_arguments + i));
	}

	for (i = 0; i < PHILOSOPHER_COUNT; ++i) {
		pthread_join(philosopher_threads[i], NULL);
	}

	return 0;
}

void *philosopher(void *argument) {
	struct thread_argument *thread_argument = (struct thread_argument *)argument;
	int i = thread_argument->index;

	while (1) {
		printf("%d is thinking...\n", i);
		thread_argument->states[i] = THINKING;
		sleep(THINK_TIME);

		take_forks(i, thread_argument->mutex, thread_argument->states, thread_argument->locks);
		printf("%d is eating\n", i);
		sleep(EAT_TIME);
		put_forks(i, thread_argument->mutex, thread_argument->states, thread_argument->locks);
	}

	return NULL;
}

void test(int i, enum state *states, pthread_mutex_t *locks) {
	if (states[i] == HUNGRY && states[LEFT(i)] != EATING && states[RIGHT(i)] != EATING) {
		states[i] = EATING;
		pthread_mutex_unlock(locks + i);
	}
}

void take_forks(int i, pthread_mutex_t *mutex, enum state *states, pthread_mutex_t *locks) {
	pthread_mutex_lock(mutex);

	states[i] = HUNGRY;
	test(i, states, locks);

	pthread_mutex_unlock(mutex);
	pthread_mutex_lock(locks + i);
}

void put_forks(int i, pthread_mutex_t *mutex, enum state *states, pthread_mutex_t *locks) {
	pthread_mutex_lock(mutex);

	states[i] = THINKING;
	test(LEFT(i), states, locks);
	test(RIGHT(i), states, locks);

	pthread_mutex_unlock(mutex);
}
