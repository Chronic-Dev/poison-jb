PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS4.0.sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
STRIP=$(PLATFORM)/Developer/usr/bin/strip

RAMDISK_VOLUME=POISONJB

SOURCES=launchd.c utils.c syscalls.S

CFLAGS=-arch armv6 -isysroot $(SDK) -std=c99 -I./include
LDFLAGS=-F$(SDK)/System/Library/PrivateFrameworks \
	-framework CoreFoundation 

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
all: launchd ramdisk encrypt
	cp ramdisk.img3 ../staging/ramdisk.dmg
else
	
all: encrypt
	cp ramdisk.img3 ../staging/ramdisk.dmg
endif

launchd:
	$(CC) $(CFLAGS) -static -o launchd $(SOURCES) -nostdlib
	$(STRIP) launchd
	/opt/local/bin/ldid -S launchd
	chmod 755 launchd
	mv launchd rdfs/sbin/launchd

ramdisk: launchd
	sudo hdiutil create -srcfolder ./rdfs -align 512k -fs HFS+ -volname $(RAMDISK_VOLUME) -layout NONE -format UDRW ramdisk-img.dmg
	sudo hdiutil attach ramdisk-img.dmg
	sudo chown 0:0 /Volumes/$(RAMDISK_VOLUME)/sbin/launchd
	sudo chown 501:20 ramdisk-img.dmg
	sudo hdiutil detach /Volumes/$(RAMDISK_VOLUME)

encrypt:
	xpwntool ramdisk-img.dmg ramdisk.img3 -t 018-7074-092.dmg -iv c58929f652c1c086f70f941f3bb31058 -k 358e67475d675410517ccbfcbbc38fa4c56d0e892b627460851a1fa5e9b430ab

clean:
	#rm -f ramdisk-img.dmg launchd
