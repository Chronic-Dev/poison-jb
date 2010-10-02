all:
	mkdir -p staging
	make -C tools
	mkdir -p injector/resources
	make -C exploit
	mkdir -p injector/payloads
	make -C payload
	make -C loader
	make -C ramdisk
	make -C injector
	#make -C interface

clean:	
	make clean -C tools
	make clean -C exploit
	make clean -C payload
	make clean -C loader
	make clean -C ramdisk
	make clean -C injector
	#make clean -C interface
