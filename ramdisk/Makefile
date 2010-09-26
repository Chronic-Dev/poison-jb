PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDKVER=4.0
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc
LD=$(PLATFORM)/Developer/usr/bin/ld
AS=$(PLATFORM)/Developer/usr/bin/as
STRIP=$(PLATFORM)/Developer/usr/bin/strip


RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg

LAUNCHD_SOURCES=src/launchd.c src/utils.c src/syscalls.S

RAMDISK_VOLUME=pois0ndisk

RAMDISK=/Volumes/$(RAMDISK_VOLUME)

CFLAGS=-arch armv6 -isysroot=$(SDK) -static -nostdlib -I./include -I$(SDK)/usr/include
LDFLAGS=

UNAME := $(shell uname -s)
PWD = $(shell pwd)

ifeq ($(UNAME),Darwin)
all: launchd ramdisk encrypt
else
all: encrypt
endif

launchd:
	$(CC) $(CFLAGS) $(LDFLAGS) -o stage/sbin/launchd $(LAUNCHD_SOURCES)
	#strip stage/sbin/launchd
	ldid -S stage/sbin/launchd

ramdisk:
	sudo hdiutil attach ramdisk.dmg
	#mkdir $(RAMDISK)/mnt
	#mkdir $(RAMDISK)/dev
	sudo rm -rf $(RAMDISK)/sbin/launchd
	sudo cp stage/sbin/launchd $(RAMDISK)/sbin/launchd
	sudo chown 0:0 $(RAMDISK)/sbin/launchd
	sudo chmod 755 $(RAMDISK)/sbin/launchd
	sync
	sudo hdiutil detach $(RAMDISK)

encrypt:
	xpwntool ramdisk.dmg ramdisk.img3 -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)
	cp ramdisk.img3 ../staging/ramdisk.dmg

clean:
	rm -rf src/*.o
	rm -rf ramdisk.img3
	rm -rf stage/sbin/launchd
