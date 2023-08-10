#ifndef FIELD_H_SENTRY
#define FIELD_H_SENTRY

#include <sys/time.h>

#include "field_block.h"

enum { field_width = 10, field_height = 20 };
enum { empty_block_char = ' ' };

struct field {
    int x, y;
    field_block_t blocks[field_height][field_width];
};
typedef struct field field_t;

void init_field(field_t *field, int x, int y);
void reinit_field(field_t *field, int x, int y);

void show_field(const field_t *field);
int check_field_coords(int x, int y);
int is_field_block_empty(int x, int y, const field_t *field);

#endif
