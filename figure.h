#ifndef FIGURE_H_SENTRY
#define FIGURE_H_SENTRY

#include <sys/time.h>

#include "shape.h"
#include "field.h"


enum { figure_block_char = '#', shadow_block_char = '.' };


struct figure {
    int x, y;
    int shadow_y;
    shape_t *shape;
    field_t *field;
    struct timeval last_move_time;
};
typedef struct figure figure_t;

void init_figure(figure_t *figure, shape_t *shape, field_t *field);
void destroy_figure(figure_t *figure);
void reinit_figure(figure_t *figure, shape_t *shape);

void show_figure(const figure_t *figure);
void hide_figure(const figure_t *figure);
int move_figure(figure_t *figure, int dx, int dy);
void rotate_figure(figure_t *figure);
void force_figure_down(figure_t *figure);

#endif
