PLATFORM=/Developer/Platforms/iPhoneOS.platform
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS4.0.sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
STRIP=$(PLATFORM)/Developer/usr/bin/strip

RAMDISK_VOLUME=POISONJB

SOURCES=launchd.c utils.c syscalls.S

CFLAGS=-arch armv6 -isysroot $(SDK) -std=c99 -I./include
LDFLAGS=-F$(SDK)/System/Library/PrivateFrameworks \
	-framework CoreFoundation 

all: launchd

launchd:
	$(CC) $(CFLAGS) -static -o launchd $(SOURCES) -nostdlib
	$(STRIP) launchd
	/opt/local/bin/ldid -S launchd
	chmod 755 launchd
	mv launchd rdfs/sbin/launchd

ramdisk: launchd
	hdiutil create -srcfolder ./rdfs -align 512k -fs HFS+ -volname $(RAMDISK_VOLUME) -layout NONE -format UDRW ramdisk-img.dmg
	hdiutil attach ramdisk-img.dmg
	sudo chown 0:0 /Volumes/$(RAMDISK_VOLUME)/sbin/launchd
	hdiutil detach /Volumes/$(RAMDISK_VOLUME)

encrypt:
	xpwntool ramdisk-img.dmg ramdisk.dmg -t template-ipt2g4.0.dmg -iv b2b8949bd33b65f22bd0c7e55ccc836b -k 1e3e144a55fa8d1782db5153796238f0

clean:
	rm -f ramdisk-img.dmg
