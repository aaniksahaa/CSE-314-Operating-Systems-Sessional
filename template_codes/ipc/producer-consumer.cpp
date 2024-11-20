#include<stdio.h>
#include<iostream>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<stack>
#include <unistd.h>
#include<ctime>

using namespace std;

#define BUFFER_SIZE 5

sem_t empty;
sem_t full;
pthread_mutex_t lock;

// bounded buffer
queue<int>q;

void init_semaphore(){
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&lock, 0);
}

void* producer_func(void* arg){
    printf("%s\n", (char*)arg);
    int i;
    for(i=1; i<=100; i++){
        sem_wait(&empty);

        pthread_mutex_lock(&lock);
        q.push(i);
        printf("producer produced item %d\n", i);
        pthread_mutex_unlock(&lock);

        sem_post(&full);
        sleep(1);
    }
}

void* consumer_func(void* arg){
    printf("%s\n", (char*)arg);
    int i;
    for(i=1; i<=100; i++){
        sem_wait(&full);

        pthread_mutex_lock(&lock);
        int item = q.front();
        q.pop();
        printf("consumer consumed item %d\n", item);
        pthread_mutex_unlock(&lock);

        sem_post(&empty);
        sleep(3);
    }
}

int main(void){
    pthread_t P;
    pthread_t C;

    init_semaphore();

    char* msg1 = "I am producer";
    char* msg2 = "I am consumer";

    pthread_create(&C, NULL, consumer_func, (void*)msg2);
    pthread_create(&P, NULL, producer_func, (void*)msg1);

    while(1);

    return 0;
}



