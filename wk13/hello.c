#include <stdio.h>

// gcc hello.c -o hello

// returns libraries needed for library/executable in question: ldd hello

// To make a static binary
// gcc hello.c -o hello -static
// make it static so it doesn't depend on other libraries
// takes and copies in everything needed from libraries
// binary file becomes larger of course

// To see system calls
// strace ./hello

// To see binary file size
// ls -l hello

// removes metadata and information not needed for executable (debugging symbols, function names, etc.)
// strip hello


// STATIC LIBRARY
// ar -cvq libheap.a heap.o
// ar -t libheap.a
// gcc user_heap.c -lheap -o use_heap
// gcc use_heap.c -L. -lheap -o user_heap
// ./use_heap

// DYNAMIC LIBRARY
// gcc heap.c -fPIC -c
// gcc -shared -Wl,-soname,libheap.so -o libheap.so heap.o
// gcc use_heap.c -L. -lheap -o use_heap
// LD_LIBRARY_PATH=$LIBRARY_PATH:. ./use_heap



int main (int argc, char *argv[])
{
    printf("Hello World!");

    return 0;
}