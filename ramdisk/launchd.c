#include "utils.h"

int copyFiles() {
	//Here's where we copy the files we want and mkdir's we need... Lets do it!
	mkdir("/mnt/Applications/Loader.app/", 755);
	int ret = copy("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	ret = copy("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	ret = copy("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	ret = copy("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	ret = copy("/files/Loader.app/Loader_", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	ret = copy("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/");
	if (ret == 0)
		return 1;
	return 0;
}

int main(int argc, char **args, char **env) {
        struct stat status;
        int console;
	int ret;
        mlock(0x1000,0x2000);

        console = open("/dev/console", O_WRONLY);
        dup2(console, 1);
        dup2(console, 2);

	_puts("jDisk Loaded - Chronic-Dev 2010 bitches!\n\n");
	_puts("Waiting for disk...\n");
	while(stat("/dev/disk0", &status) != 0) {
		_puts("waiting...\n");
	}

	_puts("Disk Found! Mounting...\n");
	ret = mountHFS("/dev/disk0s1", "/mnt", MNT_ROOTFS);
	if (ret == 0) {
		_puts("Mounted Disk...\n");
		_puts("Mounting Dev...\n");
		ret = mount("devfs", "/mnt/dev", 0, NULL);
		if (ret == 0) {
			_puts("Mounted Dev!\n");
			//_puts("Opening Ramdisk...\n");
			//ret = open("/dev/rmd0", 0);
			_puts("Copying nessessary files...\n");
			if (copyFiles()) {
				if (unmount("/mnt", 0)) {
					reboot();
				} else {
					_puts("Damn! We were almost there too!\n");
				}
			} else {
				_puts("Failed to copy files.. :(\n");
			}
		} else {
			_puts("Mount Failed! :(\n");
		}
	} else {
		_puts("Mount Failed! :(\n");
	}
	return 0;
}

