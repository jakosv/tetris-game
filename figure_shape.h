#ifndef FIGURE_SHAPE_H_SENTRY
#define FIGURE_SHAPE_H_SENTRY

enum shape_type { st_I, st_J, st_L, st_O, st_S, st_T, st_Z , st_count };

enum { shape_blocks = 4 };
enum { block_coords = 2 };

struct figure_shape {
    signed char blocks[shape_blocks][block_coords];
    enum shape_type type;
    int color;
};
typedef struct figure_shape figure_shape;

extern const int default_shapes[st_count][shape_blocks][block_coords];

figure_shape *generate_figure_shape();
void rotate_figure_shape(figure_shape *shape);
void draw_figure_shape(int x, int y, char ch, const figure_shape *shape);

#endif
