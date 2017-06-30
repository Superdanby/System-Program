all:
	gcc -Wall -Wextra -o mypipe mypipe.c -std=gnu11
run:
	./mypipe cat ./mypipe.c wc -c
clean:
	rm ./mypipe
auto: all run clean
