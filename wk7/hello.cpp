#include <stdio.h>
#include <iostream> // header for C++ printing 

// gcc hello.cpp -o hello
// g++ hello.cpp -o hello

// can overload a processor in C++

// Among other things, iostream instantiates cin, cout, and cerr.
// Analogous to stdin, stdout, and stderr from C, but they are instances of class istream and ostream.
// << is the ouput operator when applied to an ostream.
// endl prints a newline and forces a buffer flush.
// :: is the scope resolution operator - look inside scope and find symbol.

int main(int argc, char *argv[])
{
    //printf("Hello World!\n");

    // std is the standard namespace. C++ has namespaces
    // "::" is scope resolution operator
    std::cout << "Hello World!\n";
    std::cout << "Hello World!" << std::endl; // this will force a buffer flush on the newline


    return 0;
}