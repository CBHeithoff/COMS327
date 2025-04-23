#include <stdlib.h>

#include "heap.h"

int main(int argc, char *argv[])
{
    heap_t h;

    heap_init(&h, NULL, NULL);

    heap_delete(&h);

    return 0;
}
