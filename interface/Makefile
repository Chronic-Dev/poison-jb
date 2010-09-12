UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	OS := macosx
else
	OS := linux
endif

all:
	make -C $(OS)

	
clean:
	make clean -C $(OS)