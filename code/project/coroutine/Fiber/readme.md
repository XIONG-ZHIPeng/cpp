# Fiber基本介绍

Fiber类提供协程基本功能，包括创建、管理、切换和销毁协程。
使用了ucontext_t结构保存和恢复协程的上下文，并通过std::function来存储协程的执行逻辑

## 协程的状态

运行态(RUNNING)，就绪态(READY)和结束态(TERM)

