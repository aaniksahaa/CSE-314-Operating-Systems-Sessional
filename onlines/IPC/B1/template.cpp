#include <iostream>
#include <chrono>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<ctime>
#include <unistd.h>
#include <vector>
#include <random>
#include <regex>

using namespace std;

// semaphores

// sem_t sem;
// pthread_mutex_t lock;

void init_semaphore(){
    // sem_init(&sem,0,MAX);
    // pthread_mutex_init(&lock,0);
}

void* func(void* index){
    // pthread_mutex_lock(&lock);
    // pthread_mutex_unlock(&lock);

    // sem_wait(&sem);
    // sem_post(&sem);
}

int main(){
    // init_semaphore();
    // pthread_t T;

    // pthread_create(&T,NULL,func,(void*)i );

    // while(1);  // to let the other threads finish
    // pthread_exit(NULL);

    cout<<"hello\n";
    return 0;
}
