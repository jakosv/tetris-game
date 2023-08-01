#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "tetris.h"
#include "field.h"
#include "time_utils.h"
#include "colors.h"


enum { key_exit = 'q', key_space = ' ' };
enum { delay_duration = 60 };
enum { figure_move_interval = 1000 };

const double start_game_speed = 1.0;
const double game_speed_increase = 0.5;

static void init_ncurses(tetris *game)
{
    int row, col;
    initscr();
    cbreak();
    timeout(delay_duration);
    keypad(stdscr, 1);
    noecho();
    curs_set(0);
    getmaxyx(stdscr, row, col);
    game->win_height = row;
    game->win_width = col;
}

void init_tetris(tetris *game)
{
    init_ncurses(game);
    init_field(&game->field, 
               (game->win_width-fwidth)/2, (game->win_height-fheight)/2);  
    game->score = 0;
    game->speed = start_game_speed;
    init_colors();
}

static int is_game_over(tetris *game)
{
    return game->field.blocks[figure_start_y][figure_start_x] == fblock;
}

static void handle_key_up(tetris *game)
{
    hide_figure(&game->field);
    rotate_figure(&game->field); 
    show_figure(&game->field);
}

static int is_row_full(int row, const field *fld)
{
    int i;
    for (i = 1; i < game_field_width - 1; i++)
        if (fld->blocks[row][i] == fempty)
            return 0;
    return 1;
}

static int *get_full_rows(tetris *game, int *count)
{
    int i;
    int arr[fheight-2];
    int *rows;
    *count = 0;
    for (i = fheight - 2; i >= 2; i--)
        if (is_row_full(i, &game->field)) {
            arr[*count] = i;
            ++(*count);
        }
    rows = NULL;
    if (*count > 0) {
        rows = malloc(*count * sizeof(int));
        memcpy(rows, arr, *count * sizeof(int));
    }
    return rows;
}

static void move_top_rows_down(int start_row, field *fld)
{
    int i, j;
    for (i = start_row; i >= 2; i--)
        for (j = 1; j < game_field_width - 1; j++)
            fld->blocks[i][j] = fld->blocks[i-1][j];
}

static void remove_full_rows(int *rows, int count, tetris *game)
{
    game->score += count * count;
    game->speed += game_speed_increase;
    while (count > 0) {
        --count;
        move_top_rows_down(rows[count], &game->field);
    }
    free(rows);
    rows = NULL;
}

int check_figure_time(tetris *game)
{
    struct timeval now;
    long msec_between;
    gettimeofday(&now, NULL);
    msec_between = 
        milliseconds_between(&game->field.figure->last_move_time, &now);
    return (long)(1.0*msec_between * game->speed) >= figure_move_interval; 
}

static void show_score(const tetris *game)
{
    enum { score_str_max_len = 10 };
    char buf[score_str_max_len];
    int x, y;
    sprintf(buf, "Score: %d", game->score);
    x = game->field.x + (fwidth - strlen(buf)) / 2;
    y = game->field.y - 1;
    mvprintw(y, x, "%s", buf);
}

void start_tetris(tetris *game)
{
    int key, full_rows_cnt;
    int *full_rows;
    full_rows = NULL;
    show_score(game);
    draw_field(&game->field);
    while((key = getch()) != key_exit) {
        switch (key) {
        case KEY_UP:
            handle_key_up(game);
            break;
        case KEY_DOWN:
            move_figure(&game->field, 0, 1);
            break;
        case KEY_LEFT:
            move_figure(&game->field, -1, 0);
            break;
        case KEY_RIGHT:
            move_figure(&game->field, 1, 0);
            break;
        case key_space:
            force_figure_down(&game->field);
        case KEY_RESIZE:
            break;
        default:
            if (check_figure_time(game))
                move_figure(&game->field, 0, 1);
        }
        if (is_game_over(game))
            break;
        full_rows = get_full_rows(game, &full_rows_cnt);
        if (full_rows_cnt > 0) {
            remove_full_rows(full_rows, full_rows_cnt, game);
            show_score(game);
            update_player_figure(&game->field);
            draw_field(&game->field);
        }
    }
    destroy_field(&game->field);
    timeout(-1);
    key = getch();
    endwin();
}
