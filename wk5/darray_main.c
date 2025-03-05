#include <stdio.h>

#include "darray.h"

int main(int argc, char *argv[])
{
    darray *d = NULL;
    int i;

    //printf("%d\n", d->size); user cannot look inside data structure

    darray_init(&d, sizeof (int));

    //darray_resize(d);

    for(i = 0; i < 1000; i++){
        darray_append(d, &i);
    }

    darray_at(d, 10, &i);
    printf("%d\n", i);
    darray_at(d, 500, &i);
    printf("%d\n", i);
    darray_at(d, 999, &i);
    printf("%d\n", i);

    darray_remove(d, 500, &i);

    darray_at(d, 500, &i);
    printf("%d\n", i);
    darray_at(d, 998, &i);
    printf("%d\n", i);

    darray_destroy(d);

    return 0;
}
