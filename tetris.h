#ifndef TETRIS_H_SENTRY
#define TETRIS_H_SENTRY

#include "field.h"
#include "figure.h"

struct tetris {
    int win_height, win_width;
    field_t field; 
    figure_t figure;
    int score;
    double speed;
    int game_over;
};
typedef struct tetris tetris_t;

void init_tetris(tetris_t *game);
void start_tetris(tetris_t *game);

#endif
