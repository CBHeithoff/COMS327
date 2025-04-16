#include <iostream>

using namespace std;

// Me: only want one instance of the a class
// A singleton is a class which is restricted to at most one instance
// using langauge semantics and the compiler to enforce that restriction
class singleton {
    private:
        static singleton *the_instance;
        // don't allow user to call constructor
        singleton() {}
        singleton(const singleton &s) {}
    public:
        static singleton *get_instance()
        {
            if(!the_instance){
                the_instance = new singleton();
            }
            return the_instance;
        }

};

singleton *singleton::the_instance;

int main(int garc, char *argv[])
{
    // cannot make an instance of a singleton
    //singleton a;

    
    // all prints print the same address, meaning there is only one instance of singleton class
    singleton *a;
    a = singleton::get_instance();
    cout << a << endl;

    singleton *b;
    b = singleton::get_instance();
    cout << b << endl;

    cout << singleton::get_instance() << endl;

    singleton c = *a;
    // same statment, calling the copy constructor
    //singleton c(*a);

    cout << &c << endl;


    return 0;
}