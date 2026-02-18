#include <iostream>
#include <map>

#include "allocator.h"
#include "resizable_allocator.h"
#include "container.h"

long long factorial(int n)
{
	if (n > 12)
		return 0;

	long long res = 1;

	for (int i = 2; i <= n; i++)
		res = res * (long long)i;

	return res;
}

void print_title(const char* s)
{
	std::cout << s << std::endl;
}

void print_empty_line()
{
	std::cout << std::endl;
}

void fill_std_map(std::map<int, long long>& m)
{
	for (int i = 0; i < 10; i++)
		m[i] = factorial(i);
}

void print_std_map(std::map<int, long long>& m)
{
	std::map<int, long long>::iterator it = m.begin();

	while (it != m.end())
	{
		std::cout << it->first << " " << it->second << std::endl;
		++it;
	}
}

typedef std::pair<const int, long long> Pair;

typedef std::map<int, long long, std::less<int>, Allocator<Pair, 10>> MapWithFixedAllocator;

void fill_map_fixed(MapWithFixedAllocator& m)
{
	for (int i = 0; i < 10; i++)
		m[i] = factorial(i);
}

void print_map_fixed(MapWithFixedAllocator& m)
{
	MapWithFixedAllocator::iterator it = m.begin();

	while (it != m.end())
	{
		std::cout << it->first << " " << it->second << std::endl;
		++it;
	}
}

void fill_container_std(Container<int>& c)
{
	for (int i = 0; i < 10; i++)
		c.push_back(i);
}

void print_container_std(Container<int>& c)
{
	Container<int>::iterator it = c.begin();

	while (it != c.end())
	{
		std::cout << *it << std::endl;
		++it;
	}
}

typedef Container<int, Allocator<int, 10>> ContainerWithFixedAllocator;

void fill_container_fixed(ContainerWithFixedAllocator& c)
{
	for (int i = 0; i < 10; i++)
		c.push_back(i);
}

void print_container_fixed(ContainerWithFixedAllocator& c)
{
	ContainerWithFixedAllocator::iterator it = c.begin();

	while (it != c.end())
	{
		std::cout << *it << std::endl;
		++it;
	}
}

typedef std::map<int, long long, std::less<int>, ResizableAllocator<Pair, 10>> MapResizable;

void fill_map_resizable(MapResizable& m, int count)
{
	for (int i = 0; i < count; i++)
		m[i] = factorial(i);
}

void print_map_resizable(MapResizable& m)
{
	MapResizable::iterator it = m.begin();

	while (it != m.end())
	{
		std::cout << it->first << " " << it->second << std::endl;
		++it;
	}
}

typedef Container<int, ResizableAllocator<int, 10>> ContainerResizable;

void fill_container_resizable(ContainerResizable& c, int count)
{
	for (int i = 0; i < count; i++)
		c.push_back(i);
}

void print_container_resizable(ContainerResizable& c)
{
	ContainerResizable::iterator it = c.begin();

	while (it != c.end())
	{
		std::cout << *it << std::endl;
		++it;
	}
}

int main()
{
	print_title("STD MAP");
	std::map<int, long long> map_std;
	fill_std_map(map_std);
	print_std_map(map_std);
	print_empty_line();

	print_title("STD MAP WITH FIXED ALLOCATOR");
	MapWithFixedAllocator map_fixed;
	fill_map_fixed(map_fixed);
	print_map_fixed(map_fixed);
	print_empty_line();

	print_title("CONTAINER WITH STD ALLOCATOR");
	Container<int> cont_std(10);
	fill_container_std(cont_std);
	print_container_std(cont_std);
	print_empty_line();

	print_title("CONTAINER WITH FIXED ALLOCATOR");
	ContainerWithFixedAllocator cont_fixed(10);
	fill_container_fixed(cont_fixed);
	print_container_fixed(cont_fixed);
	print_empty_line();

	print_title("STD MAP WITH RESIZABLE ALLOCATOR (0..12)");
	MapResizable map_resizable;
	fill_map_resizable(map_resizable, 13);
	print_map_resizable(map_resizable);
	print_empty_line();

	print_title("ERASE KEYS 3, 7, 10 THEN PRINT");
	map_resizable.erase(3);
	map_resizable.erase(7);
	map_resizable.erase(10);
	print_map_resizable(map_resizable);
	print_empty_line();

	print_title("ADD KEYS 30, 31, 32 THEN PRINT");
	map_resizable[30] = factorial(9);
	map_resizable[31] = factorial(8);
	map_resizable[32] = factorial(7);
	print_map_resizable(map_resizable);
	print_empty_line();

	print_title("CONTAINER WITH RESIZABLE ALLOCATOR (0..12)");
	ContainerResizable cont_resizable(13);
	fill_container_resizable(cont_resizable, 13);
	print_container_resizable(cont_resizable);
	print_empty_line();

	return 0;
}