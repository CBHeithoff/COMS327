#include <stdio.h>
#include <stdarg.h>

// int printf(const char *restrict format, ...)
// the "..." allows lots of inputs, we can do that too.
// printf parses inputs and calls int vprintf(const char *restrict format, va_list ap)
// There are va_start/_arg/_end/_copy which are macros.
// type va_arg(va_list ap, type); telling it what type it is

// This is an extremely watered-down printf. It prints ints, floats, chars, and strings.
// We will not expect % before conversion specifiers.
// We will not print the contents of format strings.
// We will not support modifiers to conversions.
void printf3270(const char *format, ...)
{
    va_list ap;

    va_start(ap, format); // format is the last input before the "..." (called eclipse?) 

    while (*format) {
        switch (*format) {
        case 'd':
            //printf("%-+3.8d\t", va_arg(ap, int)); // print right end, the sign, three wide, eight levels of precise
            //printf("%*d\t", va_arg(ap, int)); // does something with pointer and input
            printf("%d\t", va_arg(ap, int));
            break;
        case 's':
            printf("'%s'\t", va_arg(ap, char *));
            break;
        case 'c':
            printf("%c\t", va_arg(ap, int)); // must use int so va_arg reads 4 bytes
            break;
        case 'f':
            printf("%f\t", va_arg(ap, double)); // must use double so va_arg reads 8 bytes
            break;
        }
        format++;
    }
    printf("\n");

    va_end(ap);
}

int main(int argc, char *argv[])
{
    printf3270("dscf", 189, "Hello World!", 't', 1.616);


    return 0;
}