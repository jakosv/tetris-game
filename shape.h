#ifndef SHAPE_H_SENTRY
#define SHAPE_H_SENTRY

#include "field.h"

enum shape_type { 
    shape_I, 
    shape_J, 
    shape_L, 
    shape_O, 
    shape_S, 
    shape_T, 
    shape_Z , 
    shapes_cnt 
};

enum { shape_blocks_cnt = 4 };

struct shape_block {
    int x, y;
};
typedef struct shape_block shape_block_t;

struct shape {
    shape_block_t blocks[shape_blocks_cnt];
    enum shape_type type;
    int color;
};
typedef struct shape shape_t;

extern const shape_block_t default_shapes[shapes_cnt][shape_blocks_cnt];

shape_t *get_random_shape();
void rotate_shape(shape_t *shape, int clockwise);
int check_shape_collisions(int x, int y, const shape_t *shape, 
                           const field_t *field);
void show_shape(int x, int y, char ch, const shape_t *shape, field_t *field);

#endif
