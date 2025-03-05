#include <iostream>

void swap_c(int *x, int *y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

// this is invalid in C
// & is reference in C++ or pointers under the cover
// no new functionality with references, designers just thought it was better than C pointer design
void swap_cpp(int &x, int &y)
{
    int tmp;

    tmp = x;
    x = y;
    y = tmp;
}

using namespace std; // take everything in std namespace and put it in current namespace
// adding 1000s of names to namespace, namespace contamiation, not good coding, but saves time in class
// use std:: when needed

int main(int argc, char *argv[])
{
    int i, j;
    // reference is pointer without pointer syntax
    // references have to be initialized when created
    // cannot be made to point somewhere else
    int &r = i;

    i = 0;
    j = 1;

    cout << i << " " << j << endl; // 0 1

    swap_c(&i, &j);

    cout << i << " " << j << endl; // 1 0

    // pass in varaible and compiler will create the reference (only for variables, not #s)
    swap_cpp(i, j);

    cout << i << " " << j << endl; // 0 1

    return 0;
}