// g++ main.cpp -o main.exe --std=c++17
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

class A
{
public:
    A() {}
    ~A() {}
    std::string str{"hello"};
};

int main(int argc, char **argv)
{
    void *mem = ::malloc(sizeof(A));
    if (!mem)
    {
        exit(EXIT_FAILURE);
    }
    A *ptrA = new (mem) A;
    if (!ptrA)
    {
        exit(EXIT_FAILURE);
    }
    cout << ptrA->str << endl; // hello
    ptrA->~A();
    free(mem);
    mem = nullptr;
    return 0;
}