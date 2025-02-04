#include <stdlib.h>
#include "queue.h" // do a relative search in the local directory

//int queue_init(struct queue *q)
int queue_init(queue *q)
{
    
    //q->front = q->back = (void *) 1; // causes crash
    q->front = q->back = NULL;
    q->length = 0;

    return 0;
}

int queue_destroy(struct queue *q)
{
    /*
    int value;

    while(!queue_dequeue(q, &value)); // function calls are expensive

    return 0;
    */

    struct queue_node *tmp;

    while ((tmp = q->front)){
        q->front = q->front->next;
        free(tmp);
    }

    q->back = NULL;
    q->length = 0;

    return 0;
}

int queue_enqueue(struct queue *q, int value)
{
    struct queue_node *tmp;

    if(!(tmp = malloc(sizeof (*tmp)))){
        return -1;
    }

    tmp-> data = value;

    if (!q->front){
        q->front = q->back = tmp;
    } else{
        q->back->next = tmp;
        q->back = tmp;
    }

    tmp->next = NULL;

    q->length++;

    return 0;
}

int queue_dequeue(struct queue *q, int *value)
{
    struct queue_node *tmp;

    if(!q->front){
        return -1;
    }

    *value = q->front->data;

    tmp = q->front;
    q->front = q->front->next;
    free(tmp);

    if(!q->front){
        q->back = NULL;
    }

    q->length--;

    return 0;
}

int queue_front(struct queue *q, int *value)
{
    if(!q->front){
        return -1;
    }

    *value = q->front->data;

    return 0;
}

int queue_length(struct queue *q)
{
    return q->length;
}

int queue_is_empty(struct queue *q)
{
    return !q->front;
}
