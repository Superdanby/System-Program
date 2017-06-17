all: pi.c
	gcc ./pi.c -o pi -pthread
run:
	time ./pi 2147483647 16
clean:
	rm ./pi
auto: all run clean
