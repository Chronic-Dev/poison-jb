#include <sys/stat.h>
#include <sys/wait.h>

#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

#define INSTALL_LOADER
//#define INSTALL_HACKTIVATION
#define INSTALL_UNTETHERED

const char* fsck_hfs[] = { "/sbin/fsck_hfs", "-fy", "/dev/rdisk0s1", NULL };
const char* patch_dyld[] = { "/usr/bin/patch", "-C", "/System/Library/Caches/com.apple.dyld/dyld_shared_cache_armv7", NULL };
const char* patch_kernel[] = { "/usr/bin/patch", "-K", "/System/Library/Caches/com.apple.kernelcaches/kernelcache", NULL };

static char** envp = NULL;

int install_files() {
	int ret = 0;

	puts("Creating directories for install\n");
	mkdir("/mnt/private", 0755);
	mkdir("/mnt/private/etc", 0755);
	mkdir("/mnt/private/var", 0755);
	mkdir("/mnt/private/var/db", 0755);
	mkdir("/mnt/Applications/Loader.app", 0755);

	puts("Installing fstab\n");
	ret = cp("/files/fstab", "/mnt/private/etc/fstab");
	if (ret < 0) return -1;

	puts("Installing Services.plist\n");
	ret = cp("/files/Services.plist", "/mnt/System/Library/Lockdown/Services.plist");
	if (ret < 0) return -1;

#ifdef INSTALL_HACKTIVATION
	puts("Installing hacktivate.dylib...\n");
	ret = install("/files/hacktivate.dylib", "/mnt/usr/lib/hacktivate.dylib", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing patched com.apple.mobile.lockdown.plist...\n");
	ret = install("/files/com.apple.mobile.lockdown.plist", "/mnt/System/Library/LaunchDaemons/com.apple.mobile.lockdown.plist", 0, 0, 0644);
	if (ret < 0) return ret;
#endif

#ifdef INSTALL_LOADER
	puts("Installing Bootstrap\n");
	ret = install("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/Bootstrap", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader binary\n");
	ret = install("/files/Loader.app/Loader", "/mnt/Applications/Loader.app/Loader", 0, 80, 06755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: cydia.png\n");
	ret = install("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/cydia.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: cydia@2x.png\n");
	ret = install("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/cydia@2x.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: Info.plist\n");
	ret = install("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/Info.plist", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: icon.png\n");
	ret = install("/files/Loader.app/icon.png", "/mnt/Applications/Loader.app/icon.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: icon@2x.png\n");
	ret = install("/files/Loader.app/icon@2x.png", "/mnt/Applications/Loader.app/icon@2x.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: icon-ipad.png\n");
	ret = install("/files/Loader.app/icon-ipad.png", "/mnt/Applications/Loader.app/icon-ipad.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader Resource: PkgInfo\n");
	ret = install("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/PkgInfo", 0, 80, 0755);
	if (ret < 0) return ret;
#endif

	if(access("/mnt/System/Library/CoreServices/SpringBoard.app/K48AP.plist", 0) == 0) {
	   puts("Installing patched K48AP.plist\n");
	   ret = install("/files/K48AP.plist", "/mnt/System/Library/CoreServices/SpringBoard.app/K48AP.plist", 0, 80, 0755);
	   if (ret < 0) return ret;
	}

#ifdef INSTALL_UNTETHERED
	char* env[] = {
			"DYLD_SHARED_CACHE_DONT_VALIDATE=1",
			"DYLD_SHARED_CACHE_DIR=/System/Library/Caches/com.apple.dyld",
			"DYLD_SHARED_REGION=private"
	};

	unlink("/mnt/private/var/db/.launchd_use_gmalloc");
	puts("Creating untethered exploit\n");
	unlink("/mnt/usr/bin/patch");
	ret = install("/files/patch", "/mnt/usr/bin/patch", 0, 80, 0755);
	if (ret < 0) return -1;

	puts("Installing pf2\n");
	unlink("/mnt/usr/lib/pf2");
	fsexec(patch_kernel, env);
	ret = install("/mnt/pf2", "/mnt/usr/lib/pf2", 0, 80, 0755);
	if (ret < 0) return -1;

	puts("Installing libgmalloc\n");
	unlink("/mnt/usr/lib/libgmalloc.dylib");
	fsexec(patch_dyld, env);
	ret = install("/mnt/libgmalloc.dylib", "/mnt/usr/lib/libgmalloc.dylib", 0, 80, 0755);
	if (ret < 0) return -1;

	puts("Installing launchd_use_gmalloc\n");
	unlink("/mnt/private/var/db/.launchd_use_gmalloc");
	ret = install("/files/launchd_use_gmalloc", "/mnt/private/var/db/.launchd_use_gmalloc", 0, 80, 0755);
	if (ret < 0) return -1;

	//unlink("/mnt/pf2");
	//unlink("/mnt/usr/bin/patch");
	//unlink("/mnt/libgmalloc.dylib");
#endif

	return 0;
}

int main(int argc, char* argv[], char* env[]) {
	int ret = 0;
	struct stat status;

	console = open("/dev/console", O_WRONLY);
	dup2(console, 1);
	dup2(console, 2);
	envp = env;

	puts("Searching for disk...\n");
	while (stat("/dev/disk0s1", &status) != 0) {
		sleep(1);
	}
	puts("Pois0nDisk - by Chronic-Dev Team\n");

	puts("Mounting filesystem...\n");
	if (hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS | MNT_RDONLY) != 0) {
		puts("Unable to mount filesystem!\n");
		return -1;
	}
	puts("Filesystem mounted\n");

	puts("Mounting devices...\n");
	if (mount("devfs", "/mnt/dev", 0, NULL) != 0) {
		puts("Unable to mount devices!\n");
		unmount("/mnt", 0);
		return -1;
	}
	puts("Devices mounted\n");

	puts("Checking filesystem...\n");
	if (fsexec(fsck_hfs, env) != 0) {
		puts("Unable to alter root directory!\n");
		unmount("/mnt/dev", 0);
		unmount("/mnt", 0);
		return -1;
	}
	puts("Filesystem checked\n");

	puts("Updating filesystem...\n");
	if (hfs_mount("/dev/disk0s1", "/mnt", MNT_ROOTFS | MNT_UPDATE) != 0) {
		puts("Unable to update filesystem!\n");
		unmount("/mnt/dev", 0);
		unmount("/mnt", 0);
		return -1;
	}
	puts("Filesystem updated\n");

	puts("Installing files...\n");
	if (install_files() != 0) {
		puts("Failed to install files!\n");
		unmount("/mnt/dev", 0);
		unmount("/mnt", 0);
		return -1;
	}
	puts("Installation complete\n");
	sync();

	puts("Unmounting disks...\n");
	unmount("/mnt/dev", 0);
	unmount("/mnt", 0);

	puts("Flushing buffers...\n");
	sync();

	puts("Rebooting device...\n");
	close(console);
	reboot(1);

	return 0;
}
