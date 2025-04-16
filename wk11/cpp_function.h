#ifndef CPP_FUNCTION_H
#define CPP_FUNCTION_H


// __cplusplus is deifnied by the C++ compiler. It only exists if we are compoiling C++ code in a C++ compiler.
// We use it here to ensure certain code is only viewed byt he C++ compiler
// and other code is only viewed by the C compiler.
# ifdef __cplusplus
// code in here is only viewed by the C++ compiler
#include <iostream>
using namespace std; // needs to go here so only cpp sees it, since it is invalid c elsewhere

extern "C" {
#else
    // code in here is only viewed by the C compiler.
    // C doesn;t know what an ostream is, and we can't tell it, since it doesn't understand classes.
    // SO we only have pointers to ostreams and typedef pstream to void.
    // thus, an ostream inC is a pointer to void.
    typedef void ostream; // only seen by c compiler, since we cannot let it stump on cpp ostream definition
# endif

// the compiler doesn't support function overloading, so any C interface must be free of overloading. 
// more generally, any C interface must be free od any C++ features that are not in C
void print(const char *s);

ostream *get_cout();
void print_with_cout(ostream *, const char *);

# ifdef __cplusplus
}
# endif



#endif