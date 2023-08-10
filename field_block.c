#include <ncurses.h>

#include "field_block.h"
#include "colors.h"

void set_field_block(char ch, int color, field_block_t *block)
{
    block->ch = ch;
    block->color = color; 
}

void show_field_block(int x, int y, const field_block_t *block)
{
    if (!work_bw)
        attrset(COLOR_PAIR(block->color));
    move(y, x);
    addch(block->ch);
    if (!work_bw)
        attrset(COLOR_PAIR(0));

}
