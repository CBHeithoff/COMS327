#include <iostream>

// g++ static.cpp -c
// g++ static_main.cpp -c
// g++ static.cpp static_main.cpp

using namespace std;

int count_calls();
void call_static();

class count_instances{
    private: 
        // make static so all instances share this variable 
        static int num_instances;
    public:
        count_instances()
        {
            num_instances++;
        }
        // with static it can be called without an instance of the class
        // Prof: static methods (not technically a method, since it doesn't appl
        // to an object!) can be called without an instance of the class with the 
        // syntax: class_name:: function_name().
        static int get_num_instances()
        {
            return num_instances;
        }
};

// like static in C, class variables in C++ are located in the data segment.
// Data in the data segment is initialized by the runtime to all bits 0.
int count_instances::num_instances;

int main(int argc, char *argv[])
{
    int i;
    for(i = 0; i < 10; i++){
        cout << count_calls() << endl;
    }

    call_static();

    cout << count_instances::get_num_instances() << endl;

    count_instances a;

    cout << a.get_num_instances() << endl;

    count_instances b,c,d;

    cout << a.get_num_instances() << endl;

    return 0;
}

