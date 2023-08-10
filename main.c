#include "tetris.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
    tetris_t game;
    srandom(time(NULL));
    init_tetris(&game);
    start_tetris(&game);
    printf("Score: %d\n", game.score);

    return 0;
}
