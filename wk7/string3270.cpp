
#include "string3270.h"

// std::exception class in <excpetion>
// exception exports an abstract method const char *what()
// If you extend exception, you must implement what()
// Uncaught excepts are caught by the runtime after the stack unwinds past main.
// The runtime prints the string returned by what().
//
// In C++ *anything* can be used as an exception! Catch by type.
//
// In C, the compiler defines a macro every time it compiles a 
// function: __FUNCTION__ . This macro contains the function name as a const char *
// C++ also implements __FUNCTION__; however, function overloading makes this 
// potentially ambiguious. So C++ introduces ++PRETTY_FUNCTION__ , which 
// contains the entire function signature.
// 
 
string3270::string3270()
{
    // if make NULL, then all other method need to check for NULL, not great
    // always malloc, so no special cases

    // str = malloc(1);
    // str[0] = '\0';

    // one line implemenation
    // str = strdup("");

    if(!(str = strdup(""))){
        throw __PRETTY_FUNCTION__;
    }

}

string3270::string3270(const char *s)
{
    // str = strdup(s);

    if(!(str = strdup(s))){
        throw __PRETTY_FUNCTION__;
    }
}

string3270::string3270(const string3270 &s)
{
    // str = strdup(s.str);

    if(!(str = strdup(s.str))){
        throw __PRETTY_FUNCTION__;
    }
}

string3270::~string3270()
{
    //since it is always malloced, just free()
    free(str);
}

bool string3270::operator==(const string3270 &s) const
{
    // "this" is a pointer if there was name shadowing
    return strcmp(str, s.str) == 0;
}

bool string3270::operator!=(const string3270 &s) const
{
    return strcmp(str, s.str) != 0;
}

bool string3270::operator>=(const string3270 &s) const
{
    return strcmp(str, s.str) >= 0;
}

bool string3270::operator<=(const string3270 &s) const
{
    return strcmp(str, s.str) <= 0;
}

bool string3270::operator>(const string3270 &s) const
{
    return strcmp(str, s.str) > 0;
}

bool string3270::operator<(const string3270 &s) const
{
    return strcmp(str, s.str) < 0;
}

string3270 &string3270::operator=(const string3270 &s)
{
    free(str);
    // str = strdup(s.str);
    if(!(str = strdup(s.str))){
        throw __PRETTY_FUNCTION__;
    }

    // compiler handles the creation of the reference
    return *this;
}

string3270 &string3270::operator=(const char *s)
{
    free(str);
    // str = strdup(s);
    if(!(str = strdup(s))){
        throw __PRETTY_FUNCTION__;
    }

    return *this;
}

string3270 &string3270::operator+=(const string3270 &s)
{
    char *tmp;
    
    // copies str into space allocated until all copied over or runs out of space
    if (!(tmp = (char *) realloc(str, strlen(str) + strlen(s.str) + 1))) {
        // never exit in a library. Use exception handling.
        //exit(-1);
        throw __PRETTY_FUNCTION__;
    }
    
    str = tmp;

    strcat(str, s.str);

    return *this;
}

string3270 &string3270::operator+=(const char *s)
{
    char *tmp;
    
    // copies str into space allocated until all copied over or runs out of space
    if (!(tmp = (char *) realloc(str, strlen(str) + strlen(s) + 1))) {
        // never exit in a library. Use exception handling.
        //exit(-1);
        throw __PRETTY_FUNCTION__;
    }
    
    str = tmp;

    strcat(str, s);

    return *this;
}

string3270 string3270::operator+(const string3270 &s) const
{
    // costly function calls
    // string3270 tmp(str);
    // tmp += s;

    string3270 tmp; // using default constructor

    free(tmp.str);
    if (!(tmp.str = (char *) malloc(strlen(str) + strlen(s.str) + 1))){
        throw __PRETTY_FUNCTION__;
    }
    strcpy(stpcpy(tmp.str, str), s.str);

    return tmp;
    
}

string3270 string3270::operator+(const char *s) const
{
    string3270 tmp; // using default constructor

    free(tmp.str);
    if (!(tmp.str = (char *) malloc(strlen(str) + strlen(s) + 1))){
        throw __PRETTY_FUNCTION__;
    }
    strcpy(stpcpy(tmp.str, str), s);

    return tmp;
}

int string3270::length() const
{
    return strlen(str);
}

/**
string3270 s = "foo";
cout << s << endl;
s[0] = 'b';         //needs char & so this compiles
cout << s << endl;  // needs char & so actuall string is modified
*/

// "throw" after const would mean its not going to throw an exception
// "noexcept" does the same as "throw"
// throw (const char *) to mean throwing a const char *
char &string3270::operator[](int i) const
{
    // >= to not index the null byte
    if(i >= strlen(str)){
        throw __PRETTY_FUNCTION__;
    }
    
    return str[i]; // needs exception handling
}

const char *string3270::c_str() const
{
    return str;
}

// this function is a bit more complicated than the others in order to fully and correctly
// implement it. In particular, it requires us to scan the input string byte-by-byte looking
// for a delimiter character. We're going to do it half-assed.
std::istream &operator>>(std::istream &i, const string3270 &s)
{
    free(s.str);
    if (!(s.str = (char *) malloc(80))){ // real way would scan to find the end
        throw __PRETTY_FUNCTION__;
    }

    return i.getline(s.str, 80, '\n');
}

/**
string3270c s = "foo";
cout << s << endl;      // need ostream & so outputs can be chained together
*/


std::ostream &operator<<(std::ostream &o, const string3270 &s)
{
    //return o << s.str // cannot do, s.str is private
    
    return o << s.c_str();

    // no new line needed, that is responsibility of the user
}

