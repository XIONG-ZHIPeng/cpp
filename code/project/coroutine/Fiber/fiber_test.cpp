#include "fiber.h"
#include <vector>

using namespace sylar;

class Scheduler {
public:
// 添加调度任务
    void schedule(std::shared_ptr<Fiber> task) {
        m_tasks.push_back(task);
    }

    // 执行调度任务
    void run() {
        std::cout << " number " << m_tasks.size() << std::endl;

        std::shared_ptr<Fiber> task;
        auto it = m_tasks.begin();
        while (it != m_tasks.end()) {
            task = *it;
            task->resume();
            it++;
        }
        m_tasks.clear();
    }

private:
    std::vector<std::shared_ptr<Fiber>> m_tasks; // 存储调度任务的容器

};

void test_fiber(int i){
    std::cout << "test_fiber " << i << ": Hello!" << std::endl;
}

int main(){
    // 初始化当前线程的主协程
    Fiber::GetThis();

    // 创建调度器
    Scheduler scheduler;

    // 创建多个协程任务并添加到调度器
    for (int i = 0; i < 20; ++i) {
        std::shared_ptr<Fiber> fiber = std::make_shared<Fiber>(std::bind(test_fiber, i));
        scheduler.schedule(fiber);
    }

    // 执行调度器中的任务
    scheduler.run();

    std::cout << "All tasks completed." << std::endl;
    return 0;

}