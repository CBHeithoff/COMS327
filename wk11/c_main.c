#include "cpp_function.h"

// g++ cpp_function.cpp -c
// gcc c_main.c -c
// g++ c_main.o cpp_function.o -o c_main

void _Z5printPKc(const char *);

int main(int argc, char *argv[])
{
    //_Z5printPKc("Hello World!");
    print("Hello World!");

    // ostream being void, this is sometimes called an "opaque handle"
    ostream *o;

    o = get_cout();
    print_with_cout(o, "Goodbye, cruel world.\n");

    return 0;
}