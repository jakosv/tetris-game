#ifndef COLORS_H_SENTRY
#define COLORS_H_SENTRY

#include <ncurses.h>

enum { DEFAULT_COLOR = 0, colors_count = 6 };

extern int work_bw;


void init_colors();

#endif
