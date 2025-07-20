# std::condition_variable类介绍

1. std::condition_variable 是条件变量。
2. 当 std::condition_variable 对象的某个 wait 函数被调用的时候，它使用 std::unique_lock(通过 std::mutex) 来锁住当前线程。当前线程会一直被阻塞，直到另外一个线程在相同的 std::condition_variable 对象上调用了 notification 函数来唤醒当前线程。
3. std::condition_variable 对象通常使用 std::unique_lock<std::mutex> 来等待，如果需要使用另外的 lockable 类型，可以使用 std::condition_variable_any 类

## std::condition_variable 构造函数

default| condition_variable|
-------|-------------------|
copy [deleted] | condition_variable (const condition_variable&) = delete;//拷贝构造函数被禁用|

## std::condition_variable::wait()

unconditional| void wait (unique_lock<mutex>& lck);|
-------|-------------------|
predicate | template \<class Predicate\> <br> void wait (unique_lock<mutex>& lck, Predicate pred);|

1.  std::condition_variable 提供了两种 wait() 函数。当前线程调用 wait() 后将被阻塞(此时当前线程应该获得了锁（mutex），不妨设获得锁 lck)，直到另外某个线程调用 notify_* 唤醒了当前线程，该函数会自动调用 lck.unlock() 释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。
2.  在第二种情况下（即设置了 Predicate），只有当 pred 条件为 false 时调用 wait() 才会阻塞当前线程，并且在收到其他线程的通知后只有当 pred 为 true 时才会被解除阻塞。

## std::condition_variable::wait_for()
unconditional| template \<class Rep, class Period\> <br> cv_status wait_for (unique_lock<mutex>& lck, const chrono::duration<Rep,Period>& rel_time);|
-------|-------------------|
predicate | template <class Rep, class Period, class Predicate> <br> bool wait_for (unique_lock<mutex>& lck, const chrono::duration<Rep,Period>& rel_time, Predicate pred);|


与 std::condition_variable::wait() 类似，不过 wait_for 可以指定一个时间段，在当前线程收到通知或者指定的时间 rel_time 超时之前，该线程都会处于阻塞状态。而一旦超时或者收到了其他线程的通知，wait_for 返回，剩下的处理步骤和 wait() 类似。

## std::condition_variable::wait_until（）

与 std::condition_variable::wait_for 类似，但是 wait_until 可以指定一个时间点，在当前线程收到通知或者指定的时间点 abs_time 超时之前，该线程都会处于阻塞状态。而一旦超时或者收到了其他线程的通知，wait_until 返回，剩下的处理步骤和 wait_for() 类似。

## std::condition_variable::notify_one()

 唤醒某个等待(wait)线程。如果当前没有等待线程，则该函数什么也不做，如果同时存在多个等待线程，则唤醒某个线程是不确定的(unspecified)。
## std::condition_variable::notify_all()

 唤醒所有的等待(wait)线程。如果当前没有等待线程，则该函数什么也不做。

# unique_lock 与  lock_guard

unique_lock 允许灵活的加锁和解锁
lock_guard必须在构造时锁定，析构时解锁