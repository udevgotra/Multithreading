#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int counter{0};

void foo(){
    std::scoped_lock lock(mtx);  // Lock once for efficiency
    for(int i = 0; i < 5; i++){
        ++counter;
    }
}

int main(){
    std::thread t1(foo);
    std::thread t2(foo);

    t1.join();
    t2.join();

    {   
        std::scoped_lock lock(mtx);
        std::cout << "Counter is = " << counter << std::endl;
    }
    
    return 0;
}
