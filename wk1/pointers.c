#include <stdio.h> 
#include <time.h>
#include <stdlib.h>

struct foo{
    int i;
    float f;
};

// f is a copy (pass by value) of the struct passed...
// void foo_init(struct foo f, int i, float g)
// need to pass by address
void foo_init(struct foo *f, int i, float g)
{
    f->i = i;
    f->f = g;

    printf("f.i: %d, f.f: %f\n", f->i, f->f);
}

// Incorrect - swaps the addresses in the pointers!
// We say that C has two parameter passing modes: pass by value and 
// pass by address, but it actually only has pass by value!
/*
void swap(int *i, int *j)
{
    int *tmp;

    tmp = i;
    i = j;
    j = tmp;

    printf("%d %d\n", *i, *j);
}
*/

void swap(int *i, int *j)
{
    int tmp;

    // Unary * is the dereference operator. It gives the contents
    // of the memory addressed by the pointer to which it is applied.
    // * and & are opposites.
    tmp = *i;
    *i = *j;
    *j = tmp;

    printf("%d %d\n", *i, *j);
}

int main (int argc, char *argv[]) 
{
    struct foo f;
    int x, y;
    int *p;
    int i;

    x = 0;
    y = 1;

    p = &x;

    swap(&x, &y);
    printf("%d %d\n", x, y);

    printf("%d\n", *p); // p is pointing at x. *p is the contents of x.
                        // *p and x are aliases

    p = malloc(sizeof (*p)); // sizeof is a static operator

    printf("%d\n", *p); // Error: *p hasn't been initialized

    *p = 5;
    printf("%d\n", *p);

    free(p);
    printf("%d\n", *p); // Error: The data addressed by p no longer belongs to 
                        // this program. Behavior is undefined.

    p = malloc(10 * sizeof (*p)); // able to dynamic allocate x number of things

    for ( i = 0; i < 10; i++){
        p[i] = i; // Array syntax can be applied to pointers.
    }
    for ( i = 0; i < 10; i++){
        printf("%d\n", p[i]); // *p prints ten 0s
                            // *(p + i) prints 0,1,2,..,9
                            // "pointer arthmetic": adding an indev to a pointer give the 
                            // address of the index's element
    }

    free(p);



    // to get a pointer from an instance, take the  in...
    // using the address operator, &
    foo_init(&f, 42, 3.1415926);

    printf("f.i: %d, f.f: %f\n", f.i, f.f);
}