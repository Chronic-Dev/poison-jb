#include "libpois0n.h"

void print_progress(double progress, void* data) {
	int i = 0;
	if(progress < 0) {
		return;
	}

	if(progress > 100) {
		progress = 100;
	}

	printf("\r[");
	for(i = 0; i < 50; i++) {
		if(i < progress / 2) {
			printf("=");
		} else {
			printf(" ");
		}
	}

	printf("] %3.1f%%", progress);
	fflush(stdout);
	if(progress == 100) {
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	pois0n_init();
	pois0n_set_callback(&print_progress, NULL);

	info("Waiting for device to enter DFU mode\n");
	while(pois0n_is_ready()) {
		sleep(1);
	}

	info("Found device in DFU mode\n");
	if(!pois0n_is_compatible()) {
		pois0n_inject();
	}

	pois0n_exit();
	return 0;
}
