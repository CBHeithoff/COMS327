#include <iostream>


using namespace std;

int count_calls()
{
    // Me: num_calls will maintain its value across different calls to count_calls()
    // Prof: Static "local variables" have local shape, but global lifetime
    // Initialization doesn't actualy happen here, and the variable is not on the stack.
    // The data is in the "data segment", which is initialized by the runtime before the start of main.
    static int num_calls = 0;
    // this initialization '=0' is unnecessary because of placement (auto =0)

    return ++num_calls;
}

// scope: when can you see it
// lifetime: when it exists


int *return_local_address()
{
    // lifetime issue returning local variable i
    //int i;

    // fixes lifetime issue
    // Prof: Returning the addresss of a local varibale is an error (generates a warning)
    // because after the stack is popped the data no longer exists (in a logical sense).
    // But static variables are not local. Returning the address of a static is just fine.
    static int i;

    return &i;
}


// Me: static makes this function exist only in this file.
// static applied to a function tells the compiler that the function should get static linkage,
// that is, it should be callable only from within the file where it is deinfed.
// This is why the compiler complains if you declare a function static and don't call it; If you 
// don't call it here, and you can't call it elsewhere, then why does it even exist?
static void static_func()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void call_static()
{
    static_func();
}