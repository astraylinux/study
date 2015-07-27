#libs
CUR_PATH = $(shell pwd)


test.bin: rbtree.o test.c
	gcc -g test.c rbtree.o -o test.bin

rbtree.o: rbtree.c
	gcc -g -c rbtree.c -o rbtree.o

clean:
	rm rbtree.o test.bin
