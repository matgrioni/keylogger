MAIN_OBJS = http.o ip.o process.o timed_logger.o key_util.o config.o
GHOST_OBJS = process.o

all: main ghost

timed_logger.o: timed_logger.h timed_logger.c
gcc -Wall -c timed_logger.c

key_util.o: key_util.h key_util.c
gcc -Wall -c key_util.c

config.o: config.h config.c
gcc -Wall -c config.c
