#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "tetris.h"
#include "field.h"
#include "figure.h"
#include "time_utils.h"
#include "colors.h"


enum { key_exit = 'q', key_space = ' ' };
enum { delay_duration = 60 };
enum { figure_move_interval = 1000 };

const double start_game_speed = 1.0;
const double game_speed_increase = 0.5;

/*
enum { next_figures_count = 3 };
enum { next_figure_text_y = 2 };
enum { next_figure_y = 4, next_figures_padding = 3 };

static void add_next_figure(fqueue *next_figures)
{
    figure_shape *new_shape;
    new_shape = generate_figure_shape();
    fqueue_add(new_shape, next_figures);
}

static void draw_next_figures(const field *fld)
{
    int row;
    struct fqueue_item *tmp;
    mvprintw(fld->y + next_figure_text_y, 
             fld->x + game_field_width + 1, "Next");
    row = next_figure_y + fld->y;
    for (tmp = fld->next_figures->first; tmp; tmp = tmp->next) {
        int x;
        x = fld->x + game_field_width + (fwidth - game_field_width) / 2;
        draw_figure_shape(x, row, fblock, tmp->data);
        row += next_figures_padding;
    }
}
*/

static void init_ncurses(tetris_t *game)
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

void init_tetris(tetris_t *game)
{
    init_ncurses(game);
    init_field(&game->field, 
               (game->win_width - field_width) / 2, 
               (game->win_height - field_height) / 2);  
    init_figure(&game->figure, get_random_shape(), &game->field);
    game->score = 0;
    game->speed = start_game_speed;
    game->game_over = 0;
    init_colors();
}

static int is_game_over(tetris_t *game)
{
    int x, y;
    x = game->figure.x;
    y = game->figure.y;

    return check_shape_collisions(x, y, game->figure.shape, 
                                  game->figure.field);
}

static void handle_key_up(tetris_t *game)
{
    rotate_figure(&game->figure); 
}

static void handle_key_space(tetris_t *game)
{
    force_figure_down(&game->figure);
}

static int is_correct_window_size(int height, int width)
{
    return (height >= field_height + 2) && (width >= field_width + 2);
}

static void handle_resize(tetris_t *game)
{
    int row, col;
    getmaxyx(stdscr, row, col);
    if (!is_correct_window_size(row, col)) {
        game->game_over = 1;
        return;
    }
    game->win_width = col;
    game->win_height = row;
    reinit_field(&game->field, 
                (game->win_width - field_width) / 2, 
                (game->win_height - field_height) / 2);  
    clear();
}

static void update_figure(tetris_t *game, figure_t *figure)
{
    reinit_figure(figure, get_random_shape());
}

static int is_row_full(int row, const field_t *field)
{
    int i;
    for (i = 0; i < field_width; i++)
        if (field->blocks[row][i].ch != figure_block_char)
            return 0;
    return 1;
}

static int *get_full_rows(tetris_t *game, int *count)
{
    int i;
    int arr[field_height];
    int *rows;
    *count = 0;
    for (i = field_height-1; i >= 1; i--)
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

static void move_top_rows_down(int start_row, field_t *field)
{
    int i, j;
    for (i = start_row; i >= 1; i--)
        for (j = 0; j < field_width; j++)
            field->blocks[i][j] = field->blocks[i-1][j];
}

static void remove_full_rows(int *rows, int count, tetris_t *game)
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

static int check_figure_time(tetris_t *game)
{
    struct timeval now;
    long msec_between;
    gettimeofday(&now, NULL);
    msec_between = 
        milliseconds_between(&game->figure.last_move_time, &now);
    return (long)(1.0*msec_between * game->speed) >= figure_move_interval; 
}

static void check_full_rows(tetris_t *game)
{
    int full_rows_cnt;
    int *full_rows;
    full_rows = NULL;
    full_rows = get_full_rows(game, &full_rows_cnt);
    if (full_rows_cnt > 0)
        remove_full_rows(full_rows, full_rows_cnt, game);
}

static void show_score(const tetris_t *game)
{
    enum { score_str_max_len = 10 };
    char buf[score_str_max_len];
    int x, y;
    sprintf(buf, "Score: %d", game->score);
    x = game->field.x + (field_width - strlen(buf)) / 2;
    y = game->field.y - 1;
    mvprintw(y, x, "%s", buf);
}

void start_tetris(tetris_t *game)
{
    int key, figure_finish;

    show_figure(&game->figure);
    show_field(&game->field);
    show_score(game);

    figure_finish = 0;
    while((key = getch()) != key_exit) {
        switch (key) {
        case KEY_UP:
            handle_key_up(game);
            break;
        case KEY_DOWN:
            figure_finish = move_figure(&game->figure, 0, 1);
            break;
        case KEY_LEFT:
            move_figure(&game->figure, -1, 0);
            break;
        case KEY_RIGHT:
            move_figure(&game->figure, 1, 0);
            break;
        case key_space:
            handle_key_space(game);
            figure_finish = 1;
            break;
        case KEY_RESIZE:
            handle_resize(game);
            break;
        default:
            if (check_figure_time(game))
                figure_finish = move_figure(&game->figure, 0, 1);
        }

        if (figure_finish) {
            check_full_rows(game);
            update_figure(game, &game->figure);
            game->game_over = is_game_over(game);
            show_figure(&game->figure);
            figure_finish = 0;
        }

        if (game->game_over)
            break;

        show_field(&game->field);
        show_score(game);
    }

    destroy_figure(&game->figure);

    endwin();
}
