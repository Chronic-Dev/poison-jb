all:
	mkdir -p staging
	mkdir -p injector/payloads
	mkdir -p injector/resources
	make -C tools
	make -C payload
	make copy -C loader
	make -C ramdisk
	make -C injector
	make -C interface

clean:	
	make clean -C tools
	make clean -C payload
	make clean -C loader
	make real-clean -C loader
	make clean -C ramdisk
	make clean -C injector
	make clean -C interface
