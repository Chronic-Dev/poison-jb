#include <sys/stat.h>

#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

#undef NULL
#define NULL 0

// comment this out if you are posixninja ;)
#define INSTALL_LOADER

int install_files() {
	int ret = 0;

#ifdef INSTALL_LOADER
	puts("Installing Loader directory...\n");
	mkdir("/mnt/Applications/Loader.app", 0755);
	chmod("/mnt/Applications/Loader.app", 0755);
	
	puts("Installing Loader Bootstrap...\n");
	ret = install("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/Bootstrap", 0, 80, 0755);
	if (ret < 0) return ret;
	
	puts("Installing Loader Resource: cydia.png...\n");
	ret = install("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/cydia.png", 0, 80, 0755);
	if (ret < 0) return ret;
	
	puts("Installing Loader Resource: cydia@2x.png...\n");
	ret = install("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/cydia@2x.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: Info.plist...\n");
	ret = install("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/Info.plist", 0, 80, 0755);
	if (ret < 0) return ret;
	
	puts("Installing Loader Resource: PkgInfo\n");
	ret = install("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/PkgInfo", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader binary...\n");
	ret = install("/files/Loader.app/Loader", "/mnt/Applications/Loader.app/Loader", 0, 80, 06755);
	if (ret < 0) return ret;
#endif
	
	if(access("/mnt/System/Library/CoreServices/SpringBoard.app/K48AP.plist", 0) == 0) {
	   puts("Installing patched K48AP.plist...\n");
	   ret = install("/files/K48AP.plist", "/mnt/System/Library/CoreServices/SpringBoard.app/K48AP.plist", 0, 80, 0755);
	   if (ret < 0) return ret;
	}
	
	puts("Installing patched fstab...\n");
	ret = cp("/files/fstab", "/mnt/private/etc/fstab");
	if (ret < 0) return -1;

	puts("Installing patched Services.plist...\n");
	ret = cp("/files/Services.plist", "/mnt/System/Library/Lockdown/Services.plist");
	if (ret < 0) return -1;

	return 0;
}

int main(int argc, char* argv[]) {
	int ret = 0;
	int console = 0;
	struct stat status;

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
		return -1;
	}
	puts("Disk mounted\n");

	puts("Installing files...\n");
	if (install_files() != 0) {
		puts("Failed to install files!\n");
		unmount("/mnt", 0);
		return -1;
	}
	puts("Installation complete\n");

	puts("Unmounting disk...\n");
	unmount("/mnt", 0);

	puts("Flushing buffers...\n");
	sync();

	puts("Rebooting device...\n");
	close(stdout);
	reboot(1);
	return 0;
}
