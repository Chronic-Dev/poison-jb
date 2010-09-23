PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDKVER=4.0
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
STRIP=$(PLATFORM)/Developer/usr/bin/strip


RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg

LAUNCHD_SOURCES=launchd/launchd.c launchd/syscalls.S launchd/utils.c  

RAMDISK_VOLUME=GREEND1SK

CFLAGS=-arch armv6 -isysroot $(SDK) -std=c99 -I./include
LDFLAGS=-c -F$(SDK)/System/Library/PrivateFrameworks \
	-framework CoreFoundation \

UNAME := $(shell uname -s)
PWD = $(shell pwd)

ifeq ($(UNAME),Darwin)
all: ramdisk/sbin/launchd ramdisk.dmg encrypt
else
all: encrypt
endif

ramdisk/sbin/launchd:
	$(CC) $(CFLAGS) -static -o ramdisk/sbin/launchd $(LAUNCHD_SOURCES) -nostdlib
	strip ramdisk/sbin/launchd
	ldid -S ramdisk/sbin/launchd

ramdisk.dmg: launchd
	sudo hdiutil create -srcfolder ./ramdisk -align 512k -fs HFSX -volname $(RAMDISK_VOLUME) -layout NONE -format UDRW ramdisk.dmg 
	sudo hdiutil attach ramdisk.dmg
	sudo chown root:wheel $(PWD)/ramdisk/sbin/launchd
	sudo hdiutil detach $(PWD)/ramdisk/sbin/launchd
	sudo mv ramdisk.dmg ramdisk.hfs

encrypt:
	xpwntool ramdisk.hfs ramdisk.dmg -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)

clean:
	rm -rf ramdisk.dmg
	rm -rf ramdisk/sbin/launchd
