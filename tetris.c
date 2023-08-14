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
enum { field_width = 10, field_height = 20 };
enum { next_shapes_field_width = 7, next_shapes_field_height = 12 };
enum { next_shapes_count = 3 };
enum { next_shape_text_y = 1 };
enum { next_shape_x = 3, first_next_shape_y = 3, next_shape_padding = 3 };

const double start_game_speed = 1.0;
const double game_speed_increase = 0.5;


static void get_game_field_coords(const tetris_t *game, int *x, int *y)
{
    *x = 
        (game->win_width - (field_width + next_shapes_field_width + 2)) / 2;
    *y = (game->win_height - field_height) / 2 + 1;
}

static void init_next_shapes(tetris_t *game)
{
    int i, x, y;

    get_game_field_coords(game, &x, &y);

    x += game->field.width + 2;

    init_field(&game->next_shapes_field, 
               next_shapes_field_height, next_shapes_field_width, 
               x, y);
    sq_init(&game->next_shapes);
    for (i = 0; i < next_shapes_count; i++)
        sq_add(get_random_shape(), &game->next_shapes);
}

static int is_correct_window_size(int height, int width)
{
    return (height >= field_height + 3) && 
           (width >= field_width + next_shapes_field_width + 2);
}


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
    int field_x, field_y;
    init_ncurses(game);
    get_game_field_coords(game, &field_x, &field_y);
    init_field(&game->field, field_height, field_width, field_x, field_y);  
    init_figure(&game->figure, get_random_shape(), &game->field);

    init_next_shapes(game);

    game->score = 0;
    game->speed = start_game_speed;
    game->game_over = 0;
    init_colors();
}

static void handle_key_up(tetris_t *game)
{
    rotate_figure(&game->figure); 
}

static void handle_key_space(tetris_t *game)
{
    force_figure_down(&game->figure);
}

static void show_next_shapes(tetris_t *game)
{
    int row;
    sq_item_t *tmp;
    field_t *field;

    field  = &game->next_shapes_field;

    clear_field(field);

    row = first_next_shape_y;
    tmp = game->next_shapes.first;    
    for (; tmp; tmp = tmp->next) {
        shape_t *shape;
        shape = tmp->data;
        show_shape(next_shape_x, row, figure_block_char, tmp->data, field);
        row += next_shape_padding;
    }

    show_field(field);
    mvprintw(field->y - 2, field->x + 1, "Next:");
}


static void handle_resize(tetris_t *game)
{
    int row, col, field_x, field_y;
    getmaxyx(stdscr, row, col);
    if (!is_correct_window_size(row, col)) {
        game->game_over = 1;
        return;
    }
    game->win_width = col;
    game->win_height = row;
    get_game_field_coords(game, &field_x, &field_y);
    set_field_coords(&game->field, field_x, field_y);
    set_field_coords(&game->next_shapes_field, 
                     field_x+ field_width + 2, field_y);
    clear();
    show_next_shapes(game);
}

static void update_figure(tetris_t *game, figure_t *figure)
{
    shape_t *shape;

    sq_pop(&shape, &game->next_shapes);
    reinit_figure(figure, shape);
    game->game_over = check_shape_collisions(figure->x, figure->y, 
                                             game->figure.shape, 
                                             game->figure.field);
    show_figure(&game->figure);

    sq_add(get_random_shape(), &game->next_shapes);
    show_next_shapes(game);
}

static int is_row_full(int row, const field_t *field)
{
    int i;
    for (i = 0; i < field->width; i++)
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
        for (j = 0; j < field->width; j++)
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
    y = game->field.y - 2;
    mvprintw(y, x, "%s", buf);
}

static void destroy_tetris(tetris_t *game)
{
    destroy_figure(&game->figure);
    destroy_field(&game->field);
    sq_destroy(&game->next_shapes);
    destroy_field(&game->next_shapes_field);
}

void start_tetris(tetris_t *game)
{
    int key, figure_finish;

    if (!is_correct_window_size(game->win_height, game->win_width)) {
        endwin();
        return;
    }

    show_figure(&game->figure);
    show_field(&game->field);
    show_score(game);
    show_next_shapes(game);

    figure_finish = 0;
    while ((key = getch()) != key_exit && !game->game_over) {
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
            figure_finish = 0;
        }

        show_field(&game->field);
        show_score(game);
    }

    if (game->game_over) {
        timeout(-1);
        key = getch();
    }

    destroy_tetris(game);

    endwin();
}
