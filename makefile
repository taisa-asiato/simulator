build:
	gcc  -c  main.c
	gcc  -c  list.c
	gcc  -c  crcope.c
	gcc  -c  cachepolicy.c
	gcc  -c  static.c
	gcc  -c  static_list.c
	gcc  -c  filter.c
	gcc  -c  flowlist.c 
	gcc  -o sim main.o list.o crcope.o cachepolicy.o static.o static_list.o filter.o flowlist.o

parallel:
	gcc -O3 -c -fopenmp main.c
	gcc -O3 -c -fopenmp list.c
	gcc -O3 -c -fopenmp crcope.c
	gcc -O3 -c -fopenmp cachepolicy.c
	gcc -O3 -c -fopenmp static.c
	gcc -O3 -c -fopenmp static_list.c 
	gcc -O3 -fopenmp -o sim main.o list.o crcope.o cachepolicy.o static.o static_list.o


sp1:
	./sim ../caputer/20160501/input_sample_20160501a.txt sp ON
lru1:
	./sim ../caputer/20160501/input_sample_20160501a.txt lru
sp2:
	./sim ../caputer/20160501/not_top100user_1packet_20160501.txt sp
lru2:
	./sim ../caputer/20160501/not_top100user_1packet_20160501.txt lru
sp3:
	./sim ../caputer/20160501/not_1packet_20160501.txt sp
lru3:
	./sim ../caputer/20160501/not_1packet_20160501.txt lru
pra:
	./sim ./out/a.txt

list:
	./sim ../caputer/20160402/input_sample_20160402.txt sp ON 100 10 1 0.01


clean:
	rm *.o
