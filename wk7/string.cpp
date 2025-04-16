#include "string3270.h"

using namespace std;

int main(int argc, char *argv[])
{
    // ------------------------------------------------------------ pre week 10
    
    // string3270 s = "Hello World!";
    // string3270 t;

    // t = "World!";

    // //cout << s << endl;

    // s += string3270(" ") + t;

    // cout << s << endl;

    // cout << s[6] << endl;

    // s[7] = 'a';
    // s[7] = 'l';
    // s[7] = 't';
    // s[7] = '\0';

    // cout << s << endl;


    // //cin >> t;

    // if(s.operator==(t)){
    // //if(s == t){
    //     cout << "Equal" << endl;
    // }else{
    //     cout << "Not equal" << endl;
    // }

    // operator<<(cout, s) << endl;

    // ------------------------------------------------------------ week 10

    string3270 s = "Hello World!";

    cout << s << endl;

    s[6] = 'E';
    s[7] = 'a';
    s[8] = 'r';
    s[9] = 'l';
    s[10] = '!';

    cout << s << endl;

    if{
        s[11] = 'f';
    }
    catch (int i){
        cout << i << endl;
    }
    catch (const char *s){
        cout << s << endl;
    }
    // Elipsis must be last. Catches any type. But, since you don't know 
    // what it is, you can't examine it.
    catch (...){
        cout << "Foo!" << endl;
    }

    cout << s << endl;

    return 0;
}