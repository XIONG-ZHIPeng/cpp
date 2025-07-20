#include "thread.h"
#include <vector>
#include <memory>
#include <iostream>
#include <unistd.h>

using namespace sylar; // 使用sylar命名空间，避免每次使用时都需要加上前缀

void func(){
    std::cout << "ID: " << Thread::GetThreadId() 
              << " Name: " << Thread::GetName();
    std::cout << ", this id: " << Thread::GetThis()->getId() 
              << " this name: " << Thread::GetThis()->getName() << std::endl;

    sleep(5);
}

int main() {
    std::vector<std::shared_ptr<Thread>> threads;
    for (int i = 0; i < 5; ++i) {
        std::string name = "Thread-" + std::to_string(i);
        auto thread = std::make_shared<Thread>(func, name);
        threads.push_back(thread);
    }

    for (auto& thread : threads) {
        thread->join(); // 等待所有线程完成
    }

    return 0;
}