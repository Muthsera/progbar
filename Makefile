CC := gcc -std=c99
CFLAGS := -Wall -Wextra
OPT := -O3
DEBUG := -g
LFLAGS := -lprogbar -lm

LIB := libprogbar.a
SRC := progbar.c
OBJ := progbar.o
HEADER := progbar.h

PREFIX := /usr/local

$(LIB): $(OBJ)
	ar crs $@ $(OBJ)

demo: $(LIB) $(HEADER)
	$(CC) $(DEBUG) $(CFLAGS) -I. -L. demo.c -o $@ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(OPT) $(CFLAGS) -c $<

install: $(LIB)
	cp -p $(HEADER) $(PREFIX)/include/
	cp -p $(LIB) $(PREFIX)/lib/

uninstall:
	rm -f $(PREFIX)/include/$(HEADER)
	rm -f $(PREFIX)/lib/$(LIB)

.PHONY: clean clean-demo
clean:
	rm -f $(LIB) $(OBJ)

clean-demo:
	rm -f demo
