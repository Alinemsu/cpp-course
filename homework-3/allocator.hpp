#pragma once
#include <cstddef>  // std::size_t
#include <cstdlib>  // std::malloc, std::free
#include <new>      // std::bad_alloc

inline std::size_t align_up(std::size_t x, std::size_t a)
{
    if (a <= 1) return x;

    while (x % a != 0)
        x = x + 1;

    return x;
}

struct BufferState
{
    unsigned char* data;
    std::size_t capacity_bytes;
    std::size_t used_bytes;
    unsigned long refs;

    BufferState(std::size_t bytes)
    {
        data = 0;
        capacity_bytes = bytes;
        used_bytes = 0;
        refs = 1;

        if (capacity_bytes != 0)
        {
            data = (unsigned char*)std::malloc(capacity_bytes);
            if (data == 0)
                throw std::bad_alloc();
        }
    }

    ~BufferState()
    {
        std::free(data);
    }

    BufferState(const BufferState&) = delete;
    BufferState& operator=(const BufferState&) = delete;
};

template<class T>
class Allocator
{
public:
    typedef T value_type;

    Allocator(std::size_t capacity_bytes = 0)
    {
        state_ = new BufferState(capacity_bytes);
    }

    Allocator(const Allocator& other)
    {
        state_ = other.state_;
        state_->refs = state_->refs + 1;
    }

    template<class U>
    Allocator(const Allocator<U>& other)
    {
        state_ = other.state_;
        state_->refs = state_->refs + 1;
    }

    ~Allocator()
    {
        state_->refs = state_->refs - 1;
        if (state_->refs == 0)
            delete state_;
    }

    T* allocate(std::size_t n)
    {
        if (n == 0)
            return 0;

        std::size_t bytes = n * sizeof(T);
        std::size_t start = align_up(state_->used_bytes, alignof(T));

        if (start + bytes > state_->capacity_bytes)
            throw std::bad_alloc();

        T* p = (T*)(state_->data + start);
        state_->used_bytes = start + bytes;
        return p;
    }

    void deallocate(T*, std::size_t) {}

    template<class U>
    bool operator==(const Allocator<U>& other) const
    {
        return state_ == other.state_;
    }

    template<class U>
    bool operator!=(const Allocator<U>& other) const
    {
        return state_ != other.state_;
    }

private:
    template<class U>
    friend class Allocator;

    BufferState* state_;
};
