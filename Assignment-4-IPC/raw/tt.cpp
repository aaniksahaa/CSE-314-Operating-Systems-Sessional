#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;

void generateVisitor(int id, bool isPremium, int arrivalTimeAtA, int arrivalTimeAtB) {
    std::cout << "Visitor " << id
              << " has arrived at A at timestamp " << arrivalTimeAtA << "\n";

    // Simulate the visitor moving to point B after some random delay
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate time taken to move to B

    std::cout << "Visitor " << id
              << " has arrived at B at timestamp " << arrivalTimeAtB << "\n";
}

void initializeVisitors(int N, int M) {
    // Random number generator for Poisson distribution
    std::default_random_engine generator;
    std::poisson_distribution<int> poissonDist(1.0); // Mean = 1.0 (adjust as needed for inter-arrival rate)

    // Current timestamp tracker
    int currentTimestamp = 0;

    // Generate standard ticket holders (ID: 1001 - 1100)
    for (int i = 0; i < N; ++i) {
        int visitorId = 1001 + i;

        // Simulate random delay using Poisson distribution
        int delayAtA = poissonDist(generator);
        currentTimestamp += delayAtA;  // Add the delay to the current timestamp

        // Generate arrival at point A
        generateVisitor(visitorId, false, currentTimestamp, currentTimestamp + 1);  // Arrival at A, then B after 1 second delay
    }

    // Generate premium ticket holders (ID: 2001 - 2100)
    for (int i = 0; i < M; ++i) {
        int visitorId = 2001 + i;

        // Simulate random delay using Poisson distribution
        int delayAtA = poissonDist(generator);
        currentTimestamp += delayAtA;  // Add the delay to the current timestamp

        // Generate arrival at point A
        generateVisitor(visitorId, true, currentTimestamp, currentTimestamp + 1);  // Arrival at A, then B after 1 second delay
    }
}

default_random_engine generator;
poisson_distribution<int> poissonDist(2.5);

int get_poisson_rand(){
    return poissonDist(generator);
}

int main() {
    int N = 500;  // Number of standard ticket holders
    int M = 3;  // Number of premium ticket holders

    for(int i=0; i<N+M; i++){
        cout<<get_poisson_rand()<<"\n";
    }

    return 0;
}
