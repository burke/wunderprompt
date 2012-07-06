CFLAGS     := -Wall -O3
SHELL       = /bin/sh
SHELL_NAME := ZSH
INSTALL    := install

DEFS     = -D$(SHELL_NAME)
objects  = git-prompt-helper
binaries = ruby_info git-ref-and-time prompt
bindir   = bin

prefix       := /usr/local
exec_prefix  := $(prefix)
bindir       := $(exec_prefix)/bin
srcdir       := ./

deps = components/*.c

all: prompt

prompt: prompt.c components/*.c
	$(CC) $(CFLAGS) $(DEFS) -o prompt $(deps) prompt.c

clean:
	rm -rf prompt

install: all
	$(INSTALL) prompt $(bindir)/prompt

uninstall:
	rm -rf $(bindir)/prompt
