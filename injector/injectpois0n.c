#include <stdio.h>
#include "libpois0n.h"

int main(int argc, char* argv[]) {
	if(pois0n_init() < 0) {
		error("Unable to initialize libpois0n\n");
		return -1;
	}

	info("Waiting for device to enter DFU mode\n");
	while(pois0n_is_ready()) {
		sleep(1);
	}
	info("Found device in DFU mode\n");

	pois0n_inject();
	pois0n_exit();
	return 0;
}
