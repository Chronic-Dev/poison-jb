#include "utils.h"
#include "syscalls.h"

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

void puts(const char* text) {
	write(STDOUT, text, strlen(text));
}

int cp(char *src, char *dest) {
	char buf[0x800];
	int count = 0;
	int in = open(src, O_RDONLY, 0);
	int out = open(dest, O_WRONLY | O_CREAT, 0);
	if (in == 0 || out == 0) {
		return 1; //Unable to open one or the other
	}

	do {
		count = read(in, buf, 0x800);
		if (count != -1) {
			count = write(out, buf, count);
		}
	} while (count == 0x800);
	close(in);
	close(out);
	if (count == -1) {
		return 2; //Read or Write failed...
	}
	return 0;
}
