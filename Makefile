OBJS = http.o ip.o key_util.o config.o

main: main.c $(OBJS)
	gcc main.c ip.o http.o key_util.o config.o -lpcap -o main

http.o: http.h http.c
	gcc -Wall -c http.c

ip.o: ip.h ip.c
	gcc -Wall -c ip.c

key_util.o: key_util.h key_util.c
gcc -Wall -c key_util.c

config.o: config.h config.c
gcc -Wall -c config.c
