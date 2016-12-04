CC := gcc -std=c99
CFLAGS := -Wall -Wextra
OPT := -O3
DEBUG := -g
LFLAGS := -lm

LIB := libprogbar.a
SRC := progbar.c
OBJ := progbar.o
HEADER := progbar.h

PREFIX := /usr/local

$(LIB): $(OBJ)
	ar crs $@ $<
	ranlib $@

demo: demo.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

demo.o: demo.c $(HEADER)
	$(CC) $(CFLAGS) $(DEBUG) -c $<

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(OPT) -c $<

install: $(LIB)
	cp -p $(HEADER) $(PREFIX)/include/
	cp -p $(LIB) $(PREFIX)/lib/

uninstall:
	rm -f $(PREFIX)/include/$(HEADER)
	rm -f $(PREFIX)/lib/$(LIB)

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJ) demo.o demo
