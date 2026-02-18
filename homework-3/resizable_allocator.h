#pragma once
#include <new>

typedef unsigned long long size_type;

template<class T, size_type initial_capacity>
class ResizableAllocator
{
public:
	typedef T value_type;

	ResizableAllocator() {}

	template<class U>
	ResizableAllocator(const ResizableAllocator<U, initial_capacity>&) {}

	T* allocate(size_type n)
	{
		if (n == 0)
			return 0;

		Storage& s = storage();

		if (n == 1)
		{
			if (s.free_head != 0)
			{
				FreeNode* node = s.free_head;
				s.free_head = node->next;

				Slot* slot = (Slot*)node;
				return (T*)slot;
			}
		}

		if (s.current == 0)
			s.add_block(initial_capacity);

		if (s.current->used + n > s.current->capacity)
		{
			size_type new_capacity = s.current->capacity * 2;
			if (new_capacity < n)
				new_capacity = n;

			s.add_block(new_capacity);
		}

		Slot* base = s.current->data + s.current->used;
		s.current->used = s.current->used + n;

		return (T*)base;
	}

	void deallocate(T* p, size_type n)
	{
		if (p == 0)
			return;

		if (n != 1)
			return;

		Storage& s = storage();

		FreeNode* node = (FreeNode*)p;
		node->next = s.free_head;
		s.free_head = node;
	}

	template<class U>
	struct rebind
	{
		typedef ResizableAllocator<U, initial_capacity> other;
	};

private:
	struct FreeNode
	{
		FreeNode* next;
	};

	union Slot
	{
		FreeNode free;
		T value;
	};

	struct Block
	{
		Slot* data;
		size_type capacity;
		size_type used;
		Block* next;
	};

	struct Storage
	{
		Block* head;
		Block* current;
		FreeNode* free_head;

		Storage()
		{
			head = 0;
			current = 0;
			free_head = 0;
		}

		~Storage()
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

		void add_block(size_type capacity)
		{
			Block* b = (Block*)::operator new(sizeof(Block));
			b->data = (Slot*)::operator new(sizeof(Slot) * capacity);
			b->capacity = capacity;
			b->used = 0;
			b->next = 0;

			if (head == 0)
				head = b;
			else
				current->next = b;

			current = b;
		}
	};

	static Storage& storage()
	{
		static Storage s;
		return s;
	}
};