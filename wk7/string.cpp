#include "string3270.h"

using namespace std;

int main(int argc, char *argv[])
{
    string3270 s = "Hello World!";
    string3270 t;

    t = "World!";

    //cout << s << endl;

    s += string3270(" ") + t;

    cout << s << endl;

    cout << s[6] << endl;

    s[7] = 'a';
    s[7] = 'l';
    s[7] = 't';
    s[7] = '\0';

    cout << s << endl;


    //cin >> t;

    if(s.operator==(t)){
    //if(s == t){
        cout << "Equal" << endl;
    }else{
        cout << "Not equal" << endl;
    }

    operator<<(cout, s) << endl;

    return 0;
}