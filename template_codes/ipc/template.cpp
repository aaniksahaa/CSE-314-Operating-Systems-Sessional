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

// globals

// semaphores

pthread_mutex_t print_lock;
// sem_t sem;
// pthread_mutex_t lock;

// helpers

void safe_print(string s){
    pthread_mutex_lock(&print_lock);
    cout<<s<<endl;
    pthread_mutex_unlock(&print_lock);
}

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
    init_semaphore();

    // pthread_t T;

    // pthread_create(&T,NULL,func,(void*)i );

    // while(1);  // to let the other threads finish
    pthread_exit(NULL);

    return 0;
}
