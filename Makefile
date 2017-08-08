# #####################################
# Variables for build process
# #####################################

include Makefile_in


# #####################################
# Variables describing the project
# #####################################

LIBNAME := progress
HEADER := $(LIBNAME).h
MAJOR_VERSION := 3
MINOR_VERSION := 0
VERSION := $(MAJOR_VERSION).$(MINOR_VERSION)


SRCDIR := src
BUILDDIR := build
DEPDIR := dep
DEMODIR := demo

SRCS := $(wildcard $(SRCDIR)/*.c)
SRCNAMES := $(notdir $(SRCS))
OBJNAMES := $(SRCNAMES:.c=.o)
DEPNAMES := $(SRCNAMES:.c=.d)
OBJS := $(addprefix $(BUILDDIR)/,$(OBJNAMES) )
DEPS := $(addprefix $(DEPDIR)/,$(DEPNAMES) )

# use nanosleep function in time.h
DFLAGS := -D_POSIX_C_SOURCE=199309L
LDFLAGS := -pthread


#################################################
# Use input from configure
#################################################

include Makefile_conf

INSTALL_LIBDIR := $(PREFIX)/lib
INSTALL_HEADERDIR := $(PREFIX)/include
LIBFILE := lib$(LIBNAME).$(LIBEXT)


#################################################
# building libs
#################################################

$(BUILDDIR)/$(LIBFILE): $(OBJS) | $(BUILDDIR)
	$(CREATELIB) $@ $^ $(CREATEFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(DEPDIR) $(BUILDDIR)
	@$(CC) $(COPTIONS) -MM -MT $@ -MP -MF $(DEPDIR)/$*.d $<
	$(CC) $(COPTIONS) -c -o $@ $<

-include $(DEPFILES)

$(BUILDDIR) $(DEPDIR):
	@$(MKDIR) $@


.PHONY: demo
demo:
	$(MAKE) -C $(DEMODIR)


#################################################
# clean up
#################################################

.PHONY: clean distclean clean-demo
clean:
	$(RMDIR) $(BUILDDIR) $(DEPDIR)

distclean: clean
	$(RM) Makefile_conf

clean-demo:
	$(MAKE) -C $(DEMODIR) clean


#################################################
# install
#################################################

.PHONY: install
install: | $(INSTALL_LIBDIR) $(INSTALL_HEADERDIR)
	$(INSTALL) $(HEADER) $(INSTALL_HEADERDIR)
	$(INSTALL) $(BUILDDIR)/$(LIBFILE) $(INSTALL_LIBDIR)
	$(CALL_AFTER_INSTALL)

$(INSTALL_LIBDIR) $(INSTALL_HEADERDIR):
	@$(MKDIR) $@

#############################################
# uninstall
#################################################

.PHONY: uninstall
uninstall:
	$(UNINSTALL) $(INSTALL_LIBDIR)/$(LIBFILE)
	$(UNINSTALL) $(INSTALL_HEADERDIR)/$(HEADER)
	$(LDCONFIG)
