#ifndef SHAPES_QUEUE_H_SENTRY
#define SHAPES_QUEUE_H_SENTRY

#include "shape.h"

struct sq_item {
    shape_t *data; 
    struct sq_item *next;
};
typedef struct sq_item sq_item_t;

struct shapes_queue {
    sq_item_t *first, *last;
};
typedef struct shapes_queue shapes_queue_t;

void sq_init(shapes_queue_t *q);
void sq_destroy(shapes_queue_t *q);
int sq_is_empty(const shapes_queue_t *q);
void sq_add(shape_t *shape, shapes_queue_t *q);
void sq_pop(shape_t **shape, shapes_queue_t *q);

#endif
