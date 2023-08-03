PROG = tetris
CFLAGS = -Wall -g -ansi -pedantic
LDFLAGS = -lncurses
OBJMODULES = tetris.o field.o figure_shape.o figures_queue.o time_utils.o\
			 colors.o

.PHONY: clean

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

clean:
	rm $(PROG) *.o
