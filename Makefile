CC := gcc -std=c99
CFLAGS := -Wall -Wextra -pedantic -fpic
OPT := -O3
DEBUG :=

LD := gcc -std=c99
LINKOPTIONS = $(DEBUG) -shared -Wl,-soname,$(LIBFILE).$(MAJOR_VERSION)
LFLAGS := -lm

INSTALL := cp -p
UNINSTALL := rm -f
AR := ar -crs
MKDIR := mkdir -pv
RMDIR := rm -rf
LDCONFIG := ldconfig

SHELL := /bin/bash
PREFIX := /usr/local
INSTALL_LIBDIR := $(PREFIX)/lib
INSTALL_HEADERDIR := $(PREFIX)/include


MAJOR_VERSION := 2
MINOR_VERSION := 0
VERSION := $(MAJOR_VERSION).$(MINOR_VERSION)

NAME := progbar
SRC := $(NAME).c
HEADER := $(NAME).h
LIBFILE := lib$(NAME).so

BUILDDIR := build
OBJ := $(BUILDDIR)/$(NAME).o
LIB := $(BUILDDIR)/lib$(NAME).so


$(LIB).$(VERSION): $(OBJ)
	$(LD) $(LINKOPTIONS) -o $@ $(OBJ) $(LFLAGS)

$(OBJ): $(SRC) $(HEADER) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(OPT) $(DEBUG) -c $< -o $@

$(BUILDDIR):
	$(MKDIR) $@


.PHONY: install uninstall clean clean-demo
install:
	$(INSTALL) $(HEADER) $(INSTALL_HEADERDIR)
	$(INSTALL) $(LIB).$(VERSION) $(INSTALL_LIBDIR)
	ln -fs $(INSTALL_LIBDIR)/$(LIBFILE).$(VERSION) $(INSTALL_LIBDIR)/$(LIBFILE).$(MAJOR_VERSION)
	ln -fs $(INSTALL_LIBDIR)/$(LIBFILE).$(MAJOR_VERSION) $(INSTALL_LIBDIR)/$(LIBFILE)
	$(LDCONFIG)

uninstall:
	$(UNINSTALL) $(INSTALL_HEADERDIR)/$(HEADER)
	$(UNINSTALL) $(INSTALL_LIBDIR)/$(LIBFILE)
	$(UNINSTALL) $(INSTALL_LIBDIR)/$(LIBFILE).$(MAJOR_VERSION)
	$(UNINSTALL) $(INSTALL_LIBDIR)/$(LIBFILE).$(VERSION)
	$(LDCONFIG)

clean:
	$(RMDIR) $(BUILDDIR)

clean-demo:
	$(MAKE) -C demo
