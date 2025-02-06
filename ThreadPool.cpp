#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>


class ThreadPool{

    bool stop;
    std::mutex mutexqueue;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    void worker(){
        while(true){
            std::function<void()>task;
            {
                std::unique_lock<std::mutex> lock(mutexqueue);
                cv.wait(lock, [this](){ return stop || !tasks.empty();});

                if(stop && tasks.empty())
                    return;

                task = tasks.front();    
                tasks.pop();
            }
            task();
        }
    }

    public:

    ThreadPool(size_t numThreads):stop(false){
        for(int i=0;i<numThreads;i++){
            workers.emplace_back(&ThreadPool::worker,this);
        }
    }
    void enqueueTask(std::function<void()> task){
        {
            std::lock_guard<std::mutex> lock(mutexqueue);
            tasks.push(task);
        }
        cv.notify_one();
    }

    ~ThreadPool(){
        
        {
            std::lock_guard<std::mutex> lock(mutexqueue);
            stop=true;
        }
        cv.notify_all();

        for(std::thread &worker:workers){
                worker.join();
        }
    }

};
int main(){

    ThreadPool pool(4);

    for(int i=0;i<10;i++){
        pool.enqueueTask([i](){
            std::cout<<" executing tasl "<<i<<" \n";
        });
    }

}
