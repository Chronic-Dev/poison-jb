#include "../libSHAtter/SHAtter.h"
#include <stdio.h>

int main(int argc, char **args, char **env) {
	initInjector();
	if (isInDFU() == 0) {
		if (isCompatibleDevice == 0) {
			SHAtter();
		} else {
			printf("Device not valid or unable to verify\n");
		}
	} else {
		printf("Failed to connect to DFU mode device\n");
	}
	return 0;
}
