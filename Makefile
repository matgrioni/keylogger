OBJS = http.o ip.o process.o timed_logger.o key_util.o config.o client.o
MAIN_OBJS = http.o ip.o process.o timed_logger.o key_util.o config.o client.o
GHOST_OBJS = process.o

all: main ghost server

main: main.c $(MAIN_OBJS)
	gcc -Wall main.c $(MAIN_OBJS) -o main -lpthread -lpcap

ghost: ghost.c $(GHOST_OBJS)
	gcc -Wall ghost.c $(GHOST_OBJS) -o ghost -lpthread

server: server.c
	gcc -Wall server.c -o server

process.o: process.c process.h
	gcc -Wall -c process.c

http.o: http.h http.c
	gcc -Wall -c http.c

ip.o: ip.h ip.c
	gcc -Wall -c ip.c

timed_logger.o: timed_logger.h timed_logger.c
	gcc -Wall -c timed_logger.c

key_util.o: key_util.h key_util.c
	gcc -Wall -c key_util.c

config.o: config.h config.c
	gcc -Wall -c config.c

client.o: client.c
	gcc -Wall -c client.c

clean:
	rm main ghost server $(OBJS)
