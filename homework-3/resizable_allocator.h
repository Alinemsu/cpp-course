#pragma once
#include <new>

typedef unsigned long long size_type;

inline size_type align_up_resize(size_type x, size_type a)
{
    if (a <= 1)
        return x;

    while (x % a != 0)
        x = x + 1;

    return x;
}

struct FreeNode
{
    FreeNode* next;
    size_type bytes;
};

struct Block
{
    unsigned char* data;
    size_type capacity_bytes;
    size_type used_bytes;
    Block* next;
};

struct ResizableState
{
    Block* head;
    Block* current;
    FreeNode* free_head;
    size_type initial_bytes;
    unsigned long refs;

    ResizableState(size_type bytes)
    {
        head = 0;
        current = 0;
        free_head = 0;
        initial_bytes = bytes;
        refs = 1;
    }

    ~ResizableState()
    {
        Block* b = head;

        while (b != 0)
        {
            Block* next = b->next;
            ::operator delete((void*)b->data);
            ::operator delete((void*)b);
            b = next;
        }
    }

    ResizableState(const ResizableState&) = delete;
    ResizableState& operator=(const ResizableState&) = delete;

    size_type pick_capacity(size_type need_bytes)
    {
        size_type cap = initial_bytes;

        if (cap == 0)
            cap = 1024;

        while (cap < need_bytes + 64)
            cap = cap * 2;

        return cap;
    }

    void add_block(size_type cap)
    {
        Block* b = (Block*)::operator new(sizeof(Block));

        b->data = (unsigned char*)::operator new((size_t)cap);
        b->capacity_bytes = cap;
        b->used_bytes = 0;
        b->next = 0;

        if (head == 0)
            head = b;

        if (current != 0)
            current->next = b;

        current = b;
    }
};

template<class T>
class ResizableAllocator
{
public:
    typedef T value_type;

    ResizableAllocator(size_type initial_bytes = 0)
    {
        state_ = new ResizableState(initial_bytes);
    }

    ResizableAllocator(const ResizableAllocator& other)
    {
        state_ = other.state_;
        state_->refs = state_->refs + 1;
    }

    template<class U>
    ResizableAllocator(const ResizableAllocator<U>& other)
    {
        state_ = other.state_;
        state_->refs = state_->refs + 1;
    }

    ~ResizableAllocator()
    {
        state_->refs = state_->refs - 1;
        if (state_->refs == 0)
            delete state_;
    }

    T* allocate(size_type n)
    {
        if (n == 0)
            return 0;

        size_type bytes = n * (size_type)sizeof(T);
        size_type a = (size_type)alignof(T);

        if (n == 1)
            if ((size_type)sizeof(T) >= (size_type)sizeof(FreeNode))
            {
                T* p = this->allocate_from_free_list(bytes);
                if (p != 0)
                    return p;
            }

        if (state_->current == 0)
            state_->add_block(state_->pick_capacity(bytes));

        size_type start = align_up_resize(state_->current->used_bytes, a);

        if (start + bytes > state_->current->capacity_bytes)
        {
            state_->add_block(state_->pick_capacity(bytes));
            start = align_up_resize(state_->current->used_bytes, a);
        }

        if (start + bytes > state_->current->capacity_bytes)
            throw std::bad_alloc();

        T* p = (T*)(state_->current->data + start);
        state_->current->used_bytes = start + bytes;

        return p;
    }

    void deallocate(T* p, size_type n)
    {
        if (p == 0)
            return;

        if (n != 1)
            return;

        if ((size_type)sizeof(T) < (size_type)sizeof(FreeNode))
            return;

        FreeNode* node = (FreeNode*)p;
        node->bytes = (size_type)sizeof(T);
        node->next = state_->free_head;
        state_->free_head = node;
    }

    template<class U>
    bool operator==(const ResizableAllocator<U>& other) const
    {
        return state_ == other.state_;
    }

    template<class U>
    bool operator!=(const ResizableAllocator<U>& other) const
    {
        return state_ != other.state_;
    }

private:
    template<class U>
    friend class ResizableAllocator;

    ResizableState* state_;

    T* allocate_from_free_list(size_type bytes)
    {
        FreeNode* prev = 0;
        FreeNode* cur = state_->free_head;

        while (cur != 0)
        {
            if (cur->bytes == bytes)
            {
                if (prev == 0)
                    state_->free_head = cur->next;

                if (prev != 0)
                    prev->next = cur->next;

                return (T*)cur;
            }

            prev = cur;
            cur = cur->next;
        }

        return 0;
    }
};
