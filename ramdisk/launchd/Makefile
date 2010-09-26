SDKVERSION=4.0
TARGET=iphone
GO_EASY_ON_ME=1
include ../theos/makefiles/common.mk

TOOL_NAME = launchd
launchd_CFLAGS = -static -I../include
launchd_LDFLAGS = -static -nostdlib -lgcc
launchd_FILES = launchd.c utils.c syscalls.S
launchd_INSTALL_PATH = /sbin

include $(FW_MAKEDIR)/tool.mk
