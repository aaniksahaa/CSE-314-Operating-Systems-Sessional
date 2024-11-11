#include<pthread.h>
#include<stdio.h>
#define NUM_THREADS 5

void* printHello(void* threadId){
    int tid = (int)threadId;
    printf("Hello! I am thread %d\n", tid);
    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    pthread_t threads[NUM_THREADS];
    int rc;
    for(int t=0; t<NUM_THREADS; t++){
        printf("in main: creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, printHello, (void*)t);
        if(rc){
            printf("ERROR in thread create, ret code = %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
}
