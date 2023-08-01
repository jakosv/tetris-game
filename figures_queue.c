#include <stdlib.h>

#include "figures_queue.h"
#include "figure_shape.h"


void fqueue_init(fqueue *queue)
{
    queue->first = NULL;
    queue->last = NULL;
}

int fqueue_is_empty(const fqueue *queue)
{
    return queue->first == NULL;
}

void fqueue_pop(figure_shape **shape, fqueue *queue)
{
    struct fqueue_item *tmp;
    tmp = queue->first;
    *shape = tmp->data;
    queue->first = tmp->next;
    free(tmp);
}

void fqueue_destroy(fqueue *queue)
{
    figure_shape *shape;
    while (!fqueue_is_empty(queue)) {
        fqueue_pop(&shape, queue); 
        free(shape);
    }
}

void fqueue_add(figure_shape *shape, fqueue *queue)
{
    struct fqueue_item *tmp;
    tmp = malloc(sizeof(*tmp));
    tmp->data = shape;
    tmp->next = NULL;
    if (queue->last)
        queue->last->next = tmp;
    else
        queue->first = tmp;
    queue->last = tmp;
}
