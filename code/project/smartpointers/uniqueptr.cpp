#include <iostream>
#include <memory>

template <typename T, typename D = std::default_delete<T>>
class UniquePtr
{
private:
    T* ptr_ = nullptr; // Pointer to the managed object
    D deleter_; // Deleter for the managed object

public:
    // Constructor
    explicit UniquePtr(T* p = nullptr, D d = D()) noexcept
        : ptr_(p), deleter_(d) {}

    // Move constructor
    UniquePtr(UniquePtr&& other) noexcept
        : ptr_(other.release()), deleter_(std::move(other.deleter_)) {}
        
    // Forbidden copy constructor
    UniquePtr(const UniquePtr&) = delete;
    // Forbidden copy assignment operator
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move assignment operator
    UniquePtr& operator=(UniquePtr&& other) noexcept 
    {
        if (this != &other)
        {
            reset(other.release());
        }

        return *this;
    }

    // Destructor
    ~UniquePtr()
    {
        reset();
    }

    // basic operations
    // get the raw pointer
    T* get() const noexcept { return ptr_; }
    // dereference operator
    T& operator*() const { return *ptr_; }
    // arrow operator
    T* operator->() const noexcept { return ptr_; }

    // release ownership of the managed object
    T* release() noexcept
    {
        T* temp = ptr_;
        ptr_ = nullptr; // Set to nullptr to avoid double deletion
        return temp;
    }

    // reset the managed object
    void reset(T* p = nullptr) noexcept
    {
        if (ptr_) deleter_(ptr_); // Call the deleter on the current object
        ptr_ = p; // Assign the new pointer
    }
    

};