PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDKVER=4.0
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
LD=$(PLATFORM)/Developer/usr/bin/ld
AS=$(PLATFORM)/Developer/usr/bin/as
STRIP=$(PLATFORM)/Developer/usr/bin/strip


RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg

LAUNCHD_SOURCES=launchd/launchd.c launchd/utils.c launchd/syscalls.S

RAMDISK_VOLUME=pois0ndisk

CFLAGS=-arch armv6 -static -nostartfiles -nodefaultlibs -nostdlib -I./include
LDFLAGS=

UNAME := $(shell uname -s)
PWD = $(shell pwd)

ifeq ($(UNAME),Darwin)
all: ramdisk/sbin/launchd ramdisk.dmg encrypt
else
all: encrypt
endif

ramdisk/sbin/launchd:
	$(CC) $(CFLAGS) $(LDFLAGS) -o ramdisk/sbin/launchd $(LAUNCHD_SOURCES)
	#strip ramdisk/sbin/launchd
	ldid -S ramdisk/sbin/launchd

ramdisk.dmg: ramdisk/sbin/launchd
	sudo hdiutil attach ramdisk.dmg
	sudo cp -R ramdisk/* /Volumes/$(RAMDISK_VOLUME)
	sudo hdiutil detach /Volumes/$(RAMDISK_VOLUME)
	sudo chown 501:20 ramdisk.dmg

encrypt: ramdisk.dmg
	xpwntool ramdisk.dmg ramdisk.img3 -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)
	cp ramdisk.img3 ../staging/ramdisk.dmg

clean:
	rm -rf launchd/*.o
	rm -rf ramdisk.img3
	rm -rf ramdisk/sbin/launchd
