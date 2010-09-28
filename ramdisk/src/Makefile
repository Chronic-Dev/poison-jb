# Use ?= below to not trample the user's environment if he wants something different.
SDKVERSION?=4.0
TARGET=iphone
GO_EASY_ON_ME=1 # Do not error out on warnings.
OPTFLAG = -O0 # This is required, because Theos defaults to -O2, which optimizes away our sleep() loop.
include ../theos/makefiles/common.mk

TOOL_NAME = launchd
launchd_CFLAGS = -static -nostartfiles -nodefaultlibs -nostdlib -I../include
launchd_LDFLAGS = $(launchd_CFLAGS) -Wl,-e,_main
launchd_FILES = launchd.c utils.c syscalls.S
launchd_INSTALL_PATH = /sbin

include $(FW_MAKEDIR)/tool.mk
