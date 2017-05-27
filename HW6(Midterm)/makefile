all: RecursiveInotify.c
	gcc -Wall -Wextra -std=gnu11 -o RecursiveInotify RecursiveInotify.c
debug1:
	./RecursiveInotify ./ ../
debug2:
	./RecursiveInotify ../../
clean:
	rm ./RecursiveInotify
auto: all debug1
