build:
	gcc  -c  main.c
	gcc  -c  list.c
	gcc  -c  crcope.c
	gcc  -c  cachepolicy.c
	gcc  -c  static.c
	gcc  -c  static_list.c 
	gcc  -o sim main.o list.o crcope.o cachepolicy.o static.o static_list.o

parallel:
	gcc -O3 -c -fopenmp main.c
	gcc -O3 -c -fopenmp list.c
	gcc -O3 -c -fopenmp crcope.c
	gcc -O3 -c -fopenmp cachepolicy.c
	gcc -O3 -c -fopenmp static.c
	gcc -O3 -c -fopenmp static_list.c 
	gcc -O3 -fopenmp -o sim main.o list.o crcope.o cachepolicy.o static.o static_list.o


run2:
	./sim ../caputer/input_sample_2016.txt
run1:
	./sim ../caputer/input_sample_2009.txt

pra:
	./sim ./out/a.txt

clean:
	rm *.o
