#pragma once

#include <cstddef>   // std::size_t, std::max_align_t
#include <cstdlib>   // std::malloc, std::free, std::aligned_alloc (C++17)
#include <new>       // std::bad_alloc
#include <algorithm> // std::max

using size_type = std::size_t;

static inline size_type align_up_resize(size_type x, size_type a) noexcept
{
	
	return (x + (a - 1)) & ~(a - 1);
}

static inline size_type align_up_to_multiple(size_type x, size_type a) noexcept
{
	
	return (x + (a - 1)) / a * a;
}

static inline void* aligned_malloc(size_type alignment, size_type size)
{
	alignment = std::max(alignment, alignof(std::max_align_t));

#if defined(__cpp_aligned_new) || (defined(__cpp_lib_aligned_alloc) && __cpp_lib_aligned_alloc >= 201606L)
	
	size = align_up_to_multiple(size, alignment);
	if (void* p = std::aligned_alloc(alignment, size))
		return p;
	return nullptr;
#else
	
	(void)alignment;
	return std::malloc(size);
#endif
}

static inline void aligned_free(void* p) noexcept
{
	std::free(p);
}

struct FreeNode
{
	FreeNode* next;
	size_type bytes;
};

struct Block
{
	unsigned char* data = nullptr;
	size_type capacity_bytes = 0;
	size_type used_bytes = 0;
	Block* next = nullptr;
};

struct ResizableState
{
	Block* head = nullptr;
	Block* current = nullptr;
	FreeNode* free_head = nullptr;
	size_type initial_bytes = 0;
	unsigned long refs = 1;

	explicit ResizableState(size_type bytes) : initial_bytes(bytes) {}

	~ResizableState()
	{
		for (Block* b = head; b;)
		{
			Block* next = b->next;
			aligned_free(b->data);
			std::free(b);
			b = next;
		}
	}

	ResizableState(const ResizableState&) = delete;
	ResizableState& operator=(const ResizableState&) = delete;

	size_type pick_capacity(size_type need_bytes) const noexcept
	{
		size_type cap = initial_bytes ? initial_bytes : 1024;
		while (cap < need_bytes + 64)
			cap *= 2;
		return cap;
	}

	void add_block(size_type cap, size_type data_alignment)
	{
		Block* b = static_cast<Block*>(std::malloc(sizeof(Block)));
		if (!b) throw std::bad_alloc();

		// Value-init fields
		*b = Block{};

		b->data = static_cast<unsigned char*>(aligned_malloc(data_alignment, cap));
		if (!b->data)
		{
			std::free(b);
			throw std::bad_alloc();
		}

		b->capacity_bytes = cap;

		if (!head) head = b;
		if (current) current->next = b;
		current = b;
	}
};

template <class T>
class ResizableAllocator
{
public:
	using value_type = T;

	explicit ResizableAllocator(size_type initial_bytes = 0)
		: state_(new ResizableState(initial_bytes))
	{
	}

	ResizableAllocator(const ResizableAllocator& other) noexcept
		: state_(other.state_)
	{
		++state_->refs;
	}

	template <class U>
	ResizableAllocator(const ResizableAllocator<U>& other) noexcept
		: state_(other.state_)
	{
		++state_->refs;
	}

	~ResizableAllocator()
	{
		if (--state_->refs == 0)
			delete state_;
	}

	T* allocate(size_type n)
	{
		if (n == 0) return nullptr;

		const size_type bytes = n * sizeof(T);
		const size_type a = alignof(T);

		if (n == 1 && sizeof(T) >= sizeof(FreeNode))
		{
			if (T* p = allocate_from_free_list(bytes))
				return p;
		}

		if (!state_->current)
			state_->add_block(state_->pick_capacity(bytes), a);

		size_type start = align_up_resize(state_->current->used_bytes, a);
		if (start + bytes > state_->current->capacity_bytes)
		{
			state_->add_block(state_->pick_capacity(bytes), a);
			start = align_up_resize(state_->current->used_bytes, a);
		}

		if (start + bytes > state_->current->capacity_bytes)
			throw std::bad_alloc();

		T* p = reinterpret_cast<T*>(state_->current->data + start);
		state_->current->used_bytes = start + bytes;
		return p;
	}

	void deallocate(T* p, size_type n) noexcept
	{
		if (!p) return;
		if (n != 1) return;
		if (sizeof(T) < sizeof(FreeNode)) return;

		auto* node = reinterpret_cast<FreeNode*>(p);
		node->bytes = sizeof(T);
		node->next = state_->free_head;
		state_->free_head = node;
	}

	template <class U>
	bool operator==(const ResizableAllocator<U>& other) const noexcept
	{
		return state_ == other.state_;
	}

	template <class U>
	bool operator!=(const ResizableAllocator<U>& other) const noexcept
	{
		return state_ != other.state_;
	}

private:
	template <class U>
	friend class ResizableAllocator;

	ResizableState* state_;

	T* allocate_from_free_list(size_type bytes) noexcept
	{
		FreeNode** link = &state_->free_head;
		while (*link)
		{
			FreeNode* node = *link;
			if (node->bytes == bytes)
			{
				*link = node->next;
				return reinterpret_cast<T*>(node);
			}
			link = &node->next;
		}
		return nullptr;
	}
};
