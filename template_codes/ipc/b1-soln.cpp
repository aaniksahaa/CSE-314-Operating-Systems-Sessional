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

sem_t a,b;

void init_semaphore(){
    sem_init(&a,0,0);
    sem_init(&b,0,0);
    // pthread_mutex_init(&lock,0);
}

void* pfunc(void* arg){
    int n = (int)arg;
    for(int i=1; i<=n/2; i++){
        for(int j=1; j<=n-i; j++){
            cout<<"_";
        }
        sem_post(&a);
        sem_wait(&b);
    }
}

void* mfunc(void* arg){
    int n = (int)arg;
    for(int i=1; i<=n/2; i++){
        sem_wait(&a);
        for(int j=1; j<=i; j++){
            cout<<"+";
        }
        cout<<endl;
        sem_post(&b);
    }
}

int main(){
    init_semaphore();

    int n = 10;

    pthread_t P,M;

    pthread_create(&P,NULL,pfunc,(void*)n);
    pthread_create(&M,NULL,mfunc,(void*)n);

    // while(1);  // to let the other threads finish
    pthread_exit(NULL);

    cout<<"hello\n";
    return 0;
}
