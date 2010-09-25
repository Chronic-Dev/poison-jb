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

RAMDISK_VOLUME=POIS0N

CFLAGS=-arch armv6 -isysroot $(SDK) -static -I./include -nostdlib -lgcc
LDFLAGS=

UNAME := $(shell uname -s)
PWD = $(shell pwd)

ifeq ($(UNAME),Darwin)
all: ramdisk/sbin/launchd ramdisk.dmg encrypt
else
all: encrypt
endif

ramdisk/sbin/launchd:
	#$(CC) $(CFLAGS) -c launchd/syscalls.S -o launchd/syscalls.o
	#$(CC) $(CFLAGS) -c launchd/utils.c -o launchd/utils.o
	#$(CC) $(CFLAGS) -c launchd/launchd.c -o launchd/launchd.o
	#$(CC) $(CFLAGS) launchd/launchd.o launchd/utils.o launchd/syscalls.o -o ramdisk/sbin/launchd
	$(CC) $(CFLAGS) $(LDFLAGS) -o ramdisk/sbin/launchd $(LAUNCHD_SOURCES)
	#strip ramdisk/sbin/launchd
	ldid -S ramdisk/sbin/launchd

ramdisk.dmg: launchd
	#sudo chown -R 0:0 ramdisk/
	sudo hdiutil create -srcfolder ./ramdisk -megabytes 1 -align 8 -fs HFS+ -volname $(RAMDISK_VOLUME) -layout NONE -format UDRW ramdisk.dmg 
	sudo hdiutil attach ramdisk.dmg
	sudo hdiutil detach /Volumes/$(RAMDISK_VOLUME)
	#sudo chown -R 501:20 ramdisk/
	sudo chown 501:20 ramdisk.dmg
	cp ramdisk.dmg ramdisk.hfs

encrypt:
	xpwntool ramdisk.hfs ramdisk.dmg -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)

clean:
	rm -rf launchd/*.o
	rm -rf ramdisk.dmg
	rm -rf ramdisk/sbin/launchd
