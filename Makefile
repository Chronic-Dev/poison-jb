all:
	mkdir -p staging
	make -C exploit
	make -C payload
	#make -C ramdisk
	make -C injector
	#make -C interface

	
clean:	
	make clean -C exploit
	make clean -C payload
	#make clean -C ramdisk
	make clean -C injector
	#make clean -C interface
	rm -rf staging
