#include "utils.h"

int cpFiles() {
	//Here's where we cp the files we want and mkdir's we need... Lets do it!
	mkdir("/mnt/Applications/Loader.app/", 755);
	if (cp("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	if (cp("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	if (cp("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	if (cp("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	if (cp("/files/Loader.app/Loader_", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	if (cp("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/") == 0) {
		return -1;
	}
	return 0;
}

int main(int argc, char **args, char **env) {
	int ret;
    int console;
    struct stat status;

    mlock(0x1000,0x2000);
    console = open("/dev/console", O_RDWR);
    dup2(console, 1);
    dup2(console, 2);

	puts("jDisk Loaded - Chronic-Dev 2010 bitches!\n\n");
	puts("Waiting for disk...\n");
	while(stat("/dev/disk0", &status) != 0) {
		puts("waiting...\n");
	}

	puts("Disk Found! Mounting...\n");
	ret = mountHFS("/dev/disk0s1", "/mnt", O_RDWR);
	if (ret == 0) {
		puts("Mounted Disk...\n");
		puts("Mounting Dev...\n");
		ret = mount("devfs", "/mnt/dev", 0, NULL);
		if (ret == 0) {
			puts("Mounted Dev!\n");
			//_puts("Opening Ramdisk...\n");
			//ret = open("/dev/rmd0", 0);
			puts("Copying nessessary files...\n");
			if (copyFiles()) {
				if (unmount("/mnt", 0)) {
					reboot();
				} else {
					puts("Damn! We were almost there too!\n");
				}
			} else {
				puts("Failed to copy files.. :(\n");
			}
		} else {
			puts("Mount Failed! :(\n");
		}
	} else {
		puts("Mount Failed! :(\n");
	}
	return 0;
}

