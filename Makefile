CFLAGS     := -Wall -O3
SHELL       = /bin/sh
SHELL_NAME := ZSH
INSTALL    := install

DEFS     = -D$(SHELL_NAME)
objects  = ruby_info git-prompt-helper git-ref-and-time prompt
binaries = ruby_info git-ref-and-time prompt
bindir   = bin

prefix       := /usr/local
exec_prefix  := $(prefix)
bindir       := $(exec_prefix)/bin
srcdir       := ./

deps = git-prompt-helper.c git-prompt-helper.h colours.h

git-prompt-helper: git-prompt-helper.c git-prompt-helper.h

prompt: $(deps)
	$(CC) $(CFLAGS) $(DEFS) -o prompt $(deps) prompt.c

git-ref-and-time:  $(deps)
	$(CC) $(CFLAGS) $(DEFS) -o git-ref-and-time $(deps) git-ref-and-time.c

ruby_info: ruby_info.c colours.h
	$(CC) $(CFLAGS) $(DEFS) -o ruby_info ruby_info.c

all: $(binaries)

clean:
	rm -rf $(objects)

install: all
	$(INSTALL) prompt    $(bindir)/prompt
	$(INSTALL) ruby_info $(bindir)/ruby_info

uninstall:
	rm -rf $(bindir)/prompt
	rm -rf $(bindir)/ruby_info
