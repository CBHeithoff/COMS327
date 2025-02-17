#include <stdio.h>

/*
    There are two common ways you'll create statically allocated strings:

    char a[] = "Foo"; // a string array; compiler copies literal into array. Array is mutable.
    char *p = "Bar"; // a pointer to a string literal. The pointer can be pointed elsewhere (it is mutable), but the literal is immutable!

    All strings in C are terminated by a NULL byte ('\0'). When using a string literal, 
    the compiler automatically adds this byte for us at the end of the string.

    You can have a char array that is not a string. If there's not terminiating NULL, it is not a string, 
    and it's an error to use string functions on it.
*/

int strcmp3270(const char *s1, const char *s2)
{
    int i;

    for(i = 0; s1[i] && s1[i] == s2[i]; i++) {}

    return s1[i] - s2[i];
}

int strcmp3270_idiomatic(const char *s1, const char *s2)
{
    while(*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return *s1 - *s2;
}

int strlen3270(const char *s)
{
    int i;

    for (i = 0; s[i]; i++) {}

    return i;
}

char *strcpy3270(char *dst, const char *src)
{
    int i;

    for(i = 0; (dst[i] = src[i]); i++) {}

    return dst;
}

int main(int argc, char *argv[])
{
    char a[] = "Hello";
    char b[] = "Goodbye";
    char *c = "Hello";

    printf("%d\t%d\n", strcmp3270_idiomatic(a, b), strcmp3270(a, c));
    b[0] = 'H';
    printf("%d\t%d\n", strcmp3270(a, b), strcmp3270(a, c));
    // c[0] = 'J';
    // printf("%d\t%d\n", strcmp3270(a, b), strcmp3270(a, c));

    strcpy3270(b, a);
    printf("%s\n", b);
    printf("%s\n", b + strlen3270(b) + 1); // Valid and well defined // b early part was changed, but the 'e' from goodbye is still there 

    return 0;
}