PROG = tetris
CFLAGS = -Wall -g -ansi  -pedantic -I/usr/local/include
LDFLAGS = -lncurses
OBJMODULES = tetris.o field.o figure_shape.o figures_queue.o time_utils.o\
			 colors.o

.PHONY: clean

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm $(PROG) *.o
