build:
	g++  -std=c++11 -c  main.cpp
	g++  -std=c++11 -c  list.cpp
	g++  -std=c++11 -c  crcope.cpp
	g++  -std=c++11 -c  cachepolicy.cpp
	g++  -std=c++11 -c  static.cpp
	g++  -std=c++11 -c  static_list.cpp
	g++  -std=c++11 -c  filter.cpp
	g++  -o sim main.o list.o crcope.o cachepolicy.o static.o static_list.o filter.o

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

clean:
	rm *.o
