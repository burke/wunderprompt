all: prompt

prompt: prompt.c components/path.c components/ruby.c components/git.c components/prompt.c
		cc -Wall -O3 prompt.c components/path.c components/ruby.c components/git.c components/prompt.c -o prompt


