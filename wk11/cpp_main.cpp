#include "c_function.h"

// gcc c_function.c -c
// g++ cpp_main.cpp -c
// g++ cpp_main.o c_function.o -o cpp_main

//strings c_function.o
//strings cpp_main.o

// _Z5printPKc
// always _Z, 5 is length of name, print is name, pointer P to const K char c

int main(int argc, char *argv[])
{
    print("Hello World!\n");


    return 0;
}