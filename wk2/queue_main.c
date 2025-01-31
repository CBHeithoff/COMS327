#include <stdio.h>
#include "queue.h"

//gcc queue.c queue_main.c -Wall -Werror -o queue_main


int main(int argc, char *argv[])
{
    struct queue q;
    int i, j;

    queue_init(&q); // Should check return value

    for(i = 0; i < 10; i++){
        queue_enqueue(&q, i); // Here, too!
    }

    queue_front(&q, &j);

    printf("length: %d, is empty: %d, front: %d\n",queue_length(&q), queue_is_empty(&q), j);

    while (!queue_dequeue(&q, &j)){
        printf("%d\n", j);
    }

    printf("length: %d, is empty: %d\n",queue_length(&q), queue_is_empty(&q));

    queue_destroy(&q);

    return 0;
}