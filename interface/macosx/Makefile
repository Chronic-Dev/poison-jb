CFLAGS=-I../../injector -Igreenpois0n.app/Contents/Frameworks
LDFLAGS=-Lgreenpois0n.app/Contents/Frameworks -lpois0n -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit -framework Cocoa

all:
	@echo "Building Mac OS X GUI"
	cp ../../injector/libpois0n.a greenpois0n.app/Contents/Frameworks
	mkdir -p greenpois0n.app/Contents/MacOS
	$(CC) $(CFLAGS) $(LDFLAGS) main.m appFunctions.m -o greenpois0n.app/Contents/MacOS/greenpois0n
	install_name_tool -change /Users/nicolas/Downloads/libusb-1.0.8/build/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.dylib greenpois0n.app/Contents/MacOS/greenpois0n
	
clean:
	@echo "Cleaning Mac OS X GUI"
	rm -rf greenpois0n.app/Contents/MacOS/greenpois0n
