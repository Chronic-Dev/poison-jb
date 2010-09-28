all:
	mkdir -p staging
	make -C exploit
	make -C payload
	#make -C loader
	make -C ramdisk
	make -C libpois0n
	make -C injector
	#make -C interface

clean:	
	make clean -C exploit
	make clean -C payload
	#make clean -C loader
	make clean -C ramdisk
	make clean -C libpois0n
	make clean -C injector
	#make clean -C interface
