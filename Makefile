# #####################################
# Variables for build process
# #####################################

include Makefile_in


# #####################################
# Variables describing the project
# #####################################

LIBNAME := progbar
HEADER := $(LIBNAME).h
MAJOR_VERSION := 2
MINOR_VERSION := 1
VERSION := $(MAJOR_VERSION).$(MINOR_VERSION)

SRCDIR := .
BUILDDIR := build
DEMODIR := demo

SRC := $(LIBNAME).c
OBJ := $(BUILDDIR)/$(LIBNAME).o


# #####################################
# Use input from configure
# #####################################

include Makefile_conf

INSTALL_LIBDIR := $(PREFIX)/lib
INSTALL_HEADERDIR := $(PREFIX)/include

LIBFILE := lib$(LIBNAME).$(LIBEXT)


# #####################################
# building libs
# #####################################

$(BUILDDIR)/$(LIBFILE): $(OBJ)
	$(CREATELIB) $@ $^ $(LDFLAGS)

$(OBJ): $(SRC) $(HEADER) | $(BUILDDIR)
	$(CC) $(COPTIONS) -c -o $@ $<

$(BUILDDIR):
	@$(MKDIR) $@


.PHONY: demo
demo:
	$(MAKE) -C $(DEMODIR)


# #####################################
# clean up
# #####################################

.PHONY: clean distclean clean-demo
clean:
	$(RMDIR) $(BUILDDIR)

distclean: clean
	$(RM) Makefile_conf

clean-demo:
	$(MAKE) -C demo clean


# #####################################
# install
# #####################################

.PHONY: install
install: | $(INSTALL_LIBDIR) $(INSTALL_HEADERDIR)
	$(INSTALL) $(HEADER) $(INSTALL_HEADERDIR)
	$(INSTALL) $(BUILDDIR)/$(LIBFILE) $(INSTALL_LIBDIR)
	$(CALL_AFTER_INSTALL)

$(INSTALL_LIBDIR) $(INSTALL_HEADERDIR):
	@$(MKDIR) $@


# #####################################
# uninstall
# #####################################

.PHONY: uninstall
uninstall:
	$(UNINSTALL) $(INSTALL_LIBDIR)/$(LIBFILE)
	$(UNINSTALL) $(INSTALL_HEADERDIR)/$(HEADER)
