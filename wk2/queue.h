#ifndef QUEUE_H
#define QUEUE_H

// typedef struct queue_node {
       // cannot use alias before it is defined, one-pass compiler
//     queue_node *next;
//     int data;
// } queue_node;

struct queue_node {
    struct queue_node *next;
    int data;
};

// typedef extant_type new_name;
typedef struct queue_node queue_node;

typedef struct queue {
    // struct queue_node *front, *back;
    queue_node *front, *back;
    int length;
} queue;

// int queue_init(struct queue *q);
int queue_init(queue *q);
int queue_destroy(struct queue *q);
int queue_enqueue(struct queue *q, int value);
int queue_dequeue(struct queue *q, int *value);
int queue_front(struct queue *q, int *value);
int queue_length(struct queue *q);
int queue_is_empty(struct queue *q);

#endif
