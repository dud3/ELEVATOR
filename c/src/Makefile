# generated binary name
BINARY=elevator

# compiler info: which compiler to use and flags
CC=mpicc
CCFLAGS=-std=c99 -Wall -Werror

# build rules: the actual build rules for the program. Do not call locally.
all: logfiles.o dlist.o main.o
	mkdir -p ../bin/ && $(CC) $(CCFLAGS) main.o logfiles.o dlist.o -o ../bin/$(BINARY)
	cp *.sh ../bin/
	cp *.txt ../bin

main.o: main.c 
	$(CC) $(CCFLAGS) -c main.c

dlist.o: dlist.c
	$(CC) $(CCFLAGS) -c dlist.c

logfiles.o: logfiles.c
	$(CC) $(CCFLAGS) -c logfiles.c

