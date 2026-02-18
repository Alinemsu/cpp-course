#pragma once
#include <memory>
#include <new>

template<class T, class Alloc = std::allocator<T>>
class Container
{
public:
	class iterator
	{
	public:
		iterator(T* p)
		{
			p_ = p;
		}

		T& operator*()
		{
			return *p_;
		}

		iterator& operator++()
		{
			p_ = p_ + 1;
			return *this;
		}

		bool operator!=(const iterator& other)
		{
			return p_ != other.p_;
		}

	private:
		T* p_;
	};

	Container(size_type capacity)
	{
		data_ = 0;
		size_ = 0;
		capacity_ = capacity;

		data_ = alloc_.allocate(capacity_);
	}

	void push_back(const T& x)
	{
		if (size_ >= capacity_)
			throw std::bad_alloc();

		new (data_ + size_) T(x);
		size_ = size_ + 1;
	}

	iterator begin()
	{
		return iterator(data_);
	}

	iterator end()
	{
		return iterator(data_ + size_);
	}

private:
	Alloc alloc_;
	T* data_;
	size_type size_;
	size_type capacity_;
};