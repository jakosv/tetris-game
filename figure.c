#include <stdlib.h>
#include <ncurses.h>
#include <sys/time.h>

#include "figure.h"
#include "shape.h"


static int calc_shadow_y(int figure_x, int figure_y, const shape_t *shape, 
                         const field_t *field)
{
    int x, y;
    x = figure_x;
    y = figure_y + 1;
    for (;; y++)
        if (check_shape_collisions(x, y, shape, field))
            return y - 1;
    return -1;
}

void init_figure(figure_t *figure, shape_t *shape, field_t *field)
{
    figure->x = field->width / 2;
    figure->y = 0;
    figure->shadow_y = calc_shadow_y(figure->x, figure->y, shape, field);
    figure->shape = shape;
    figure->field = field;
    gettimeofday(&figure->last_move_time, NULL);
}

void destroy_figure(figure_t *figure)
{
    free(figure->shape);
}

void reinit_figure(figure_t *figure, shape_t *shape)
{
    field_t *field;
    field = figure->field;
    destroy_figure(figure);
    init_figure(figure, shape, field);
}


void show_figure(const figure_t *figure)
{
    int x, y;
    shape_t *shape;
    field_t *field;

    x = figure->x;
    y = figure->y;
    shape = figure->shape;
    field = figure->field;

    show_shape(x, figure->shadow_y, shadow_block_char, shape, field);
    show_shape(x, y, figure_block_char, shape, field);
}

void hide_figure(const figure_t *figure)
{
    int x, y;
    shape_t *shape;
    field_t *field;

    x = figure->x;
    y = figure->y;
    shape = figure->shape;
    field = figure->field;

    show_shape(x, figure->shadow_y, empty_block_char, shape, field);
    show_shape(x, y, empty_block_char, shape, field);
}

int move_figure(figure_t *figure, int dx, int dy)
{
    int has_collision;
    hide_figure(figure); 

    has_collision = check_shape_collisions(figure->x + dx, figure->y + dy, 
                                     figure->shape, figure->field);
    if (!has_collision) {
        figure->x += dx;
        figure->y += dy;
        if (dx != 0) {
            figure->shadow_y = calc_shadow_y(figure->x, figure->y, 
                                             figure->shape, figure->field);
        } else {
            gettimeofday(&figure->last_move_time, NULL);
        }
    }
    show_figure(figure);
    return has_collision;
}

static void resolve_field_bounds_collision(int block_x, int block_y, 
                                           int *figure_x, int *figure_y,
                                           const field_t *field)
{
    int x, y;

    x = *figure_x;
    y = *figure_y;

    if (block_x < 0)
        x += abs(block_x);
    else if (block_x >= field->width)
        x = x - (block_x - field->width + 1);

    if (block_y < 0)
        y += abs(block_y);
    else if (block_y >= field->height)
        y = y - (block_y - field->height + 1);

    *figure_x = x;
    *figure_y = y;
}

static void resolve_block_collision(int shape_block_x, int shape_block_y,
                                    int *figure_x, int *figure_y)
{
    if (shape_block_x < 0)
        (*figure_x)++;
    else
        (*figure_x)--;

    if (shape_block_y < 0)
        (*figure_y)++;
    else
        (*figure_y)--;
}

static void resolve_collisions(figure_t *figure, int *new_x, int *new_y)
{
    int i, x, y;

    x = figure->x;
    y = figure->y;

    for (i = 0; i < shape_blocks_cnt; i++) {
        int block_x, block_y;

        block_x = figure->shape->blocks[i].x;
        block_y = figure->shape->blocks[i].y;

        if (block_x == 0 && block_y == 0)
            continue;

        if (!check_field_coords(x + block_x, y + block_y, figure->field))
            resolve_field_bounds_collision(x + block_x, y + block_y, &x, &y,
                                           figure->field);

        if (!is_field_block_empty(x + block_x, y + block_y, figure->field))
            resolve_block_collision(block_x, block_y, &x, &y);
    }

    *new_x = x;
    *new_y = y;
}

void rotate_figure(figure_t *figure)
{
    int new_x, new_y;

    if (figure->shape->type == shape_O)
        return;

    hide_figure(figure);
    rotate_shape(figure->shape, 1);

    resolve_collisions(figure, &new_x, &new_y);

    if (!check_shape_collisions(new_x, new_y, figure->shape, figure->field)) {
        figure->x = new_x;
        figure->y = new_y;
        figure->shadow_y = calc_shadow_y(new_x, new_y, figure->shape, 
                                         figure->field);
    } else {
        rotate_shape(figure->shape, 0);
    }
    
    show_figure(figure);
}

void force_figure_down(figure_t *figure)
{
    hide_figure(figure); 
    figure->y = figure->shadow_y;
    show_figure(figure);
}
