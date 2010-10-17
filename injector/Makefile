CC = gcc
AR = ar
CP = cp
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as
OBJCOPY = arm-elf-objcopy
BIN2C = ../tools/bin2c
UNAME := $(shell uname -s)
ADDOBJ = ""

ifeq ($(UNAME),Darwin)
	CFLAGS = -I./include -I./resources -I/usr/local/include -I/opt/local/include -mmacosx-version-min=10.5 -arch i386 -arch x86_64 -mmacosx-version-min=10.5 -arch i386 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.5.sdk
	LDFLAGS = -L/usr/lib -L/Users/posixninja/Documents/Workspace/greenpois0n2/interface/macosx/greenpois0n.app/Contents/Frameworks -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit -mmacosx-version-min=10.5 -arch i386 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.5.sdk -Wl,-no_compact_linkedit
	ADDOBJ = 
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
	$(ARMAS) -mthumb -o limera1n.o limera1n.S
	$(OBJCOPY) -O binary limera1n.o limera1n.bin
	$(BIN2C) limera1n.bin limera1n.h limera1n
	
	$(ARMAS) -mthumb -o steaks4uce.o steaks4uce.S
	$(OBJCOPY) -O binary steaks4uce.o steaks4uce.bin
	$(BIN2C) steaks4uce.bin steaks4uce.h steaks4uce
	
	$(ARMCC) -mcpu=arm10tdmi -s -Ttext=0x8402f7a0 -nostdlib -o shatter.o shatter.S
	$(ARMAS) -mthumb -o shatter2.o shatter2.S
	$(OBJCOPY) -O binary  shatter.o shatter.bin
	$(OBJCOPY) -O binary  shatter2.o shatter2.bin
	$(DD) if=shatter.bin of=shatter.dfu bs=1 seek=179200 conv=notrunc
	$(DD) if=shatter2.bin of=shatter.dfu bs=1 seek=179328 conv=notrunc
	$(BIN2C) shatter.dfu shatter.h shatter
	
	$(BIN2C) ../payload/bundles/iPad1,1/7B500/iBoot/payload payloads/iBoot.k48ap.h iBoot_k48ap
	$(CC) $(CFLAGS) -c libpois0n.c libirecovery.c libpartial.c
	$(AR) rs libpois0n.a libpois0n.o libirecovery.o libpartial.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o injectpois0n injectpois0n.c libpois0n.a $(ADDOBJ)
	$(CP) injectpois0n ../staging/injectpois0n
		
clean:
	rm -rf *.o payloads resources libpois0n.a injectpois0n
	rm -rf limera1n.h limera1n.bin 
	rm -rf steaks4uce.h steaks4uce.bin 
	rm -rf shatter.h shatter.bin shatter2.bin
