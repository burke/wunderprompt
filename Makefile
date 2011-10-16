CFLAGS:=-Wall -O3
SHELL:=ZSH

DEFS=-D$(SHELL)
objects=prompt ruby_info
binaries=$(objects)
bin_dir=/usr/local/bin

prompt: prompt.c colours.h
	$(CC) $(CFLAGS) $(DEFS) -o prompt prompt.c

ruby_info: ruby_info.c colours.h
	$(CC) $(CFLAGS) $(DEFS) -o ruby_info ruby_info.c

all: $(objects)

clean:
	rm -rf $(objects)

install: all
	chmod +x $(objects)
	mv $(binaries) $(bin_dir)
