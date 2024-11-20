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

/*
#define BLACK_COLOR "\033[30m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR "\033[34m"
#define MAGENTA_COLOR "\033[35m"
#define CYAN_COLOR "\033[36m"
#define WHITE_COLOR "\033[37m"
#define RESET_COLOR "\033[0m"
*/

#define BLACK_COLOR string("\033[30m")
#define RED_COLOR string("\033[31m")
#define GREEN_COLOR string("\033[32m")
#define YELLOW_COLOR string("\033[33m")
#define BLUE_COLOR string("\033[34m")
#define MAGENTA_COLOR string("\033[35m")
#define CYAN_COLOR string("\033[36m")
#define WHITE_COLOR string("\033[37m")
#define RESET_COLOR string("\033[0m")


#define int long long int
#define NUM_STAIR_STEPS 3
#define GALLERY_1_MAX 5
#define GLASS_CORRIDOR_MAX 3
#define INTER_ARRIVAL_RATE 2
#define STAIR_STEP_DELAY_MIN 1
#define STAIR_STEP_DELAY_MAX 2
#define GLASS_CORRIDOR_DELAY_MIN 1
#define GLASS_CORRIDOR_DELAY_MAX 3

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
uniform_int_distribution<int> stair_step_delay(STAIR_STEP_DELAY_MIN, STAIR_STEP_DELAY_MAX);
uniform_int_distribution<int> glass_corridor_delay(GLASS_CORRIDOR_DELAY_MIN, GLASS_CORRIDOR_DELAY_MAX);

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

bool FILE_INPUT = 0;
bool FILE_OUTPUT = 0;

vector<Visitor>visitors;
int N,M;
int w,x,y,z;

int standard_count = 0, premium_count = 0;

// semaphores
pthread_mutex_t stair_locks[NUM_STAIR_STEPS+1];
sem_t gallery_1_remaining_space;
sem_t glass_corridor_remaining_space;

// lock to avoid interleaving in printing
pthread_mutex_t print_lock;

// these are mutexes for access to the count variables
pthread_mutex_t standard_count_lock, premium_count_lock;

// Intuition:
// this is Reader-Writer with Writer preference
// The reader_allowed mutex is locked from the time when
// the first writer comes to when the last writer exits
// more importantly, readers are not even allowed to increase their counts in this period
// so that new incoming writers always get priority
// however, in case of writers, we do it differently
// writers can increase their count immediately
// again, from the first reader, to the exiting reader, we also
// lock the writer_allowed mutex
// note that this does not allow new incoming readers to get access while a writer is waiting
// rather this just makes sure that, while readers are getting shared access
// no writer can get inside

// these are mutexes indicating whether
// standard is allowed to visit or premium is allowed to visit
// say, when standard_access_allowed is locked, this means
// no standard visitor can access the photo booth
// and, say, when premium_access_allowed is locked, this means
// no premium visitor can access the photo booth
// and vice versa for unlocked
pthread_mutex_t standard_access_allowed, premium_access_allowed;


// this lock is needed to ensure that a new premium
// does not starve due to a long queue of waiting standards
// we basically pick one from the queue of the standards
// and then we make that one standard visitor
// wait on the standard_access_allowed lock
// because if rather many standards are waiting on the same lock
// a new premium cannot easily acquire that lock
pthread_mutex_t standard_long_queue_lock;

/******** Globals End ********/

void init_semaphore()
{
    for(int i=1; i<=NUM_STAIR_STEPS; i++){
        pthread_mutex_init(&stair_locks[i],0);
    }

    sem_init(&gallery_1_remaining_space,0,GALLERY_1_MAX);
    sem_init(&glass_corridor_remaining_space,0,GLASS_CORRIDOR_MAX);

    pthread_mutex_init(&print_lock,0);

    pthread_mutex_init(&standard_count_lock,0);
    pthread_mutex_init(&premium_count_lock,0);

    pthread_mutex_init(&standard_access_allowed,0);
    pthread_mutex_init(&premium_access_allowed,0);
}

string removeColors(const string& input) {
    static const regex colorCodeRegex("\033\\[\\d+(;\\d+)*m");
    return regex_replace(input, colorCodeRegex, "");
}

void safe_print(string s){
    if(FILE_OUTPUT){
        s = removeColors(s);
    }
    pthread_mutex_lock(&print_lock);
    cout<<s<<endl;
    pthread_mutex_unlock(&print_lock);
}

// mimics reader
// shared access in task 3
void* standard_visitor_func(void* visitor_index){
    Visitor visitor = visitors[(int)visitor_index];
    sleep(visitor.arrival_delay);
    safe_print("Visitor " + to_string(visitor.id) + " has arrived at A at timestamp " + to_string(get_current_timestamp_s()));
    sleep(w);
    safe_print("Visitor " + to_string(visitor.id) + " has arrived at B at timestamp " + to_string(get_current_timestamp_s()));

    pthread_mutex_lock(&stair_locks[1]);

    int at_step;
    for(at_step=1; at_step<=NUM_STAIR_STEPS-1; at_step++){
        safe_print("Visitor " + to_string(visitor.id) + " is at step " + to_string(at_step) + " at timestamp " + to_string(get_current_timestamp_s()));

        sleep(stair_step_delay(rand_generator));

        pthread_mutex_lock(&stair_locks[at_step+1]);    // next step

        pthread_mutex_unlock(&stair_locks[at_step]);    // current step
    }
    safe_print("Visitor " + to_string(visitor.id) + " is at step " + to_string(at_step) + " at timestamp " + to_string(get_current_timestamp_s()));

    sleep(stair_step_delay(rand_generator));

    sem_wait(&gallery_1_remaining_space);

    pthread_mutex_unlock(&stair_locks[at_step]);
    safe_print("Visitor " + to_string(visitor.id) + " is at C(entered Gallery 1) at timestamp " + to_string(get_current_timestamp_s()));

    sleep(x);

    sem_wait(&glass_corridor_remaining_space);

    sem_post(&gallery_1_remaining_space);
    safe_print("Visitor " + to_string(visitor.id) + " is at D(exiting Gallery 1) at timestamp " + to_string(get_current_timestamp_s()));

    sleep(glass_corridor_delay(rand_generator));
    // since Gallery 2 has enough capacity, no checking needed here
    sem_post(&glass_corridor_remaining_space);
    safe_print("Visitor " + to_string(visitor.id) + " is at E(entered Gallery 2) at timestamp " + to_string(get_current_timestamp_s()));

    // upto this part common

    // action in Gallery 2 before going to photo booth
    sleep(y);

    safe_print(YELLOW_COLOR + "Visitor " + to_string(visitor.id) + " is about to enter the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);

    pthread_mutex_lock(&standard_long_queue_lock);
    pthread_mutex_lock(&standard_access_allowed); // this mutex is unlocked only when there is no waiting or active writer
    pthread_mutex_lock(&standard_count_lock);
    standard_count += 1;
    if(standard_count == 1){
        pthread_mutex_lock(&premium_access_allowed);
    }
    pthread_mutex_unlock(&standard_count_lock);
    pthread_mutex_unlock(&standard_access_allowed);
    pthread_mutex_unlock(&standard_long_queue_lock);

    safe_print(CYAN_COLOR + "Visitor " + to_string(visitor.id) + " is inside the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);

    // photo booth actions
    sleep(z);

    safe_print(RED_COLOR + "Visitor " + to_string(visitor.id) + " exited the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);


    pthread_mutex_lock(&standard_count_lock);
    standard_count -= 1;
    if(standard_count == 0){
        pthread_mutex_unlock(&premium_access_allowed);
    }
    pthread_mutex_unlock(&standard_count_lock);

    return NULL;
}

// mimics writer
// prioritized exclusive access in task 3
void* premium_visitor_func(void* visitor_index){
    Visitor visitor = visitors[(int)visitor_index];
    sleep(visitor.arrival_delay);
    safe_print("Visitor " + to_string(visitor.id) + " has arrived at A at timestamp " + to_string(get_current_timestamp_s()));
    sleep(w);
    safe_print("Visitor " + to_string(visitor.id) + " has arrived at B at timestamp " + to_string(get_current_timestamp_s()));

    pthread_mutex_lock(&stair_locks[1]);

    int at_step;
    for(at_step=1; at_step<=NUM_STAIR_STEPS-1; at_step++){
        safe_print("Visitor " + to_string(visitor.id) + " is at step " + to_string(at_step) + " at timestamp " + to_string(get_current_timestamp_s()));

        sleep(stair_step_delay(rand_generator));

        pthread_mutex_lock(&stair_locks[at_step+1]);    // next step

        pthread_mutex_unlock(&stair_locks[at_step]);    // current step
    }
    safe_print("Visitor " + to_string(visitor.id) + " is at step " + to_string(at_step) + " at timestamp " + to_string(get_current_timestamp_s()));

    sleep(stair_step_delay(rand_generator));

    sem_wait(&gallery_1_remaining_space);

    pthread_mutex_unlock(&stair_locks[at_step]);
    safe_print("Visitor " + to_string(visitor.id) + " is at C(entered Gallery 1) at timestamp " + to_string(get_current_timestamp_s()));

    sleep(x);

    sem_wait(&glass_corridor_remaining_space);

    sem_post(&gallery_1_remaining_space);
    safe_print("Visitor " + to_string(visitor.id) + " is at D(exiting Gallery 1) at timestamp " + to_string(get_current_timestamp_s()));

    sleep(glass_corridor_delay(rand_generator));
    // since Gallery 2 has enough capacity, no checking needed here
    sem_post(&glass_corridor_remaining_space);
    safe_print("Visitor " + to_string(visitor.id) + " is at E(entered Gallery 2) at timestamp " + to_string(get_current_timestamp_s()));

    // upto this part common

    // action in Gallery 2 before going to photo booth
    // this added delay here is just for demonstration convenience
    sleep(y+1);

    safe_print(GREEN_COLOR + "Visitor " + to_string(visitor.id) + " is about to enter the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);

    pthread_mutex_lock(&premium_count_lock);
    premium_count += 1;
    if(premium_count == 1){
        pthread_mutex_lock(&standard_access_allowed);
    }
    pthread_mutex_unlock(&premium_count_lock);


    // exclusive access for premium
    pthread_mutex_lock(&premium_access_allowed);
    safe_print(CYAN_COLOR + "Visitor " + to_string(visitor.id) + " is inside the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);

    // photo booth actions
    sleep(z);
    safe_print(RED_COLOR + "Visitor " + to_string(visitor.id) + " exited the photobooth at timestamp " + to_string(get_current_timestamp_s()) + RESET_COLOR);
    pthread_mutex_unlock(&premium_access_allowed);


    pthread_mutex_lock(&premium_count_lock);
    premium_count -= 1;
    if(premium_count == 0){
        pthread_mutex_unlock(&standard_access_allowed);
    }
    pthread_mutex_unlock(&premium_count_lock);

    return NULL;
}

int32_t main() {
    cout<<"Usage: "<<endl;

    cout<<WHITE_COLOR + "1. Points of shared access can be manually inspected by the visitor IDs" + RESET_COLOR << endl;
    cout<<WHITE_COLOR + "2. To find places where premium got priority, look for the color pattern " + YELLOW_COLOR + "YELLOW" + WHITE_COLOR + " [...white] " + GREEN_COLOR + "GREEN" + WHITE_COLOR + " or " + GREEN_COLOR + "GREEN" + WHITE_COLOR + " [...white] " + YELLOW_COLOR + "YELLOW" + RESET_COLOR <<endl;
    cout<<WHITE_COLOR + "3. To see times of a specific visitor, use " + CYAN_COLOR + "Ctrl + Shift + F" + WHITE_COLOR + " on that visitor ID." + RESET_COLOR << endl;

    if(!FILE_INPUT){
        printf("\nPlease input in this format: \n");
        printf("N M\n");
        printf("w x y z\n\n");
    }

    // N = 10, M = 6;
    // w = 2, x = 6, y = 6, z = 3;

    if(FILE_INPUT){
        freopen("input.txt", "r", stdin);
    }
    if(FILE_OUTPUT){
        freopen("output.txt", "w", stdout);
    }

    cin>>N>>M;
    cin>>w>>x>>y>>z;

    if(!FILE_OUTPUT){
        cout<<"\n";
    }

    init_semaphore();

    for(int i=0; i<N; i++){
        visitors.push_back(Visitor(1001+i, false, arrival_delay_poisson_dist(rand_generator)));
    }
    for(int i=0; i<M; i++){
        visitors.push_back(Visitor(2001+i, true, arrival_delay_poisson_dist(rand_generator)));
    }

    pthread_t visitor_threads[N+M];

    starting_time_s = time(nullptr);

    for(int i=0; i<N+M; i++){
        // v.display();
        if(!visitors[i].is_premium){
            pthread_create(&visitor_threads[i],NULL,standard_visitor_func,(void*)i );
        } else {
            pthread_create(&visitor_threads[i],NULL,premium_visitor_func,(void*)i );
        }
    }

    // while(1);  // to let the other threads finish
    pthread_exit(NULL);

    return 0;
}

/*
10 6
2 6 6 3


10 6
2 1 6 3


*/

