#include <iostream>
#include <map>
#include <utility>

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

int main()
{
    typedef std::pair<const int, long long> Pair;

    print_title("STD MAP");
    std::map<int, long long> map_std;

    for (int i = 0; i < 10; i++)
        map_std[i] = factorial(i);

    for (auto it = map_std.begin(); it != map_std.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;

    print_empty_line();

    print_title("STD MAP WITH FIXED ALLOCATOR (BYTES LIMIT)");
    std::map<int, long long, std::less<int>, Allocator<Pair>> map_fixed((Allocator<Pair>(65536)));

    for (int i = 0; i < 10; i++)
        map_fixed[i] = factorial(i);

    for (auto it = map_fixed.begin(); it != map_fixed.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;

    print_empty_line();

    print_title("CONTAINER WITH STD ALLOCATOR");
    Container<int> cont_std;

    for (int i = 0; i < 10; i++)
        cont_std.push_back(i);

    for (auto it = cont_std.begin(); it != cont_std.end(); ++it)
        std::cout << *it << std::endl;

    print_empty_line();

    print_title("CONTAINER WITH FIXED ALLOCATOR (BYTES LIMIT)");
    Container<int, Allocator<int>> cont_fixed((Allocator<int>(256)));

    for (int i = 0; i < 10; i++)
        cont_fixed.push_back(i);

    for (auto it = cont_fixed.begin(); it != cont_fixed.end(); ++it)
        std::cout << *it << std::endl;

    print_empty_line();

    print_title("STD MAP WITH RESIZABLE ALLOCATOR (0..12)");
    std::map<int, long long, std::less<int>, ResizableAllocator<Pair>> map_resizable((ResizableAllocator<Pair>(1024)));

    for (int i = 0; i < 13; i++)
        map_resizable[i] = factorial(i);

    for (auto it = map_resizable.begin(); it != map_resizable.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;

    print_empty_line();

    print_title("CONTAINER WITH RESIZABLE ALLOCATOR (0..12)");
    Container<int, ResizableAllocator<int>> cont_resizable((ResizableAllocator<int>(256)));

    for (int i = 0; i < 13; i++)
        cont_resizable.push_back(i);

    for (auto it = cont_resizable.begin(); it != cont_resizable.end(); ++it)
        std::cout << *it << std::endl;

    print_empty_line();

    return 0;
}
