CFLAGS     := -Wall -O3
SHELL       = /bin/sh
SHELL_NAME := ZSH
INSTALL    := install

DEFS     = -D$(SHELL_NAME)
objects  = prompt ruby_info
binaries = $(objects)
bindir   = bin

prefix       := /usr/local
exec_prefix  := $(prefix)
bindir       := $(exec_prefix)/bin
srcdir       := ./

prompt: prompt.c colours.h
	$(CC) $(CFLAGS) $(DEFS) -o prompt prompt.c

ruby_info: ruby_info.c colours.h
	$(CC) $(CFLAGS) $(DEFS) -o ruby_info ruby_info.c

all: $(objects)

clean:
	rm -rf $(objects)

install: all
	$(INSTALL) prompt    $(bindir)/prompt
	$(INSTALL) ruby_info $(bindir)/ruby_info

uninstall:
	rm -rf $(bindir)/prompt
	rm -rf $(bindir)/ruby_info
