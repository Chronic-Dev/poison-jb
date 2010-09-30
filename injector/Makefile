CC = gcc
AR = ar
CP = cp
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -g -I./include -I./resources -I/usr/local/include -I/opt/local/include
	LDFLAGS = -L/opt/local/lib -L/usr/local/lib -lcurl -lz -framework CoreFoundation -framework IOKit
	USB_BACKEND = iokit
else
	CFLAGS = -g -I./include -I./resources
	LDFLAGS = -lusb-1.0 -lcurl -lz
	USB_BACKEND = libusb1
endif

all:
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c $(USB_BACKEND).c
	$(AR) rs libpois0n.a libpois0n.o libirecovery.o libpartial.o $(USB_BACKEND).o
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a
	$(CP) injectpois0n ../staging/injectpois0n
		
clean:
	rm -rf *.o payloads resources libpois0n.a injectpois0n
