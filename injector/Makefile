CC = gcc
AR = ar
CP = cp
AS = arm-elf-as
OBJCOPY = arm-elf-objcopy
BIN2C = ../tools/bin2c
UNAME := $(shell uname -s)
ADDOBJ = ""

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I./resources -I/usr/local/include -I/opt/local/include
	LDFLAGS = -L/opt/local/lib -L/usr/local/lib -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit
else
	ifeq ($(UNAME),MINGW32_NT-5.1)
		CFLAGS = -O3 -I./resources -DCURL_STATICLIB
		LDFLAGS = -lcurl -lz
		ADDOBJ = /mingw/lib/libcurl.a /mingw/lib/libwsock32.a /mingw/lib/libwldap32.a /mingw/lib/libpenwin32.a /mingw/lib/libz.a /mingw/lib/libsetupapi.a
	else
		CFLAGS = -O3 -I./resources
		LDFLAGS = -lusb-1.0 -lcurl -lz
		ADDOBJ = 
	endif
endif

all:
	$(AS) -mthumb -o limera1n.o limera1n.S
	$(OBJCOPY) -O binary limera1n.o limera1n.bin
	$(BIN2C) limera1n.bin limera1n.h limera1n
	
	$(AS) -mthumb -o steaks4uce.o steaks4uce.S
	$(OBJCOPY) -O binary steaks4uce.o steaks4uce.bin
	$(BIN2C) steaks4uce.bin steaks4uce.h steaks4uce
	
	$(BIN2C) ../payload/bundles/iPad1,1/7B500/iBoot/payload payloads/iBoot.k48ap.h iBoot_k48ap
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c
	$(AR) rs libpois0n.a libpois0n.o libirecovery.o libpartial.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a $(ADDOBJ)
	$(CP) injectpois0n ../staging/injectpois0n
		
clean:
	rm -rf *.o payloads resources libpois0n.a injectpois0n
