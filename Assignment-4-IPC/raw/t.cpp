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

#define BLACK_COLOR string("\033[30m")
#define RED_COLOR string("\033[31m")
#define GREEN_COLOR string("\033[32m")
#define YELLOW_COLOR string("\033[33m")
#define BLUE_COLOR string("\033[34m")
#define MAGENTA_COLOR string("\033[35m")
#define CYAN_COLOR string("\033[36m")
#define WHITE_COLOR string("\033[37m")
#define RESET_COLOR string("\033[0m")

int main(){
    string s = YELLOW_COLOR + "hello" + to_string(1) + RESET_COLOR;
    cout<<s<<endl;
    return 0;
}
