#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>

std::mutex mtx;
std::queue<int> buffer;
std::condition_variable cv;
bool finished=false;
#define MAX_INPUT 10;

void producer(int i){

    for(int j=0; j<10; j++){
        int num = rand() % 100;
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Producer " << i << " produced: " << num << "\n";
            buffer.push(num);
        }
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void consumer(int i){
    
    while(true){   
        
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{ return finished || !buffer.empty();});

                if(buffer.empty() || finished)
                    break;
                    
                int num = buffer.front();
                std::cout << "Consumer " << i << " consumed: " << num << "\n";
                buffer.pop();
            
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main() {

    int number_producer{2};
    int number_consumer{3};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for(int i=0;i<number_producer;i++){
        producers.push_back(std::thread(producer,i));
    }


    for(int i=0;i<number_consumer;i++){
        consumers.push_back(std::thread(consumer,i));
    }

    for(auto&& p :producers){
        p.join();
    }
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
    }
    
    cv.notify_all();

    for(auto&& c :consumers){
        c.join();
    }

}
