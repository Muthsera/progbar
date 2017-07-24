SRCS := $(wildcard *.c)
HEADERS := $(wildcard *.h)
TARGETS := $(SRCS:.c=)
LIBDIR := ../build
# LDFLAGS := -Wl,-rpath=$(LIBDIR)
LDFLAGS := -L$(LIBDIR)

.PHONY: default all demo
default all demo: $(TARGETS)

%: %.c $(HEADERS)
	gcc -std=c99 -Wall -Wextra -Wpedantic -o $@ $< $(LDFLAGS) -lprogress -lm


.PHONY: clean clean-demo
clean clean-demo:
	rm -f $(TARGETS)
