#ifndef FIELD_H_SENTRY
#define FIELD_H_SENTRY

#include <sys/time.h>

#include "figure_shape.h"
#include "figures_queue.h"


enum { fwidth = 19, fheight = 22, game_field_width = 12 };
enum { fborder = '#', fblock = '#', fempty = ' ', fshadow = '.' };
enum { figure_start_x = game_field_width / 2, figure_start_y = 1 };
enum { next_figures_count = 3 };

struct figure {
    int center_x, center_y;
    int shadow_center_y;
    figure_shape *shape;
    struct timeval last_move_time;
};
typedef struct figure figure;

struct field {
    int x, y;
    char blocks[fheight][fwidth];
    figure *figure;
    fqueue *next_figures;
};
typedef struct field field;

void init_field(field *fld, int x, int y);
void destroy_field(field *fld);
void draw_field(const field *fld);
void hide_figure(const field *fld);
void show_figure(const field *fld);
void move_figure(field *fld, int dx, int dy);
void rotate_figure(field *fld);
void update_player_figure(field *fld);
void force_figure_down(field *fld);

#endif
