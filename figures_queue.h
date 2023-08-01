#ifndef FIGURES_QUEUE_H_SENTRY
#define FIGURES_QUEUE_H_SENTRY

#include "figure_shape.h"

struct fqueue_item {
    figure_shape *data; 
    struct fqueue_item *next;
};

struct fqueue {
    struct fqueue_item *first, *last;
};
typedef struct fqueue fqueue;

void fqueue_init(fqueue *queue);
void fqueue_destroy(fqueue *queue);
int fqueue_is_empty(const fqueue *queue);
void fqueue_add(figure_shape *shape, fqueue *queue);
void fqueue_pop(figure_shape **shape, fqueue *queue);

#endif
