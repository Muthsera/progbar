SRCS := $(wildcard *.c)
HEADERS := $(wildcard *.h)
TARGETS := $(SRCS:.c=)

.PHONY: default all demo
default all demo: $(TARGETS)

%: %.c $(HEADERS)
	gcc -std=c99 -Wall -Wextra -Wpedantic -o $@ $< --static -L../build -lprogbar -lm


.PHONY: clean clean-demo
clean clean-demo:
	rm -f $(TARGETS)
