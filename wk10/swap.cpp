#include <iostream>

using namespace std;

void swap_c(char *x, char *y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

void swap_cpp(char &x, char &y)
{
    int tmp;

    tmp = x;
    x = y;
    y = tmp;
}

// function overloading
void swap_cpp(double &x, double &y)
{
    double tmp;

    tmp = x;
    x = y;
    y = tmp;
}

// GENERICS
// or template<typename T>
template<class T>
void swap3270(T &x, T &y)
{
    T tmp;

    tmp = x;
    x = y;
    y = tmp;
}
// don't generate code itself, when called in main, it looks at this for how to generate code.
// normally in header



// Java
// ArrayList<Int> a;

// a.add(1);
// a.add(f); // compiler complains that f is not an int

// type erasure, here <Int> is statically checked, then removed so every thing is objects

int main(int argc, char *argv[])
{
    int i, j;

    i = 0;
    j = 1;

    cout << i << " " << j << endl;
    swap3270(i, j);
    cout << i << " " << j << endl;

    double  x, y;
    x = 1.5;
    y = 2.5;
    
    cout << x << " " << y << endl;
    swap3270(x, y);
    cout << x << " " << y << endl;

    string  s, t;
    s = "foo";
    t = "bar";
    
    cout << s << " " << t << endl;
    swap3270(s, t);
    cout << s << " " << t << endl;
    
    return 0;
}