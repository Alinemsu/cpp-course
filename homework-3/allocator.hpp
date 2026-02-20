#pragma once
#include <new>

typedef unsigned long long size_type;

inline size_type align_up_alloc(size_type x, size_type a)
{
	if (a <= 1)
		return x;

	while (x % a != 0)
		x = x + 1;

	return x;
}

struct BufferState
{
	unsigned char* data;
	size_type capacity_bytes;
	size_type used_bytes;
	unsigned long refs;

	BufferState(size_type bytes)
	{
		data = 0;
		capacity_bytes = bytes;
		used_bytes = 0;
		refs = 1;

		if (capacity_bytes != 0)
			data = (unsigned char*)::operator new((size_t)capacity_bytes);
	}

	~BufferState()
	{
		::operator delete((void*)data);
	}

	BufferState(const BufferState&) = delete;
	BufferState& operator=(const BufferState&) = delete;
};

template<class T>
class Allocator
{
public:
	typedef T value_type;

	Allocator(size_type capacity_bytes = 0)
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

	T* allocate(size_type n)
	{
		if (n == 0)
			return 0;

		size_type bytes = n * (size_type)sizeof(T);
		size_type start = align_up_alloc(state_->used_bytes, (size_type)alignof(T));

		if (start + bytes > state_->capacity_bytes)
			throw std::bad_alloc();

		T* p = (T*)(state_->data + start);
		state_->used_bytes = start + bytes;

		return p;
	}

	void deallocate(T*, size_type) {}

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
