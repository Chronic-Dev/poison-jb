UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	OS := macosx
else
	OS := linux
endif

all:
	echo "Building Mac OS X GUI"
	$(CC) $(CFLAGS) $(LDFLAGS) main.m appFunctions.m -framework Cocoa -o greenpois0n.app/Contents/MacOS/greenpois0n
	
clean:
	echo "Cleaning Mac OS X GUI"
	rm greenpois0n.app/Contents/MacOS/greenpois0n
