#include <stdlib.h>

#include "shapes_queue.h"
#include "shape.h"


void sq_init(shapes_queue_t *q)
{
    q->first = NULL;
    q->last = NULL;
}

int sq_is_empty(const shapes_queue_t *q)
{
    return q->first == NULL;
}

void sq_pop(shape_t **shape, shapes_queue_t *q)
{
    sq_item_t *tmp;
    tmp = q->first;
    *shape = tmp->data;
    q->first = tmp->next;
    free(tmp);
}

void sq_destroy(shapes_queue_t *q)
{
    shape_t *shape;
    while (!sq_is_empty(q)) {
        sq_pop(&shape, q); 
        free(shape);
    }
}

void sq_add(shape_t *shape, shapes_queue_t *q)
{
    sq_item_t *tmp;
    tmp = malloc(sizeof(*tmp));
    tmp->data = shape;
    tmp->next = NULL;
    if (q->last)
        q->last->next = tmp;
    else
        q->first = tmp;
    q->last = tmp;
}

#ifdef FQ_TEST

#include <stdio.h>
#include <stdlib.h>

int main()
{
    shape_t *shape;
    shapes_queue_t q;
    int i, cnt;
    sq_init(&q);
    cnt = 5;
    for (i = 0; i < cnt; i++) {
        shape = get_random_shape();
        printf("Add: %d\n", shape->type);
        sq_add(shape, &q);
    }
    if (sq_is_empty(&q))
        puts("Empty");
    else
        puts("Not empty");
    while (!sq_is_empty(&q)) {
        sq_pop(&shape, &q);
        printf("Pop: %d\n", shape->type);
        free(shape);
    }
    sq_destroy(&q);
    return 0;
    
    return 0;
}

#endif
