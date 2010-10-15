GO_EASY_ON_ME=1
include theos/makefiles/common.mk

BUNDLE_NAME = ashyLarry
ashyLarry_FILES = ashyLarryAppliance.mm P0isonedMenuLauncherController.m GPMedia.m GPProgressBarControl.m GPDownloadController.m GPMediaPreview.mm CPlusFunctions.mm
ashyLarry_INSTALL_PATH = /Applications/Lowtide.app/Appliances
ashyLarry_BUNDLE_EXTENSION = frappliance
ashyLarry_LDFLAGS = -undefined dynamic_lookup #-L$(FW_PROJECT_DIR) -lBackRow

include $(FW_MAKEDIR)/bundle.mk