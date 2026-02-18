#pragma once
#include <new>

typedef unsigned long long size_type;

template<class T, size_type limit>
class Allocator
{
public:
	typedef T value_type;

	Allocator() {}

	template<class U>
	Allocator(const Allocator<U, limit>&) {}

	T* allocate(size_type n)
	{
		if (n == 0)
			return 0;

		if (used() + n > limit + 1)
			throw std::bad_alloc();

		T* p = data() + used();
		used() = used() + n;

		return p;
	}

	void deallocate(T*, size_type) {}

	template<class U>
	struct rebind
	{
		typedef Allocator<U, limit> other;
	};

private:
	struct Storage
	{
		T* ptr;

		Storage()
		{
			ptr = (T*)::operator new((limit + 1) * (size_type)sizeof(T));
		}

		~Storage()
		{
			::operator delete(ptr);
		}
	};

	static T* data()
	{
		static Storage s;
		return s.ptr;
	}

	static size_type& used()
	{
		static size_type u = 0;
		return u;
	}
};