#include <stdlib.h>
#include <ncurses.h>

#include "shape.h"
#include "field.h"
#include "colors.h"


const shape_block_t default_shapes[shapes_cnt][shape_blocks_cnt] = {
    { {-2, 0}, {-1, 0}, {0, 0}, {1, 0} },   /* #### */

    { {-1, 0}, {0, 0}, {1, 0}, {1, 1} },    /* ###  */
                                            /*   #  */
    
    { {-1, 1}, {-1, 0}, {0, 0}, {1, 0} },   /* ###  */
                                            /* #    */

    { {-1, 1}, {-1, 0}, {0, 0}, {0, 1} },   /* ##   */
                                            /* ##   */
    
    { {-1, 1}, {0, 1}, {0, 0}, {1, 0} },    /*  ##  */
                                            /* ##   */

    { {-1, 0}, {0, 0}, {1, 0}, {0, 1} },    /*  ### */
                                            /*   #  */

    { {-1, 0}, {0, 0}, {0, 1}, {1, 1} },    /* ##   */
                                            /*  ##  */
};

static int rand_number(int min, int max)
{
    int count;
    count = max - min + 1;
    return min + (int)(1.0 * random() * count / (RAND_MAX+1.0));
}

shape_t *get_random_shape()
{
    int type, i;
    shape_t *shape;

    shape = malloc(sizeof(*shape));
    type = rand_number(0, shapes_cnt-1);
    shape->type = type;
    for (i = 0; i < shape_blocks_cnt; i++) {
        shape->blocks[i].x = default_shapes[type][i].x;
        shape->blocks[i].y = default_shapes[type][i].y;
    }
    shape->color = rand_number(1, colors_count);
    return shape;
}

static void rotate_clockwise(shape_t *shape)
{
    int i;
    for (i = 0; i < shape_blocks_cnt; i++) {
        int x, y;
        x = shape->blocks[i].x;
        y = shape->blocks[i].y;
        shape->blocks[i].x = y;
        shape->blocks[i].y = -x;
    }
}

static void rotate_counterclockwise(shape_t *shape)
{
    int i;
    for (i = 0; i < shape_blocks_cnt; i++) {
        int x, y;
        x = shape->blocks[i].x;
        y = shape->blocks[i].y;
        shape->blocks[i].x = -y;
        shape->blocks[i].y = x;
    }
}

void rotate_shape(shape_t *shape, int clockwise)
{
    if (clockwise)
        rotate_clockwise(shape);
    else
        rotate_counterclockwise(shape);
    
}

int check_shape_collisions(int x, int y, const shape_t *shape, 
                           const field_t *field)
{
    int i;

    for (i = 0; i < shape_blocks_cnt; i++) {
        int block_x, block_y;
        block_x = x + shape->blocks[i].x;
        block_y = y + shape->blocks[i].y;
        if (!check_field_coords(block_x, block_y, field) ||
            !is_field_block_empty(block_x, block_y, field)) 
        {
            return 1;
        }
    }
    return 0;
}


void show_shape(int x, int y, char ch, const shape_t *shape, field_t *field)
{
    int i;
    for (i = 0; i < shape_blocks_cnt; i++) {
        int block_x, block_y;
        field_block_t *block_ptr;
        block_x = x + shape->blocks[i].x;
        block_y = y + shape->blocks[i].y;
        block_ptr = &field->blocks[block_y][block_x];
        set_field_block(ch, shape->color, block_ptr);
    }
}
