#ifndef STRING3270_H
#define STRING3270_H

// C
// typedef class string {
    // private int foo;
    // public int bar();
// } str;

// C++
class string3270 {
    // private by default
    // all following is private to class
    private:
        char *string
    
    // anyone can use it
    public: 
        // default constructor doesn't need return type (not a method technically)
        string3270(); 
        string3270(const char *);
        string3270(const string3270 &);
        // deconstructor because no garbage collector
        ~string3270();

        bool operator==(const string3270 &);
        bool operator!=(const string3270 &);
        bool operator>=(const string3270 &);
        bool operator<=(const string3270 &);
        bool operator>(const string3270 &);
        bool operator<(const string3270 &);

        string3270 &operator=(const string3270 &);
        string3270 &operator=(const char *);
        string3270 &operator+=(const string3270 &);
        string3270 &operator+=(const char *);
        string3270 operator+(const string3270 &);
        string3270 operator+(const char *);

        int length();
        // indexing
        char &operator[](int i);
        const char *c_str();
    
        // Friends have access to our privates
        friend istream &operator>>(istream & const string3270 &);

};

// needs to be outside of the class (don't have access to private elements)
ostream &operator<<(ostream & const string3270 &);

/*
    Operator overloading

    string3270 s, t, u;


    // reason for ostream and reference return type, "ostream &", for operator<<()
    cout << s << endl; // operator<<(ostream &, string3270 &).operator<<(endl);


    // reason for char and erference return type, "char &", for operator[](int i)
    cout << s[i] << endl;
    s[i] = 'f';


    bool operator==(const string3270 &)
    if (s == t) { // s.operator==(t)
        ...
    

    // reason for string and referance return type, "string3270 &", for operator=();
    s = (t = u); // s.operator=(t.operator=(u))
    (s = t).remove_all_es();
    (s += t).remove_all_es();


    // reason for string and no reference return type "string3270", for string3270 operator+();
    foo = (s + t);

*/

#endif