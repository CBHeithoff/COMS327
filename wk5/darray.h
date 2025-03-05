#ifndef DARRAY_H
#define DARRAY_H

struct darray; // forward reference - tells the compiler that there exists a 
               // struct darray, which will be fully defined

typedef struct darray darray;

// after taking struct definition out of darray.h
//int darray_init(darray *d, unsigned element_size);
int darray_init(darray **d, unsigned element_size);
int darray_destroy(darray *d);
int darray_append(darray *d, void *data);
int darray_at(darray *d, unsigned index, void *v);
int darray_pop(darray *d, void *v);
int darray_remove(darray *d, unsigned i, void *v); // Remove element at index i; shift remaining 
int darray_size(darray *d);                        // elments one position toward front of array



#endif