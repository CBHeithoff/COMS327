// gcc -E ___.c
// to run pre-compiler

// pre-compilers make #include libraries work and macros
// #include <stdio.h>

// value-type macros
#define DARRAY_DEFAULT_CAPACITY 10

// function-type macros
#define min(x,y) (x < y ? x : y)

// curly braces enclose "basic block". Basic blocks contain a scope.
// Variables declared within a basic block go out of scope at the end of the block.
// Basic blocks have a value. Their value is the value of their last line.
// However, basic blocks are not expressions!
// The tirck is to weap the BB in parenthesis, which transforms it into an expression.
#define max(x, y) ({     \
    typeof (x) _x = (x); \
    typeof (y) _y = (y); \
    _x > _y ? _x : _y;   \ 
})
// macros need to be a single line. Use "\" to logically make it one line.

// only works in macros
// "#" takes literal input as string
#define stringify(x) #x

// "##" concatenates stuff to strings.
#define concatenate(x) cmd_ ## x

// when user types foo/baz, run foo()/baz()
// dont want to compare so my strings in linear time
int cmd_foo(int);
int cmd_bar(int);
int cmd_baz(int);
// ...
int cmd_zip(int);

// couldn't put functions in array, but can put pointers to functions in array
typedef int (*fptr)(int);

// or sort the functions, put in array, and binary search array in logn time
struct lu_entry {
    const char *name;
    fptr func;
};

#define lu_value(name) { #name, cmd_ ## name }

struct lu_entry table[] = {
    // what if typo, difficult debugging
    lu_value(bar)
    lu_value(baz)
    //{ "bar", bar },
    //{ "baz", baz },
    lu_value(foo)
    //{ "foo", foo },
    // ...
    lu_value(zip)
    //{ "zip", zip },
}


// wk6
//#define LOG(format, ...) fprintf(log, format, __VA_ARGS__)
#define LOG(...) fprintf(log, format, __VA_ARGS__)

// gcc has "0b", not C
// int b = 0b<32-bit number>

/* Hexdecimal: Hexdecimal is base 16. It's binary for people.
 * 
 *    base 10            base 16             base 2
 *    0                  0                   0000
 *    1                  1                   0001
 *    2                  2                   0010
 *    3                  3                   0011
 *    4                  4                   0100
 *    5                  5                   0101
 *    6                  6                   0110
 *    7                  7                   0111
 *    8                  8                   1000
 *    9                  9                   1001
 *    10                 A                   1010
 *    11                 B                   1011
 *    12                 C                   1100
 *    13                 D                   1101
 *    14                 E                   1110 
 *    15                 F                   1111 
 *  
 * 27 decimal = 00011011 binary = 1B hex
 * 
 */

 /* Bitwise operators
  * 
  * Bitwise and &:  011010
  *               & 111000
  *                 ------
  *                 011000
  * 
  * Bitwise or |:   011010
  *               | 111000
  *                 ------
  *                 111010
  * 
  * Bitwise xor ^:  011010
  *               ^ 111000
  *                 ------
  *                 100010
  * 
  * Bitwise compliment (unary) ~:  ~011010 = 100101
  * 
  * 
  */

#define ATTRIBUTE_SMART         0x00000001
#define ATTRIBUTE_TUNNEL        0x00000002
#define ATTRIBUTE_ERRATIC       0x00000004
#define ATTRIBUTE_TELEPATHIC    0x00000008
#define ATTRIBUTE_MAGIC         0x00000010
#define ATTRIBUTE_BREATH        0x00000020

struct monster {
    int attrubutes;
    
} m;

if (m.attributes & (ATTRUBUTE_TELEPATHIC | ATTRIBUTE_BREATH)){
    // Monster is telepathic or can breathe fire
}

// Monster is checks if monster m has attribute attr
#define M_IS(m, attr) (m->attributes & ATTRIBUTE_ ## attr)

// Can this monster tunnel through walls?
M_IS(m, tunnel)




int main(int argc, char *argv[])
{
    // pre-compiler replaces macros wuth literal, in this case 10.
    printf("%d\n", DARRAY_DEFAULT_CAPACITY);

    min(a, b);
    // function_with_side_effects() prints, writes to disk, writes to network
    // very_expensive_function() takes two weeks
    min (function_with_side_effects(), very_expensive_function());
    max (function_with_side_effects(), very_expensive_function());

    stringify(foo);

    bsearch(key, table, number of elements, size of element, comparitor)->func(0);

    concatenate(foo);



    // wk6
    LOG("Error in %s\n", file, f, g, h);

    return 0;
}