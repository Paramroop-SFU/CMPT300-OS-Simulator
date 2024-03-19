all: main

main: main.o commands.o list.o
    gcc -o main main.o commands.o list.o

main.o: main.c commands.h
    gcc -c main.c

commands.o: commands.c commands.h list.h
    gcc -c commands.c

list.o: list.c list.h
    gcc -c list.c

clean:
    rm -f *.o main

