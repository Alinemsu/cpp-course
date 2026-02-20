#pragma once
#include <memory>
#include <new>

typedef unsigned long long size_type;

template<class T, class Alloc = std::allocator<T>>
class Container
{
private:
	struct Node
	{
		T value;
		Node* next;
	};

	typedef typename std::allocator_traits<Alloc>::template rebind_alloc<Node> NodeAlloc;

public:
	class iterator
	{
	public:
		iterator(Node* p)
		{
			current_ = p;
		}

		T& operator*()
		{
			return current_->value;
		}

		iterator& operator++()
		{
			current_ = current_->next;
			return *this;
		}

		bool operator!=(const iterator& other)
		{
			return current_ != other.current_;
		}

	private:
		Node* current_;
	};

	Container(const Alloc& a = Alloc())
		: alloc_(a)
	{
		head_ = 0;
		tail_ = 0;
		size_ = 0;
	}

	~Container()
	{
		Node* current = head_;

		while (current != 0)
		{
			Node* next = current->next;

			std::allocator_traits<NodeAlloc>::destroy(alloc_, current);
			std::allocator_traits<NodeAlloc>::deallocate(alloc_, current, 1);

			current = next;
		}
	}

	Container(const Container&) = delete;
	Container& operator=(const Container&) = delete;

	void push_back(const T& x)
	{
		Node* n = std::allocator_traits<NodeAlloc>::allocate(alloc_, 1);

		std::allocator_traits<NodeAlloc>::construct(alloc_, n);

		n->value = x;
		n->next = 0;

		if (tail_ == 0)
			head_ = n;

		if (tail_ != 0)
			tail_->next = n;

		tail_ = n;
		size_ = size_ + 1;
	}

	iterator begin()
	{
		return iterator(head_);
	}

	iterator end()
	{
		return iterator(0);
	}

	size_type size() const
	{
		return size_;
	}

private:
	NodeAlloc alloc_;
	Node* head_;
	Node* tail_;
	size_type size_;
};
