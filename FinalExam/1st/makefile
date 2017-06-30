all: yx2.c
	gcc ./yx248.c -o yx248 -pthread -std=gnu11
run:
	time ./yx248 2147483647 8
clean:
	rm ./yx248
auto: all run clean
no48: yx2.c
	gcc ./yx2.c -o yx2 -pthread -std=gnu11
	time ./yx2 2147483647 8
	rm ./yx2
