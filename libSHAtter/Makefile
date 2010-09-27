CC = gcc
AR = ar
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I/usr/local/include -I/opt/local/include -c
else
	CFLAGS = -I./include -c
endif

all:
	$(CC) libSHAtter.c libirecovery.c partial.c $(CFLAGS)
	$(AR) -cvq libSHAtter.a *.o
	cp libSHAtter.a ../injector/
		
clean:
	rm -rf *.o libSHAtter.a ../injector/libSHAtter.a
