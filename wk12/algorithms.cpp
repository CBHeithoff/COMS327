#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

void print_int(int i)
{
    cout << i << endl;
}

void double_int(int &i)
{
    i *= 2;
}

bool is_prime(int i)
{
    int j;

    // Please forgive this bullshit
    if(i < 2 || i == 4) return false;

    for (j = 2; j < i / 2; j++){
        if(i % j == 0){
            return false;
        }
    }

    return true;
}

bool is17(int i)
{
    return i == 17;
}

bool is125(int i)
{
    return i == 125;
}

class value_predicate {
    private:
        int value;
    public:
        value_predicate(int i) : value(i) {}
        bool operator()(int i)
        {
            return value == i;
        }
};

int main(int argc, char *argv[])
{
    vector<int> v;
    int i;
    vector<int>::iterator vi;

    for(i = 0; i < 100; i++){
        v.push_back(i);
    }

    for_each(v.begin(), v.end(), print_int);

    /**
    for_each(v.begin(), v.end(), double_int);
    for_each(v.begin(), v.end(), print_int);
    */

    for(vi = v.begin(); vi != v.end(); vi++){
        vi = find_if(vi, v.end(), is_prime);
        if(vi == v.end()) break;
        cout << *vi << endl;
    }

    cout << count_if(v.begin(), v.end(), is_prime) << endl;

    cout << count_if(v.begin(), v.end(), is17) << endl;
    cout << count_if(v.begin(), v.end(), is125) << endl;

    value_predicate vp24(24);
    cout << count_if(v.begin(), v.end(), vp24) << endl;
    cout << count_if(v.begin(), v.end(), value_predicate(89)) << endl;

    return 0;
}