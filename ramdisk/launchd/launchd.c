#include "utils.h"

const char* greeting = "GreenPois0n Ramdisk - by Chronic-Dev\n";

int main(int argc, char* argv[], char* env[]) {
	write(0, greeting, strlen(greeting));
	//reboot(0);
	return 0;
}
