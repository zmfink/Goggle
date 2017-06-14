# Makefile

CC = g++
LD = g++

CFLAGS = -std=c++11 -pedantic -Wall -c
PAGERANK_LFLAGS = -ldl -lpthread

PAGERANK_OBJS = eecs485pa6p.o

PAGERANK = eecs485pa6p

default: $(PAGERANK)

debug: CFLAGS += -g
debug: PAGERANK_LFLAGS += -g
debug: $(PAGERANK)

$(PAGERANK): $(PAGERANK_OBJS)
	$(LD) $(PAGERANK_OBJS) -o $(PAGERANK) $(PAGERANK_LFLAGS)

eecs485pa5p.o: eecs485pa6p.cpp eecs485pa6p.h
	$(CC) $(CFLAGS) eecs485pa6p.cpp

clean:
	rm -rf *.o $(PAGERANK)
