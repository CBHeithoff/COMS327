#include <iostream>

#include "cpp_function.h"

using namespace std;

void print(const char *s)
{
    cout << s << endl;
}

ostream *get_cout()
{
    return &cout;
}

void print_with_cout(ostream *o, const char *s)
{
    *o << s;
}