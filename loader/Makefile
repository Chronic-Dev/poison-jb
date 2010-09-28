include theos/makefiles/common.mk

APPLICATION_NAME = Loader
TOOL_NAME = Bootstrap
Loader_FILES = main.m Callback.m MemLoaderOperation.m LoaderAppDelegate.mm LoaderVC.m
Bootstrap_FILES = Bootstrap.m
Bootstrap_INSTALL_PATH = /files/Loader.app
Loader_INSTALL_PATH = /files

include $(FW_MAKEDIR)/application.mk
include $(FW_MAKEDIR)/tool.mk

after-Loader-stage::
	$(FAKEROOT) chmod 6755 $(FW_SHARED_BUNDLE_RESOURCE_PATH)/Loader
