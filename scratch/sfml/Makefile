CC= g++
COPTS= -Wall -O3

all: main

main: main.o
	$(CC) main.o -o main -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	$(CC) -c main.cpp

clean:
	rm *.o main
