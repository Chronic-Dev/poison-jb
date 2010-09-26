#include <sys/stat.h>

#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

#undef NULL
#define NULL 0

int install() {
	int ret = 0;
	mkdir("/mnt/Applications/Loader.app/", 755);

	ret = cp("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	ret = cp("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	ret = cp("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	ret = cp("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	ret = cp("/files/Loader.app/Loader_", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	ret = cp("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/");
	if (ret == 0) {
		return -1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	// Lets see how far this is running
	reboot(1);

	int ret = 0;
	int console = 0;
	struct stat status;
	//mlock((void*) 0x1000, 0x2000);

	stdout = open("/dev/console", O_WRONLY);
	dup2(stdout, 1);
	dup2(stdout, 2);

	puts("Pois0nDisk - by Chronic-Dev\n");
	puts("Searching for disk...\n");
	while (stat("/dev/disk0", &status) != 0) {
		puts("Searching...\n");
	}
	puts("Disk found\n");

	puts("Mounting disk...\n");
	if (hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS) != 0) {
		puts("Unable to mount filesystem!\n");
		exit(1);
	}
	puts("Disk mounted\n");

	puts("Mounting devices...\n");
	if (mount("devfs", "/dev", 0, NULL) != 0) {
		puts("Unable to mount devices!\n");
		unmount("/mnt", 0);
		exit(1);
	}
	puts("Devices mounted\n");

	puts("Installing files...\n");
	if (install() != 0) {
		puts("Failed to install files!\n");
		unmount("/mnt", 0);
		unmount("/dev", 0);
		exit(1);
	}
	puts("Files installed\n");

	puts("Rebooting device...\n");

	unmount("/mnt", 0);
	unmount("/dev", 0);
	reboot(1);
	return 0;
}
