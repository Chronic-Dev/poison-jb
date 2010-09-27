#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

int stdout = 0;
int stderr = 0;

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
	int out = open(dest, O_WRONLY | O_CREAT, 0);
	if (in == 0 || out == 0) {
		return -1; //Unable to open one or the other
	}

	do {
		//memset(buffer, '\0', 0x800);
		count = read(in, buf, 0x800);
		if (count > 0) {
			count = write(out, buf, count);
		}

	} while (count > 0);

	close(in);
	close(out);

	if (count < 0) {
		return -1; //Read or Write failed...
	}

	return 0;
}

int hfs_mount(const char* device, const char* mountdir, int options) {
	struct hfs_mount_args args;
	args.fspec = device;
	return mount("hfs", mountdir, options, &args);
}
