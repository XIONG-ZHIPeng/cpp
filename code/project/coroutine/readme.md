1. thread : 多线程模块，弥补协程的缺点。 使用多线程配合多协程更好的利用多核cpu资源
2. fiber：负载协程的创建、暂停等运行任务的地方
3. scheduler：调度协程的执行与暂停
4. io+scheduler: 使用epoll监听file descriptor上绑定的读写事件，当读写事件触发时将其放入调度器中等待调度
5. timer：定时器，完成定时任务的创建，删除和取消。使用最小堆，将超时定时器触发作为信号tickle来触发ioscheduler等待的epoll_wait
6. hook ： 实现非阻塞的服务器框架，外挂式加载部分io系统函数并添加定时任务