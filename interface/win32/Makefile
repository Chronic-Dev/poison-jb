UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	OS := macosx
else
	OS := linux
endif

all: wingui.c wingui.rc
	echo "Building Win32 Interface"
	/opt/local/bin/i386-mingw32-windres wingui.rc -O coff -o wingui.res
	/opt/local/bin/i386-mingw32-gcc -m32 "-DWINVER=0x0501" -I./include -std=gnu99 -o wingui.exe wingui.res wingui.c  -L./lib -I/opt/local/i386-mingw32/include -L/opt/local/i386-mingw32/lib -lwininet -lgdi32 -mwindows -lcomctl32 -lpois0n /mingw/lib/libcurl.a /mingw/lib/libwsock32.a /mingw/lib/libwldap32.a /mingw/lib/libpenwin32.a /mingw/lib/libz.a /mingw/lib/libsetupapi.a /mingw/lib/libssl32.a /mingw/lib/libcrypto.a /mingw/lib/libssh2.a /mingw/lib/libzdll.a /mingw/lib/libidn.a
	


clean:
	echo "Cleaning Win32 Interface"

