all: s-talk

s-talk: s-talk.o list.o
	gcc -o s-talk s-talk.o list.o -pthread

s-talk.o: s-talk.c list.h
	gcc -c s-talk.c -Wall -pthread

list.o: list.c list.h
	gcc -c list.c -Wall -pthread

clean:
	rm -f s-talk s-talk.o list.o
