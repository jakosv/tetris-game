#ifndef FIELD_BLOCK_H_SENTRY
#define FIELD_BLOCK_H_SENTRY

struct field_block {
    int ch;
    int color;
};
typedef struct field_block field_block_t;

void set_field_block(char ch, int color, field_block_t *block);
void show_field_block(int x, int y, const field_block_t *block);

#endif
