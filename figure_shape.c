#include <stdlib.h>
#include <ncurses.h>

#include "figure_shape.h"
#include "colors.h"


const int default_shapes[st_count][shape_blocks][block_coords] = {
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

figure_shape *generate_figure_shape()
{
    int type, i;
    figure_shape *shape;

    shape = malloc(sizeof(*shape));
    type = rand_number(0, st_count-1);
    shape->type = type;
    for (i = 0; i < shape_blocks; i++) {
        shape->blocks[i][0] = default_shapes[type][i][0];
        shape->blocks[i][1] = default_shapes[type][i][1];
    }
    shape->color = rand_number(1, color_count);
    return shape;
}

void rotate_figure_shape(figure_shape *shape)
{
    int i;
    for (i = 0; i < shape_blocks; i++) {
        int x, y;
        x = shape->blocks[i][0];
        y = shape->blocks[i][1];
        shape->blocks[i][0] = y;
        shape->blocks[i][1] = -x;
    }
}

void draw_figure_shape(int x, int y, char ch, const figure_shape *shape)
{
    int i;
    for (i = 0; i < shape_blocks; i++) {
        int dx, dy;
        dx = shape->blocks[i][0];
        dy = shape->blocks[i][1];
        move(y + dy, x + dx);
        if (!work_bw)
            attrset(COLOR_PAIR(shape->color));
        addch(ch);
        if (!work_bw)
            attrset(COLOR_PAIR(0));
    }
}
