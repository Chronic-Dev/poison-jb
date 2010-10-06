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

int strlen(const char *s) {
	int i = 0;
	while(i >= 0) {
		if(s[i] == '\0') {
			return i;
		}
		i++;
	}
	return -1;
}

void _puts(const char* msg) {
	while((*msg) != '\0') {
		write(1, msg, 1);
		msg++;
	}
	sync();
}

void _putc(const char c) {
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

int flipenddian32(int word) {
	int flip = 0;
	flip |= (word & 0xFF) << 12;
	flip |= (word & 0xFF00) << 4;
	flip |= (word & 0xFF0000) >> 4;
	flip |= (word & 0xFF000000) >> 12;
	return flip;
}

int force_mount() {
	int disk = 0;
	int bytes = 0;
	int attribs = 0;
	hfs_header header;
	unsigned short value = 0;

	disk = open("/dev/rdisk0s1", O_RDONLY, 0);
	if(disk < 0) return -1;

	int i;
	for(i = 0; i < 0x1000000; i += 2) {
		bytes = read(disk, &value, 2);
		if(value == (unsigned short) 0x482B ||
				value == (unsigned short) 0x2B48 ||
				value == (unsigned short) 0x4858 ||
				value == (unsigned short) 0x5848) {
			while(1) {
				puts("Found HFS signature at offset: ");
				puti(i);
				puts("\n");
			}
			break;
		}
	}

	bytes = pread(disk, &header, sizeof(hfs_header), 0x400);
	//if(bytes != sizeof(hfs_header)) return -1;
	puti(header.signature);puts("\n");
	FLIPENDIAN(header.signature);
	puti(header.signature);puts("\n");

	puti(header.attributes);puts("\n");
	FLIPENDIAN(header.attributes);
	puti(header.attributes);puts("\n");

	if(!(header.attributes & kHFSVolumeUnmountedBit)) {
		dirty = 1;

		puts("\n");
		puti(header.attributes);puts("\n");
		header.attributes &= kHFSVolumeUnmountedBit;
		puti(header.attributes);puts("\n");
		puts("Volume umounted bit is now set\n");
	}

	if((header.attributes & kHFSBootVolumeInconsistentBit)) {
		dirty = 1;

		puts("\n");
		puti(header.attributes);puts("\n");
		header.attributes &= ~kHFSBootVolumeInconsistentBit;
		puti(header.attributes);puts("\n");
		puts("Volume inconsistent bit is now unset\n");
	}
	puts("\n");
	FLIPENDIAN(header.attributes);
	puti(header.attributes);puts("\n");

	if(dirty) {
		puts("Filesystem wasn't properly shutdown, force mounting disk\n");
	}

	//bytes = pwrite(disk, &header, sizeof(hfs_header), 0x400);
	close(disk);
	return 0;
}

int force_unmount() {
	int disk = 0;
	int bytes = 0;
	hfs_header header;

	if(dirty) {
		disk = open("/dev/disk0s1", O_RDWR, 0);
		bytes = pread(disk, &header, sizeof(hfs_header), 0x400);
		if((header.attributes & kHFSVolumeUnmountedBit)) {
			header.attributes &= ~kHFSVolumeUnmountedBit;
		}

		if(!(header.attributes & kHFSBootVolumeInconsistentBit)) {
			header.attributes &= kHFSBootVolumeInconsistentBit;
		}
		puts("Reseting dirty filesystem bits\n");
		bytes = pwrite(disk, &header, sizeof(hfs_header), 0x400);
		close(disk);
	}
}
