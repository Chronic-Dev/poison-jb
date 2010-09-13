#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "partial.h"
#include "libirecovery.h"
#include "injectpois0n.h"

int injectpois0n_debug = 1;

int fetch_ibss(irecv_device_t device) {
	char name[32];
	char path[256];

	memset(name, '\0', 32);
	memset(path, '\0', 256);
	snprintf(name, 31, "iBSS.%s.RELEASE.dfu", device->model);
	snprintf(path, 255, "Firmware/dfu/%s", name);

	if(download_file_from_zip(device->url, path, "image.bin") != 0) {
		return -1;
	}

	return 0;
}

int shift_upload_counter(irecv_client_t client, int shift) {
	char* buffer = NULL;
	irecv_error_t error = 0;

	buffer = (char*) malloc(shift);
	if(buffer == NULL) {
		error("Out of memory\n");
		return -1;
	}

	error = irecv_recv_buffer(client, buffer, shift);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		free(buffer);
		return -1;
	}

	free(buffer);
	return 0;
}

int overwrite_sha1_registers(irecv_client_t client) {
	irecv_error_t error = 0;

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		return -1;
	}

	debug("Shifting upload pointer\n");
	if(shift_upload_counter(client, 0x80)) {
		error("Unable to shift upload counter\n");
		irecv_close(client);
		return -1;
	}

	debug("Resetting device\n");
	irecv_reset(client);

	debug("Finishing shift transaction\n");
	error = irecv_finish_transfer(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	irecv_error_t error = 0;
	irecv_client_t client = NULL;
	irecv_device_t device = NULL;

	// open connection to the device
	debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	// check the device mode
	debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}

	// discover the device type
	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("ERROR: Unable to discover device type\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	debug("Checking to make sure this is a compatiable device\n");
	if (device->chip_id != 8930) {
		error("Sorry this device is not compatiable for this jailbreak");
		irecv_close(client);
		return -1;
	}

	debug("Attempting to fetch iBSS from Apple's servers\n");
	if(fetch_ibss(device) < 0) {
		error("Unable to fetch iBSS from Apple's servers\n");
		irecv_close(client);
		return -1;
	}

	debug("Preparing to overwrite SHA1 registers\n");
	if(overwrite_sha1_registers(client) < 0) {
		error("Unable to overwrite SHA1 registers\n");
		irecv_close(client);
		return -1;
	}

	debug("Disconnecting from device\n");
	irecv_close(client);
	return 0;
}
