#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

int stdout = 0;
int stderr = 0;

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

int hfs_mount(char *device, char *mountdir, int options) {
	int i;
	struct hfs_mount_args args;
	/*
	struct hfs_mount_args *p = &args;
	for (i = 0; i < sizeof(args); i++) {
	p[i] = 0;
	}
	*/
	args.fspec = device;

	//Call mount
	return mount("hfs", mountdir, options, &args);
}
