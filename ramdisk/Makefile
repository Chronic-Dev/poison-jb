PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDKVER=4.1
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
LD=$(PLATFORM)/Developer/usr/bin/ld
AS=$(PLATFORM)/Developer/usr/bin/as
BIN2C=../tools/bin2c
STRIP=$(PLATFORM)/Developer/usr/bin/strip


RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg

LAUNCHD_SOURCES=src/launchd.c src/utils.c src/syscalls.S

RAMDISK_LABEL=pois0ndisk
RAMDISK_VOLUME=/Volumes/$(RAMDISK_LABEL)

CFLAGS=-arch armv6 -isysroot=$(SDK) -static -nostartfiles -nodefaultlibs -nostdlib -I./include -I$(SDK)/usr/include -Wl,-e,_main
LDFLAGS=

UNAME := $(shell uname -s)
PWD = $(shell pwd)

ifeq ($(UNAME),Darwin)
all: launchd ramdisk encrypt
else
all: encrypt
endif

launchd:
	$(CC) $(CFLAGS) $(LDFLAGS) -o src/launchd $(LAUNCHD_SOURCES)
	#strip src/launchd
	ldid -S src/launchd

ramdisk:
	cp template.dmg ramdisk.dmg
	hdiutil attach ramdisk.dmg
	rm -rf $(RAMDISK_VOLUME)/sbin/launchd
	cp src/launchd $(RAMDISK_VOLUME)/sbin/launchd
	cp -R files/* $(RAMDISK_VOLUME)/files/
	#chown 0:0 $(RAMDISK_VOLUME)/sbin/launchd
	#chmod 755 $(RAMDISK_VOLUME)/sbin/launchd
	sync
	hdiutil detach $(RAMDISK_VOLUME)

encrypt:
	xpwntool ramdisk.dmg ramdisk.img3 -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)
	$(BIN2C) ramdisk.img3 ../injector/resources/ramdisk.h ramdisk

clean:
	rm -rf src/*.o
	rm -rf ramdisk.img3
	rm -rf stage/sbin/launchd
