SRCS := $(wildcard *.c)
HEADERS := $(wildcard *.h)
TARGETS := $(SRCS:.c=)
LIBDIR := ../build
# LDFLAGS := -Wl,-rpath=$(LIBDIR)
LDFLAGS := -L$(LIBDIR)

.PHONY: default all demo
default all demo: $(TARGETS)

%: %.c
	@echo "Nothing to do"

# %: %.c $(HEADERS)
# 	gcc -std=c99 -Wall -Wextra -Wpedantic -o $@ $< $(LDFLAGS) -lprogress -lm

indi: indi.c $(HEADERS)
	gcc -std=c99 -Wall -Wextra -Wpedantic -o $@ $< $(LDFLAGS) -lprogress -lm

bar_short: bar_short.c $(HEADERS)
	gcc -std=c99 -Wall -Wextra -Wpedantic -o $@ $< $(LDFLAGS) -lprogress -lm

.PHONY: clean clean-demo
clean clean-demo:
	rm -f $(TARGETS)
