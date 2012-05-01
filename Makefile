all: prompt

prompt: prompt.c components/*.c
		cc -Wall -O3 prompt.c components/*.c -o prompt


