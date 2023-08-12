#ifndef FIELD_H_SENTRY
#define FIELD_H_SENTRY

#include <sys/time.h>

#include "field_block.h"

enum { empty_block_char = ' ' };

struct field {
    int height, width;
    int x, y;
    field_block_t **blocks;
};
typedef struct field field_t;

void init_field(field_t *field, int height, int width, int x, int y);
void destroy_field(field_t *field);

void set_field_coords(field_t *field, int x, int y);
void show_field(const field_t *field);
int check_field_coords(int x, int y, const field_t *field);
int is_field_block_empty(int x, int y, const field_t *field);
void clear_field(field_t *field);

#endif
