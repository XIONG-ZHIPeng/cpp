#include <memory>
#include <iostream>
#include <atomic>

template<typename T>
struct ControlBlock
{
    T* ptr; // 指向实际对象的指针
    std::atomic<size_t> shared_cnt{1}; // 共享引用计数
    std::atomic<size_t> weak_cnt{0}; // 弱引用计数
    std::function<void(T*)> deleter; // 自定义删除器

    ControlBlock(T* p, std::function<void(T*)> d)
        : ptr(p), deleter(d) {}
    
};

template<typename T>
class SharedPtr
{
private:
    ControlBlock<T>* cb_ = nullptr; // 控制块指针

    explicit SharedPtr(ControlBlock<T>* cb) 
        : cb_(cb) {}

    // 释放计数
    void dec_ref() noexcept
    {
        if (!cb_) return;
        if (--cb_->shared_cnt == 0) // 最后一个共享指针
        {
            cb_->deleter(cb_->ptr); // 调用自定义删除器
            if (cb_->weak_cnt == 0) // 如果没有弱引用
            {
                delete cb_; // 删除控制块
            }
            cb_ = nullptr; // 清空控制块指针
        }
    }

public:
    // 默认构造函数
    SharedPtr() noexcept = default;

    // 裸指针构造函数
    explicit SharedPtr(T* p)
        : cb_(new ControlBlock<T>(p, [](T* ptr) { delete ptr; })) {}

    // 拷贝构造函数
    SharedPtr(const SharedPtr& other) noexcept
        : cb_(other.cb_)
        {
            if (cb_) {
                ++cb_->shared_cnt; // 增加共享引用计数
            }
        }

    // 移动构造函数
    SharedPtr(SharedPtr&& other) noexcept
        : cb_(other.cb_) 
        {
            other.cb_ = nullptr; // 清空其他的控制块指针
        }
    
    // 拷贝赋值运算符
    SharedPtr& operator=(const SharedPtr& other) noexcept
    {
        if (this != &other) 
        {
            dec_ref(); // 释放当前引用
            cb_ = other.cb_; // 复制控制块指针
            if (cb_) {
                ++cb_->shared_cnt; // 增加共享引用计数
            }
        }

        return *this;
    }

    // 移动赋值运算符
    SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        if (this != &other) 
        {
            dec_ref(); // 释放当前引用
            cb_ = other.cb_; // 复制控制块指针
            other.cb_ = nullptr; // 清空其他的控制块指针
        }

        return *this;
    }

    // 析构函数
    ~SharedPtr() 
    {
        dec_ref(); // 释放引用
    }

    // Observer API
    T* get() const noexcept 
    {
        return cb_ ? cb_->ptr : nullptr; // 返回指向对象的指针
    }

    T& operator*() const noexcept 
    {
        return *get(); // 解引用操作符
    }

    T* operator->() const noexcept 
    {
        return get(); // 成员访问操作符
    }

    // --- make_shared API ---
    template<typename... Args>
    static SharedPtr<T> make_shared(Args&&... args)
    {
        struct CBWithObject : ControlBlock<T>
        {
            alignas(T) unsigned char storage[sizeof(T)]; // 对象存储空间
            CBWithObject(Args&&... a)
                : ControlBlock<T>(reinterpret_cast<T*>(storage), [](T* ptr) { ptr->~T(); })
            {
                new (storage) T(std::forward<Args>(a)...); // 在存储空间中构造对象
            }
        };

        auto* cb = new CBWithObject(std::forward<Args>(args)...);
        return SharedPtr(cb); // 返回SharedPtr对象
    }
};


template<typename T>
class WeakPtr {
    ControlBlock<T>* cb_ = nullptr;

public:
    WeakPtr() = default;
    WeakPtr(const SharedPtr<T>& sp) noexcept : cb_(sp.cb_) {   //①
        if (cb_) ++cb_->weak_cnt;
    }
    WeakPtr(const WeakPtr& other) noexcept : cb_(other.cb_) {
        if (cb_) ++cb_->weak_cnt;
    }

    WeakPtr& operator=(const WeakPtr& o) noexcept {            //②
        if (this != &o) {
            reset();
            cb_ = o.cb_;
            if (cb_) ++cb_->weak_cnt;
        }
        return *this;
    }

    ~WeakPtr() { reset(); }

    void reset() noexcept {                                    //③
        if (cb_ && --cb_->weak_cnt == 0 && cb_->shared_cnt.load()==0)
            delete cb_;                                        //④ 最后观察者离开
        cb_ = nullptr;
    }

    // 观察功能
    bool expired() const noexcept { 
        return !cb_ || cb_->shared_cnt.load() == 0;
    }

    SharedPtr<T> lock() const noexcept {                       //⑤
        return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

private:
    // 让 SharedPtr 的“私有构造”可以创建 alias
    friend class SharedPtr<T>;
};
