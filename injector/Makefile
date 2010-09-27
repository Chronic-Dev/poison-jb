CC = gcc
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
        LDFLAGS = -L/opt/local/lib -L/usr/local/lib -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit
else
        LDFLAGS = -lusb-1.0 -lcurl -lz
endif

all:
	$(CC) -o injectpois0n *.c *.a $(LDFLAGS)
	cp injectpois0n ../staging/injectpois0n	                                                                                                                                      

clean:                                                                                                                                                                                
	rm -rf *.o injectpois0n ../staging/injectpois0n 
