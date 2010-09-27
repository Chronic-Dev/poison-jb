#include <sys/stat.h>

#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

#undef NULL
#define NULL 0

int install() {
	int ret = 0;

	puts("Creating directory for install\n");
	mkdir("/mnt/Applications/Loader.app", 755);

	puts("Installing Bootstrap\n");
	ret = cp("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/Bootstrap");
	if (ret < 0) {
		return -1;
	}

	puts("Installing cydia@2x.png\n");
	ret = cp("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/cydia@2x.png");
	if (ret < 0) {
		return -1;
	}

	puts("Installing cydia.png\n");
	ret = cp("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/cydia.png");
	if (ret < 0) {
		return -1;
	}

	puts("Installing Info.plist\n");
	ret = cp("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/Info.plist");
	if (ret < 0) {
		return -1;
	}

	puts("Installing Loader_\n");
	ret = cp("/files/Loader.app/Loader_", "/mnt/Applications/Loader.app/Loader_");
	if (ret < 0) {
		return -1;
	}
	chown("/mnt/Applications/Loader.app/Loader_", 0, 0);
	chmod("/mnt/Applications/Loader.app/Loader_", 6755);

	puts("Installing PkgInfo\n");
	ret = cp("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/PkgInfo");
	if (ret < 0) {
		return -1;
	}

	puts("Installing fstab\n");
	ret = cp("/files/fstab", "/mnt/private/etc/fstab");
	if (ret < 0) {
		return -1;
	}

	puts("Installing Services.plist\n");
	ret = cp("/files/Services.plist", "/mnt/System/Library/Lockdown/Services.plist");
	if (ret < 0) {
		return -1;
	}

	puts("Installation complete\n");
	return 0;
}

int main(int argc, char* argv[]) {
	int ret = 0;
	int console = 0;
	struct stat status;
	//mlock((void*) 0x1000, 0x2000);

	stdout = open("/dev/console", O_WRONLY);
	dup2(stdout, 1);
	dup2(stdout, 2);

	puts("Pois0nDisk - by Chronic-Dev\n");
	puts("Searching for disk...\n");
	while (stat("/dev/disk0s1", &status) != 0) {
		sleep(1);
		puts("Searching...\n");
	}
	puts("Disk found\n");

	puts("Mounting disk...\n");
	if (hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS) != 0) {
		puts("Unable to mount filesystem!\n");
		exit(1);
	}
	puts("Disk mounted\n");

	puts("Installing files...\n");
	if (install() != 0) {
		puts("Failed to install files!\n");
		unmount("/mnt", 0);
		exit(1);
	}
	puts("Files installed\n");

	puts("Unmounting disk...");
	unmount("/mnt", 0);

	puts("Flushing buffers...\n");
	sync();

	puts("Rebooting device...\n");
	reboot(1);
	return 0;
}
