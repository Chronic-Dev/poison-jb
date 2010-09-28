#include <stdio.h>
#include "libpois0n.h"

int main(int argc, char* argv[]) {
	pois0n_init();

	info("Waiting for device to enter DFU mode\n");
	while(pois0n_is_ready()) {
		sleep(1);
	}
	info("Found device in DFU mode\n");

	pois0n_inject();
	pois0n_exit();
	return 0;
}
