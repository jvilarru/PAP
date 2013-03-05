CC=gcc

all: matmul dotprod dotprod-basic

matmul: matmul.c
	$(CC) -pthread -I ./ -o matmul matmul.c

dotprod: dotprod.c
	$(CC) -pthread -I ./ -o dotprod dotprod.c
	
dotprod-basic: dotprod-basic.c
	$(CC) -pthread -I ./ -o dotprod-basic dotprod-basic.c

clean:
	rm -rf *.o matmul dotprod dotprod-basic
