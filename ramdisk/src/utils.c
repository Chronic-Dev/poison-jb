#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

#define kHFSVolumeUnmountedBit          0x100
#define kHFSBootVolumeInconsistentBit   0x800

#define FLIPENDIAN(x) flipEndian((unsigned char *)(&(x)), sizeof(x))

struct hfs_header {
    unsigned short signature;
    unsigned short version;
    unsigned int attributes;
    unsigned int lastMountedVersion;
    unsigned int journalInfoBlock;
};
typedef struct hfs_header hfs_header;

int stdout = 0;
int stderr = 0;
static int dirty = 0;

int install(const char* src, const char* dst, int uid, int gid, int mode) {
	int ret = 0;

	ret = cp(src, dst);
	if (ret < 0) {
		return ret;
	}

	ret = chown(dst, uid, gid);
	if (ret < 0) {
		return ret;
	}

	ret = chmod(dst, mode);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

void sleep(unsigned int seconds) {
	int i = 0;
	for(i = seconds * 10000000; i > 0; i--) {}
}

void puts(const char* msg) {
	while((*msg) != '\0') {
		write(1, msg, 1);
		msg++;
	}
	sync();
}

void putc(const char c) {
	char byte[2];
	byte[0] = c;
	byte[1] = '\0';
	write(1, byte, 1);
}

void puti(unsigned int integer) {
	int i = 0;
	char nyble = 0;

	for(i = 7; i >= 0; i--) {
		nyble = (integer >> (4 * i)) & 0xF;
		putc(nyble+0x30);
	}
}

int cp(const char *src, const char *dest) {
	int count = 0;
	char buf[0x800];

	int in = open(src, O_RDONLY, 0);
	if (in < 0) {
		return -1;
	}

	int out = open(dest, O_WRONLY | O_CREAT, 0);
	if (out < 0) {
		close(in);
		return -1;
	}

	do {
		count = read(in, buf, 0x800);
		if (count > 0) {
			count = write(out, buf, count);
		}
	} while (count > 0);

	close(in);
	close(out);

	if (count < 0) {
		return -1;
	}

	return 0;
}

int hfs_mount(const char* device, const char* mountdir, int options) {
	struct hfs_mount_args args;
	args.fspec = device;
	return mount("hfs", mountdir, options, &args);
}

void system(char* argv[], char *env[]) {
	if(vfork() != 0) {
		//while(wait4(-1, NULL, WNOHANG, NULL) <= 0) {
			sleep(1);
		//}
	} else {
		//execve(argv[0], argv, env);
	}
}


int strlen(const char* s) {
	int i = 0;
	for(i = 0; i >= 0; i++) {
		if(s[i] == '\0') return i;
	}
	return -1;
}

void memset(char* buffer, char value, int size) {
	int i = 0;
	for(i = 0; i < size; i++) {
		buffer[i] = value;
	}
}
