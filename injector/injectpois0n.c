#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "partial.h"
#include "libirecovery.h"
#include "injectpois0n.h"

int injectpois0n_debug = 1;
static irecv_client_t client = NULL;
static irecv_device_t device = NULL;

int fetch_ibss() {
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

int receive_data(int shift) {
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

int overwrite_sha1_registers() {
	irecv_error_t error = 0;

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Shifting upload pointer\n");
	if(receive_data(0x80)) {
		error("Unable to shift upload counter\n");
		return -1;
	}

	debug("Resetting device\n");
	irecv_reset(client);

	debug("Finishing shift transaction\n");
	error = irecv_finish_transfer(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	debug("Overwriting SHA1 registers\n");
	if(receive_data(0x2C000)) {
		error("Unable to overwrite SHA1 registers\n");
		return -1;
	}
	
	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	return 0;
}

int upload_exploit_data() {
	irecv_error_t error = 0;

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Shifting upload pointer\n");
	if(receive_data(0x140)) {
		error("Unable to shift upload counter\n");
		return -1;
	}

	debug("Resetting device\n");
	irecv_reset(client);

	debug("Finishing shift transaction\n");
	error = irecv_finish_transfer(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	debug("Sending exploit data\n");
	error = irecv_send_file(client, "exploit.dfu", 0);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to send exploit data\n");
		return -1;
	}

	debug("Forcing prefetch abort exception\n");
	if(receive_data(0x2C000)) {
		error("Unable to force prefetch abort exception\n");
		return -1;
	}
	
	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	return 0;
}

int upload_ibss_data() {
	irecv_error_t error = 0;

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	error = irecv_send_file(client, "image.bin", 1);
	if(error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}
	
	return 0;
}

int upload_payload_data(const char* type) {
	char name[32];
	irecv_error_t error = 0;

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}
	
	memset(name, '\0', 32);	
        snprintf(name, 31, "%s.%s", type, device->model);

	error = irecv_send_file(client, name, 1);
	if(error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	return 0;
}

int execute_payload() {

	debug("Setting auto-boot to false\n");
	irecv_send_command(client, "setenv auto-boot false");
	irecv_send_command(client, "saveenv");

	debug("Loading and patching iBoot\n");
	irecv_send_command(client, "go");
	irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	irecv_send_command(client, "go patch 0x41000000 0x38000");
	irecv_send_command(client, "go jump 0x41000040");

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	debug("Sending iBoot payload\n");
	upload_payload_data("iBoot");

	// it dunt werk!!! =(
	//irecv_send_command(client, "go");
	//irecv_send_command(client, "go kernel load 0x42000000");
	//irecv_send_command(client, "go patch 0x41000000 0x38000");
	//irecv_send_command(client, "go kernel patch 0x42000000 10186752");

	return 0;
}

void quit() {
	irecv_close(client);
	irecv_exit();
}

int main(int argc, char* argv[]) {
	irecv_error_t error = 0;

	irecv_init();

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
		quit();
		return -1;
	}

	// discover the device type
	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("ERROR: Unable to discover device type\n");
		quit();
		return -1;
	}
	info("Identified device as %s\n", device->product);

	debug("Checking to make sure this is a compatible device\n");
	if (device->chip_id != 8930) {
		error("Sorry this device is not compatible for this jailbreak");
		quit();
		return -1;
	}

	debug("Attempting to fetch iBSS from Apple's servers\n");
	if(fetch_ibss() < 0) {
		error("Unable to fetch iBSS from Apple's servers\n");
		quit();
		return -1;
	}

	debug("Preparing to overwrite SHA1 registers\n");
	if(overwrite_sha1_registers() < 0) {
		error("Unable to overwrite SHA1 registers\n");
		quit();
		return -1;
	}
	
	debug("Preparing to upload exploit data\n");
	if(upload_exploit_data() < 0) {
		error("Unable to upload exploit data\n");
		quit();
		return -1;
	}
	
	debug("Preparing to send iBSS to device\n");
	if(upload_ibss_data() < 0) {
		error("Unable to upload iBSS\n");
		quit();
		return -1;
	}

	debug("Preparing to send payload to device\n");
	if(upload_payload_data("iBSS") < 0) {
		error("Unable to upload payload\n");
		quit();
		return -1;
	}

	debug("Executing payload\n");
	if(execute_payload() < 0) {
		error("Unable to execute payload\n");
		quit();
		return -1;
	}

	debug("Disconnecting from device\n");
	quit();
	return 0;
}
