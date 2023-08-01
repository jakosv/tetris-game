#include "tetris.h"

#include <stdlib.h>
#include <time.h>

int main()
{
    tetris game;
    srandom(time(NULL));
    init_tetris(&game);
    start_tetris(&game);

    return 0;
}
