all: prompt ruby_info 

prompt: prompt.c 
		cc -Wall -O3 prompt.c -o prompt

ruby_info: ruby_info.c
	cc -Wall -O3 ruby_info.c -o ruby_info

