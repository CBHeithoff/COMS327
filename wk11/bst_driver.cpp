# include <iostream>

#include "bst.h"

using namespace std;

class foo{
    public:
        bool operator<(const foo &f) { return false;}
        bool operator!=(const foo &f) { return true;}
};

int main(int argc, char *argv[])
{
    bst<int> ibst;

    ibst.insert(6);
    ibst.insert(3);
    ibst.insert(4);
    ibst.insert(8);
    ibst.insert(9);
    ibst.insert(7);
    ibst.insert(5);
    ibst.insert(1);

    cout << ibst.contains(1) << endl;
    cout << ibst.contains(9) << endl;
    cout << ibst.contains(2) << endl;
    

    bst<string> sbst;

    sbst.insert("rob");
    sbst.insert("susie");
    sbst.insert("alex");
    sbst.insert("chet");
    sbst.insert("steve");
    sbst.insert("michelle");
    sbst.insert("jeremy");
    sbst.insert("kristie");

    cout << sbst.contains("alex") << endl;
    cout << sbst.contains("susie") << endl;
    cout << sbst.contains("flint") << endl;

    bst<foo> fbst;

    fbst.insert(foo());
    fbst.insert(foo());
    fbst.insert(foo());
    fbst.insert(foo());
    fbst.insert(foo());

    cout << fbst.contains(foo()) << endl;
    

    return 0;
}