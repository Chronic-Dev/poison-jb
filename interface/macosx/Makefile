CFLAGS=-I../../injector
LDFLAGS=-L/opt/local/lib -L/usr/local/lib -L../../injector -lpois0n -lusb-1.0 -lcurl -lz -framework CoreFoundation -framework IOKit -framework Cocoa

all:
	echo "Building Mac OS X GUI"
	$(CC) $(CFLAGS) $(LDFLAGS) main.m appFunctions.m -o greenpois0n.app/Contents/MacOS/greenpois0n
	
clean:
	echo "Cleaning Mac OS X GUI"
	rm -rf greenpois0n.app/Contents/MacOS/greenpois0n
