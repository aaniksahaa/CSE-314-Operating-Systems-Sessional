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

//globals

int rc = 0;

default_random_engine rand_generator;
uniform_int_distribution<int> rdelay(0,5);
uniform_int_distribution<int> wdelay(0,3);

// semaphores

// sem_t sem;
pthread_mutex_t print_lock;
pthread_mutex_t db, rc_lock;

// helpers

void safe_print(string s){
    pthread_mutex_lock(&print_lock);
    cout<<s<<endl;
    pthread_mutex_unlock(&print_lock);
}

void init_semaphore(){
    // sem_init(&sem,0,MAX);
    pthread_mutex_init(&db,0);
    pthread_mutex_init(&rc_lock,0);
}

void* reader_func(void* arg){
    sleep(rdelay(rand_generator));

    int i = (int)arg;

    safe_print("Reader "+to_string(i)+" is waiting\n");

    pthread_mutex_lock(&rc_lock);
    rc += 1;
    if(rc == 1){
        pthread_mutex_lock(&db);
    }
    pthread_mutex_unlock(&rc_lock);

    // reading
    safe_print("Reader "+to_string(i)+" is reading\n");
    sleep(1);

    pthread_mutex_lock(&rc_lock);
    rc -= 1;
    if(rc == 0){
        pthread_mutex_unlock(&db);
    }
    pthread_mutex_unlock(&rc_lock);

    safe_print("Reader "+to_string(i)+" exited\n");
}

void* writer_func(void* arg){
    sleep(wdelay(rand_generator));

    int i = (int)arg;

    safe_print("Writer "+to_string(i)+" is waiting\n");

    pthread_mutex_lock(&db);
    // writing
    safe_print("Writer "+to_string(i)+" is writing\n");
    sleep(1);
    pthread_mutex_unlock(&db);

    safe_print("Writer "+to_string(i)+" exited\n");

    // pthread_mutex_lock(&lock);
    // pthread_mutex_unlock(&lock);

    // sem_wait(&sem);
    // sem_post(&sem);
}

int main(){
    init_semaphore();

    int n = 10, m = 5;

    pthread_t readers[10], writers[5];

    for(int i=0; i<n; i++){
        pthread_create(&readers[i],NULL,reader_func,(void*)(i+1) );
    }

    for(int i=0; i<m; i++){
        pthread_create(&writers[i],NULL,writer_func,(void*)(i+1) );
    }

    // pthread_create(&T,NULL,func,(void*)i );

    // while(1);  // to let the other threads finish
    pthread_exit(NULL);

    return 0;
}
