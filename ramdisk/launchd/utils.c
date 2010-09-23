#include "utils.h"

void _puts(char *msg) {
	while((*msg) != '\0') {
		write(1, msg, 1);
		msg++;
	}
}


int _mountHFS(char *device, char *mountdir, int options) {
	int i;
	struct hfs_mount_args args;
	args.fspec = device;
	return mount("hfs", mountdir, options, &args);
}

int _cp(char *src, char *dest) {
	char buf[0x800];
	int count = 0;
	int in = open(src, O_RDONLY);
	int out = open(dest, O_WRONLY | O_CREAT);
	if (in == 0 || out == 0) {
		return 1; //Unable to open one or the other
	}

	do {
		count = read(in, buf, 0x800);
		if (count != -1)
			count = write(out, buf, count);
	} while (count == 0x800);
	close(in);
	close(out);
	if (count == -1) {
		return 2; //Read or Write failed...
	}
	return 0;
}

