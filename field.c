#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "field.h"
#include "figure_shape.h"
#include "figures_queue.h"
#include "time_utils.h"


enum { next_figure_text_y = 2 };
enum { next_figure_y = 5, next_figures_padding = 3 };

static int calculate_shadow_center_y(const field *fld)
{
    int x, y, i;
    figure_shape *shape;
    shape = fld->figure->shape;
    x = fld->figure->center_x; 
    y = fld->figure->center_y; 
    for (;; y++)
        for (i = 0; i < shape_blocks; i++) {
            int block_x, block_y;
            block_x = x + shape->blocks[i][0];
            block_y = y + shape->blocks[i][1];
            if (fld->blocks[block_y][block_x] == fblock)
                return y - 1;
        }
}


static void add_player_figure(field *fld)
{
    figure *player_figure;
    player_figure = fld->figure;
    fqueue_pop(&player_figure->shape, fld->next_figures);
    player_figure->center_x = figure_start_x;
    player_figure->center_y = figure_start_y;
    player_figure->shadow_center_y = calculate_shadow_center_y(fld);
    gettimeofday(&player_figure->last_move_time, NULL);
}

static void add_next_figure(fqueue *next_figures)
{
    figure_shape *new_shape;
    new_shape = generate_figure_shape();
    fqueue_add(new_shape, next_figures);
}

static void create_field_borders(field *fld)
{
    int i, j;

    memset(fld->blocks, fblock, fheight * fwidth);
    for (i = 1; i < fheight-1; i++)
        for (j = 1; j < fwidth-1; j++)
            if (j != game_field_width - 1)
                fld->blocks[i][j] = fempty;
}

void init_field(field *fld, int x, int y)
{
    int i;

    fld->x = x;
    fld->y = y;

    create_field_borders(fld);

    fld->next_figures = malloc(sizeof(fqueue));
    fqueue_init(fld->next_figures);
    for (i = 0; i < next_figures_count + 1; i++)
        add_next_figure(fld->next_figures);

    fld->figure = malloc(sizeof(figure));
    add_player_figure(fld);
}

void destroy_field(field *fld)
{
    free(fld->figure);
    fld->figure = NULL;

    fqueue_destroy(fld->next_figures);
    free(fld->next_figures);
    fld->next_figures = NULL;
}

static void draw_player_figure(const field *fld, char ch)
{
    int x, y;
    figure *player_figure;
    figure_shape *shape;

    player_figure = fld->figure;
    shape = fld->figure->shape;
    x = player_figure->center_x; 
    y = player_figure->center_y; 
    x += fld->x;
    y += fld->y;
    draw_figure_shape(x, y, ch, shape);
}

static void draw_player_figure_shadow(const field *fld, char ch)
{
    int x, y;
    figure_shape *shape;

    x = fld->figure->center_x; 
    y = fld->figure->shadow_center_y; 
    if ((x < 1 || x >= game_field_width - 1) || (y < 1 || y >= fheight - 1))
        return;
    x += fld->x;
    y += fld->y;
    shape = fld->figure->shape;
    draw_figure_shape(x, y, ch, shape);
}

void show_figure(const field *fld)
{
    draw_player_figure_shadow(fld, fshadow);
    draw_player_figure(fld, fblock);
}

void hide_figure(const field *fld)
{
    draw_player_figure_shadow(fld, fempty);
    draw_player_figure(fld, fempty);
}

static void move_player_figure_to_field(field *fld)
{
    int i;
    figure *player_figure;

    player_figure = fld->figure;
    for (i = 0; i < shape_blocks; i++) {
        int x, y;
        x = player_figure->shape->blocks[i][0] + player_figure->center_x;
        y = player_figure->shape->blocks[i][1] + player_figure->center_y;
        fld->blocks[y][x] = fblock;
    }
}

void update_player_figure(field *fld)
{
    add_player_figure(fld);
    add_next_figure(fld->next_figures);
}

static void save_player_figure(field *fld)
{
    move_player_figure_to_field(fld);
    free(fld->figure->shape);
}

static int check_figure_collision(const field *fld, int *x, int *y)
{
    int i;
    figure *fig;

    fig = fld->figure;
    for (i = 0; i < shape_blocks; i++) {
        *x = (fig->center_x + fig->shape->blocks[i][0]);
        *y = (fig->center_y + fig->shape->blocks[i][1]);
        if (fld->blocks[*y][*x] == fblock) {
            return 1;
        }
    }
    return 0;
}

static void move_player_figure(figure *fig, int dx, int dy)
{
    fig->center_x += dx;
    fig->center_y += dy;
} 

void move_figure(field *fld, int dx, int dy)
{
    int collision_x, collision_y;
    figure *fig;

    fig = fld->figure;
    hide_figure(fld); 
    move_player_figure(fig, dx, dy);
    if (check_figure_collision(fld, &collision_x, &collision_y)) {
        move_player_figure(fig, -dx, -dy);
        if (dx == 0) {
            save_player_figure(fld);
            update_player_figure(fld);
            draw_field(fld);
        }
    } else {
        gettimeofday(&fig->last_move_time, NULL);
        if (dx != 0)
            fig->shadow_center_y = calculate_shadow_center_y(fld);
    }
    show_figure(fld);
}

void rotate_figure(field *fld)
{
    int i, collision_x, collision_y;
    figure *player_figure;
    figure_shape shape;

    player_figure = fld->figure;
    shape = *player_figure->shape;
    if (shape.type == st_O)
        return;
    rotate_figure_shape(player_figure->shape);    
    for (i = 0; i < shape_blocks - 1; i++) {
        if (check_figure_collision(fld, &collision_x, &collision_y)) {
            int dx, dy;
            dx = collision_x - fld->figure->center_x;
            dy = collision_y - fld->figure->center_y;
            if (dx != 0)
                dx /= abs(dx);
            else if (dy != 0)
                dy /= abs(dy);
            move_player_figure(fld->figure, -dx, -dy);
        } else {
            player_figure->shadow_center_y = calculate_shadow_center_y(fld);
            return;
        }
    }
    *player_figure->shape = shape;
}

void force_figure_down(field *fld)
{
    figure *fig;
    fig = fld->figure;
    hide_figure(fld); 
    fig->center_y = fig->shadow_center_y;
    save_player_figure(fld);
    update_player_figure(fld);
    show_figure(fld);
    draw_field(fld);
}

static void draw_next_figures(const field *fld)
{
    int row;
    struct fqueue_item *tmp;

    mvprintw(fld->y + next_figure_text_y, 
             fld->x + game_field_width + 1, "Next");
    row = next_figure_y;
    for (tmp = fld->next_figures->first; tmp; tmp = tmp->next) {
        int x;
        x = fld->x + game_field_width + (fwidth - game_field_width) / 2;
        draw_figure_shape(x, row, fblock, tmp->data);
        row += next_figures_padding;
    }
}

void draw_field(const field *fld)
{
    int i, j;
    for (i = 0; i < fheight; i++)
        for (j = 0; j < fwidth; j++) {
            move(fld->y + i, fld->x + j);
            addch(fld->blocks[i][j]);
        }
    show_figure(fld);
    draw_next_figures(fld);
    refresh();
}
