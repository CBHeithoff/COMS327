#include <iostream>

using namespace std;

typedef void (*f2ptr)();
typedef f2ptr (*f1ptr)() ;

void f2()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

f2ptr f1()
{
    cout << __PRETTY_FUNCTION__ << endl;

    return f2;
}

// f1ptr (*f0())()
f1ptr f0()
{
    cout << __PRETTY_FUNCTION__ << endl;

    return f1;
}

int main(int argc, char *argv[])
{
    // able to do f0()()()()()()()()()(); with arbitary amount of ()
    // can do it manually in C and C++
    // in C++ can overload function and do it with arbitary amount

    f0()()();

    return 0;
}