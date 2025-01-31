#ifndef QUEUE_H
#define QUEUE_H

struct queue_node {
    struct queue_node *next;
    int data;
};

struct queue {
    struct queue_node *front, *back;
    int length;
};

int queue_init(struct queue *q);
int queue_destroy(struct queue *q);
int queue_enqueue(struct queue *q, int value);
int queue_dequeue(struct queue *q, int *value);
int queue_front(struct queue *q, int *value);
int queue_length(struct queue *q);
int queue_is_empty(struct queue *q);

#endif
