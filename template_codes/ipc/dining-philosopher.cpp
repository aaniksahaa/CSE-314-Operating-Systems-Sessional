#include<stdio.h>
#include<iostream>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<stack>
#include <unistd.h>
#include<ctime>

using namespace std;

#define N 5
#define LEFT(i) (i-1+N)%N
#define RIGHT(i) (i+1)%N

enum State{
    THINKING,
    HUNGRY,
    EATING
};

State state[N];
sem_t ok[N];
pthread_mutex_t lock;

void init(){
    pthread_mutex_init(&lock,0);
    for(int i=0; i<N; i++){
        state[i] = THINKING;
        sem_init(&ok[i], NULL, 0);
    }
}

void think(int i){
    state[i] = THINKING;
    sleep(2);
}

void test(int i){
    if(state[i]==HUNGRY && state[LEFT(i)]!=EATING && state[RIGHT(i)]!=EATING){
        state[i] = EATING;
        sem_post(&ok[i]);
    }
}

void take_forks(int i){
    pthread_mutex_lock(&lock);
    state[i] = HUNGRY;
    test(i);
    pthread_mutex_unlock(&lock);
    sem_wait(&ok[i]);
}

void eat(int i){
    printf("Philosopher %d is eating...\n", i);
    sleep(3);
    printf("Philosopher %d finished eating...\n", i);
}

void put_forks(int i){
    pthread_mutex_lock(&lock);

    state[i] = THINKING;
    test(LEFT(i));
    test(RIGHT(i));

    pthread_mutex_unlock(&lock);
}

void* philosopher_func(void* arg){
    int i = (int)arg;
    printf("i am philosopher %d\n", i);
    for(int t=1; t<=100; t++){
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

int main(void){
    pthread_t philosophers[N];

    init();

    for(int i=0; i<N; i++){
        pthread_create(&philosophers[i], NULL, philosopher_func, (void*)i);
    }

    while(1);

    return 0;
}



