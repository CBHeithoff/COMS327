#ifndef DARRAY_H
#define DARRAY_H

// # means pre processor something
#define DEFAULT_SIZE 10

typedef struct {
    void *a;
    int capacity;
    int size;
    int element_size;

} darray;

int darray_init(darray *d, int element_size);
int darray_destroy(darray *d);
int darray_append(darray *d, void *data);
int darray_at(darray *d, int index, void *v);
int darray_pop(darray *d, void *v);


#endif