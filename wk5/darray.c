#include <stdlib.h>
#include <string.h>

#include "darray.h"

// # means pre processor something
#define DARRAY_DEFAULT_CAPACITY 10

struct darray {
    char *a;
    unsigned capacity;
    unsigned size;
    unsigned element_size;
};

static int darray_resize(darray *d)
{
    char *tmp;

    if(!(tmp = realloc(d->a, d->capacity * 2 * d->element_size))){
        return -1;
    }

    d->a = tmp;
    d->capacity *= 2;

    return 0;
}

int darray_init(darray **d, unsigned element_size)
{
    if(!(*d = malloc(sizeof (**d))) || 
        !((*d)->a = malloc(element_size * DARRAY_DEFAULT_CAPACITY))){
        return -1;
    }

    (*d)->capacity = DARRAY_DEFAULT_CAPACITY;
    (*d)->size = 0;
    (*d)->element_size = element_size;

    return 0;
}

int darray_destroy(darray *d)
{
    free(d->a);
    free(d);

    return 0;
}

int darray_append(darray *d, void *data)
{
    if(d->capacity == d->size && darray_resize(d)){
        return -1;
    }

    // the right idea, but wrong
    //d->a[d->size] = *data;
    memcpy(d->a + (d->size * d->element_size), data, d->element_size);
    d->size++;

    return 0;
}

int darray_at(darray *d, unsigned index, void *v)
{
    if(index >= d->size){
        return -1;
    }

    memcpy(v, d->a + (index * d->element_size), d->element_size);

    return 0;
}

int darray_pop(darray *d, void *v)
{
    if(!d->size){
        return -1;
    }

    d->size--;
    memcpy(v, d->a + (d->size * d->element_size), d->element_size);

    return 0;
}

int darray_remove(darray *d, unsigned i, void *v)
{
    if(!d->size || i >= d->size){
        return -1;
    }

    memcpy(v, d->a + (i * d->element_size), d->element_size);

    memmove(d->a + (i * d->element_size),       // memmove because memcpy may not be used when
           d->a + ((i + 1) * d->element_size),  // src and dst overlap
           d->element_size * (d->size - i - 1));

    d->size--;

    return 0;
}

int darray_size(darray *d)
{
    return d->size;
}
