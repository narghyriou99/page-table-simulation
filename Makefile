OBJS	= main.o helping.o
SOURCE	= main.c helping.c
HEADER	= helping.h
OUT	= simulator.out
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

helping.o: helping.c
	$(CC) $(FLAGS) helping.c 


clean:
	rm -f $(OBJS) $(OUT)

run1: $(OUT)
	./$(OUT) LRU 100 250 20000

run2: $(OUT)
	./$(OUT) SCA 100 250 20000
