#ifndef TETRIS_H_SENTRY
#define TETRIS_H_SENTRY

#include "field.h"

struct tetris {
    int win_height, win_width;
    field field; 
    int score;
    double speed;
};
typedef struct tetris tetris;

void init_tetris(tetris *game);
void start_tetris(tetris *game);

#endif
