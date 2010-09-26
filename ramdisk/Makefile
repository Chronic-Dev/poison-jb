init: ramdisk.dmg

include theos/makefiles/common.mk
SUBPROJECTS = launchd
include $(FW_MAKEDIR)/aggregate.mk

RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg
RAMDISK_VOLUME=POIS0N

ramdisk.hfs: stage
	sudo hdiutil create -srcfolder $(FW_STAGING_DIR) -megabytes 1 -align 8 -fs HFS+ -volname $(RAMDISK_VOLUME) -layout NONE -format UDRW ramdisk.dmg 
	sudo hdiutil attach ramdisk.dmg
	sudo hdiutil detach /Volumes/$(RAMDISK_VOLUME)
	#sudo chown -R 501:20 ramdisk/
	sudo chown 501:20 ramdisk.dmg
	mv ramdisk.dmg ramdisk.hfs

ramdisk.dmg: ramdisk.hfs
	xpwntool $< $@ -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)

clean::
	rm -rf ramdisk.dmg ramdisk.hfs
