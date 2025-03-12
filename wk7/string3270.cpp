
#include "string3270.h"

 
string3270::string3270()
{
    // if make NULL, then all other method need to check for NULL, not great
    // always malloc, so no special cases

    // str = malloc(1);
    // str[0] = '\0';

    // one line implemenation
    str = strdup("");

}

string3270::string3270(const char *s)
{
    str = strdup(s);
}

string3270::string3270(const string3270 &s)
{
    str = strdup(s.str);
}

string3270::~string3270()
{
    //since it is always malloced, just free()
    free(str);
}

bool string3270::operator==(const string3270 &s)
{
    // "this" is a pointer if there was name shadowing
    return strcmp(str, s.str) == 0;
}

bool string3270::operator!=(const string3270 &s)
{
    return strcmp(str, s.str) != 0;
}

bool string3270::operator>=(const string3270 &s)
{
    return strcmp(str, s.str) >= 0;
}

bool string3270::operator<=(const string3270 &s)
{
    return strcmp(str, s.str) <= 0;
}

bool string3270::operator>(const string3270 &s)
{
    return strcmp(str, s.str) > 0;
}

bool string3270::operator<(const string3270 &s)
{
    return strcmp(str, s.str) < 0;
}

string3270 &string3270::operator=(const string3270 &s)
{
    free(str);
    str = strdup(s.str);

    // compiler handles the creation of the reference
    return *this;
}

string3270 &string3270::operator=(const char *s)
{
    free(str);
    str = strdup(s);

    return *this;
}

string3270 &string3270::operator+=(const string3270 &s)
{
    char *tmp
    
    // copies str into space allocated until all copied over or runs out of space
    if (!(tmp = realloc(str, strlen(str) + strlen(s.str) + 1))) {
        // never exit in a library. Use exception handling.
        exit(-1);
    }
    
    str = tmp;

    strcat(str, s.str);

    return *this;
}

string3270 &string3270::operator+=(const char *s)
{
    char *tmp
    
    // copies str into space allocated until all copied over or runs out of space
    if (!(tmp = realloc(str, strlen(str) + strlen(s) + 1))) {
        // never exit in a library. Use exception handling.
        exit(-1);
    }
    
    str = tmp;

    strcat(str, s);

    return *this;
}

string3270 string3270::operator+(const string3270 &s)
{
    // costly function calls
    // string3270 tmp(str);
    // tmp += s;

    string3270 tmp; // using default constructor

    free(tmp.str)
    tmp.str = malloc(strlen(str) + strlen(s.str) + 1);
    strcpy(stpcpy(tmp.str, str), s.str);

    return tmp;
    
}

string3270 string3270::operator+(const char *s)
{
    string3270 tmp; // using default constructor

    free(tmp.str)
    tmp.str = malloc(strlen(str) + strlen(s) + 1);
    strcpy(stpcpy(tmp.str, str), s);

    return tmp;
}

int string3270::length()
{
    return strlen(str);
}

char &string3270::operator[](int i)
{
    return str[i]; // needs exception handling
}

const char *string3270::c_str()
{
    return str;
}


istream &operator>>(istream &i const string3270 &s)
{

}

ostream &operator<<(ostream &o const string3270 &s)
{
    //return o << s.str // cannot do, s.str is private
    
    return o << s.c_str();

    // no new line needed, that is responsibility of the user
}

