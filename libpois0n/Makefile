CC = gcc
AR = ar
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS=-I/usr/local/include -I/opt/local/include -c
else
	CFLAGS=-c
endif

all:
	$(CC) $(CFLAGS) libpois0n.c libirecovery.c partial.c
	$(AR) -cvq libpois0n.a libpois0n.o libirecovery.o partial.o
		
clean:
	rm -rf *.o libpois0n.a
