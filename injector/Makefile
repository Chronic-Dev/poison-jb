CC = gcc
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I/usr/local/include -I/opt/local/include
	LDFLAGS = -L/opt/local/lib -L/usr/local/lib -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit
else
	CFLAGS = -I./include
	LDFLAGS = -lusb-1.0 -lcurl -lz
endif

all:
	$(CC) -o injectpois0n injectpois0n.c libirecovery.c partial.c $(CFLAGS) $(LDFLAGS)
	cp injectpois0n ../staging/injectpois0n
		
clean:
	rm -rf *.o injectpois0n ../staging/injectpois0n
