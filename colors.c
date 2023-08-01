#include <ncurses.h>

#include "colors.h"

int work_bw = 1;

static const int colors[color_count] = {
    COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
    COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN
};


void init_colors()
{
    int i;
    work_bw = !has_colors();
    if (!work_bw)
        start_color();
    use_default_colors();
    for (i = 0; i < color_count; i++)
        init_pair(i + 1, colors[i], -1);
}
