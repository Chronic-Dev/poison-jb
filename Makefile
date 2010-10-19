all:
	mkdir -p staging
	mkdir -p injector/payloads
	mkdir -p injector/resources
	make -C tools
	make -C payload
	make -C ramdisk
	make -C injector

clean:	
	make clean -C tools
	make clean -C payload
	make clean -C ramdisk
	make clean -C injector
