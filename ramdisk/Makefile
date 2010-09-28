init: ramdisk.img3

include theos/makefiles/common.mk
SUBPROJECTS = src
include $(FW_MAKEDIR)/aggregate.mk

RAMDISK_IV=4445214ed192b6ccfe6953c6ae80c55e
RAMDISK_KEY=7f50d391295747a06d03cdb2ae536fa089a368e473a470148f44750712c58105
RAMDISK_TEMPLATE=018-7074-092.dmg
RAMDISK_LABEL=pois0ndisk
RAMDISK_VOLUME=/Volumes/$(RAMDISK_LABEL)

ramdisk.dmg: stage
	#-@rm ramdisk.dmg
	#hdiutil create -srcfolder $(FW_STAGING_DIR) -align 8 -fs HFS+ -volname $(RAMDISK_LABEL) -layout NONE -format UDRW ramdisk.dmg -ov
	cp ramdisk-blank.dmg $@
	sudo hdiutil attach $@
	sudo rsync -a $(FW_STAGING_DIR)/ $(RAMDISK_VOLUME)/
	sudo sync
	sudo hdiutil detach $(RAMDISK_VOLUME)

ramdisk.img3: ramdisk.dmg
	xpwntool $< $@ -k $(RAMDISK_KEY) -iv $(RAMDISK_IV) -t $(RAMDISK_TEMPLATE)
	cp $@ ../staging/ramdisk.dmg

clean::
	rm -rf ramdisk.dmg ramdisk.img3
