#include <iostream>
#include <chrono>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<ctime>
#include <unistd.h>
#include <vector>
#include <random>

using namespace std;

#define int long long int
#define NUM_STAIR_STEPS 3
#define GALLERY_1_MAX 5
#define INTER_ARRIVAL_RATE 2

int starting_time_s = 0;

/******** Helpers ********/

int get_current_timestamp_s(){
    return time(nullptr) - starting_time_s;
}

int get_current_timestamp_ms(){
    auto now = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
    int timestamp_ms = duration.count();
    return timestamp_ms;
}

default_random_engine rand_generator;
poisson_distribution<int> arrival_delay_poisson_dist(INTER_ARRIVAL_RATE);

/******** Helpers End ********/

class Visitor {
public:
    int id;
    bool is_premium;
    int arrival_delay;

    Visitor(int id, bool is_premium, int arrival_delay)
        : id(id), is_premium(is_premium), arrival_delay(arrival_delay) {
        //
    }

    void display() {
        std::cout << "ID: " << id << ", Status: " << (is_premium ? "Premium" : "Standard")
                  << ", Arrival Delay: " << arrival_delay << " seconds" << std::endl;
    }
};

/******** Globals ********/

vector<Visitor>visitors;
int N,M;
int w,x,y,z;

// semaphores

pthread_mutex_t stair_mutexes[NUM_STAIR_STEPS];
sem_t gallery_1_remaining_space;

/******** Globals End ********/

void init_semaphore()
{
    for(int i=0; i<NUM_STAIR_STEPS; i++){
        pthread_mutex_init(&stair_mutexes[i],0);
    }
    sem_init(&gallery_1_remaining_space,0,GALLERY_1_MAX);
}

void* visitor_func(void* visitor_index){
    Visitor visitor = visitors[(int)visitor_index];
    sleep(visitor.arrival_delay);
    printf("Visitor %d has arrived at A at timestamp %d\n", visitor.id, get_current_timestamp_s());
    sleep(w);
    printf("Visitor %d has arrived at B at timestamp %d\n", visitor.id, get_current_timestamp_s());
    for(int i=0; i<NUM_STAIR_STEPS; i++){
        pthread_mutex_lock(&stair_mutexes[i]);
        printf("Visitor %d is at step %d at timestamp %d\n", visitor.id, i+1, get_current_timestamp_s());
        sleep(1);
        pthread_mutex_unlock(&stair_mutexes[i]);
    }
}

int32_t main() {
    starting_time_s = time(nullptr);

    // cin>>N>>M;
    // cin>>w>>x>>y>>z;

    init_semaphore();

    N = 10;
    w = 2;

    for(int i=0; i<N; i++){
        visitors.push_back(Visitor(1001+i, false, arrival_delay_poisson_dist(rand_generator)));
    }

    pthread_t visitor_threads[N];

    for(int i=0; i<N; i++){
        // v.display();
        pthread_create(&visitor_threads[i],NULL,visitor_func,(void*)i );
    }

    // while(1);  // to let the other threads finish
    pthread_exit(NULL);

    return 0;
}

