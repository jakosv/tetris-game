#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "field.h"
#include "field_block.h"
#include "colors.h"

enum { border_char = '#' };

static void show_border_line(int x, int y, int width)
{
    int i;
    move(y, x);
    for (i = 0; i < width; i++)
        addch(border_char);
}

static void show_field_borders(const field_t *field)
{
    int i;
    show_border_line(field->x-1, field->y-1, field->width+2);
    for (i = 0; i < field->height; i++) {
        move(field->y + i, field->x-1);
        addch(border_char);
        move(field->y + i, field->x + field->width);
        addch(border_char);
    }
    show_border_line(field->x-1, field->y + field->height, field->width+2);
}

void clear_field(field_t *field)
{
    int i, j;

    for (i = 0; i < field->height; i++)
        for (j = 0; j < field->width; j++)
            set_field_block(empty_block_char, DEFAULT_COLOR, 
                            &field->blocks[i][j]);
}

void init_field(field_t *field, int height, int width, int x, int y)
{
    int i;

    field->height = height;
    field->width = width;

    field->x = x;
    field->y = y;

    field->blocks = malloc(height * sizeof(field_block_t *));
    for (i = 0; i < height; i++)
        field->blocks[i] = malloc(width * sizeof(field_block_t));

    clear_field(field);
}

void destroy_field(field_t *field)
{
    int i;

    for (i = 0; i < field->height; i++)
        free(field->blocks[i]);
    free(field->blocks);
}

void set_field_coords(field_t *field, int x, int y)
{
    field->x = x;
    field->y = y;
}


int check_field_coords(int x, int y, const field_t *field)
{
    return (x >= 0 && x < field->width && y >= 0 && y < field->height);
}

int is_field_block_empty(int x, int y, const field_t *field)
{
    return field->blocks[y][x].ch == empty_block_char;
}

void show_field(const field_t *field)
{
    int x, y;

    show_field_borders(field);

    for (y = 0; y < field->height; y++)
        for (x = 0; x < field->width; x++) {
            int block_x, block_y;
            block_x = field->x + x;
            block_y = field->y + y;
            show_field_block(block_x, block_y, &field->blocks[y][x]);
        }

    refresh();
}
