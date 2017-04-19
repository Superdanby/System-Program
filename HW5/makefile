yee := $(shell ls | head -n 1)
Yee = $(yee)out
all: acp.c
	gcc -Wall -Wextra -std=gnu11 -o acp acp.c
debug:
	./acp $(yee) $(Yee)
clean:
	rm acp
auto: all debug clean
