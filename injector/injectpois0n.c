#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "partial.h"
#include "libirecovery.h"
#include "injectpois0n.h"

int injectpois0n_debug = 1;
static irecv_client_t client = NULL;
static irecv_device_t device = NULL;

int receive_data(int bytes) {
	char* buffer = NULL;
	irecv_error_t error = 0;

	buffer = (char*) malloc(bytes);
	if(buffer == NULL) {
		error("Out of memory\n");
		return -1;
	}

	error = irecv_recv_buffer(client, buffer, bytes);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		free(buffer);
		return -1;
	}

	free(buffer);
	return 0;
}

int fetch_dfu_image(char* type) {
	char name[32];
	char path[256];

	memset(name, '\0', 32);
	memset(path, '\0', 256);
	snprintf(name, 31, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 255, "Firmware/dfu/%s", name);

	if(download_file_from_zip(device->url, path, type) != 0) {
		return -1;
	}

	return 0;
}

int upload_dfu_image(char* type) {
	irecv_error_t error = 0;

	debug("Attempting to fetch iBSS from Apple's servers\n");
	if(fetch_dfu_image(type) < 0) {
		debug("%s\n", irecv_strerror(error));\
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));\
		return -1;
	}

	error = irecv_send_file(client, "iBSS", 1);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));\
		return -1;
	}
}

int upload_dfu_payload(char* type) {
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

	debug("Uploading iBSS payload\n");
	error = irecv_send_file(client, name, 1);
	if(error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

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

int upload_exploit() {
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
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	return 0;
}

int fetch_ibss() {
	if(fetch_dfu_image("iBSS") < 0) {
		error("Unable to download iBSS from Apple's servers\n");
		return -1;
	}
	return 0;
}

int fetch_ibec() {
	if(fetch_dfu_image("iBEC") < 0) {
		error("Unable to download iBEC from Apple's servers\n");
		return -1;
	}
	return 0;
}

int upload_ibss() {
	irecv_error_t error = 0;

	debug("Attempting to fetch iBSS from Apple's servers\n");
	if(fetch_ibss() < 0) {
		error("Unable upload iBSS\n");
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload iBSS\n");
		return -1;
	}

	error = irecv_send_file(client, "iBSS", 1);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload iBSS\n");
		return -1;
	}
	
	return 0;
}

int upload_ibec() {
	irecv_error_t error = 0;

	debug("Attempting to fetch iBEC from Apple's servers\n");
	if(fetch_ibec() < 0) {
		error("Unable to upload iBEC\n");
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload iBEC\n");
		return -1;
	}

	error = irecv_send_file(client, "iBEC", 1);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload iBEC\n");
		return -1;
	}

	return 0;
}

int upload_ibss_payload() {
	if(upload_dfu_payload("iBSS") < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}
	return 0;
}

int upload_ibec_payload() {
	if(upload_dfu_payload("iBEC") < 0) {
		error("Unable to upload iBEC payload\n");
		return -1;
	}
	return 0;
}

int execute_ibss_payload() {
	debug("Initializing greenpois0n in iBSS\n");
	irecv_send_command(client, "go");

	debug("Loading and patching iBoot\n");
	irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	irecv_send_command(client, "go patch 0x41000000 0x38000");
	irecv_send_command(client, "go jump 0x41000040");

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	upload_dfu_payload("iBoot");
	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	return 0;
}

void quit() {
	irecv_close(client);
	irecv_exit();
}

int main(int argc, char* argv[]) {
	irecv_error_t error = 0;
	irecv_init();

	//////////////////////////////////////
	// Begin
	debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	//////////////////////////////////////
	// Check device
	debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		quit();
		return -1;
	}

	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("ERROR: Unable to discover device type\n");
		quit();
		return -1;
	}
	info("Identified device as %s\n", device->product);

	debug("Checking if this device is compatible with this jailbreak\n");
	if (device->chip_id != 8930) {
		error("Sorry this device is not compatible with this jailbreak");
		quit();
		return -1;
	}

	//////////////////////////////////////
	// Send exploit
	debug("Preparing to overwrite SHA1 registers\n");
	if(overwrite_sha1_registers() < 0) {
		error("Unable to overwrite SHA1 registers\n");
		quit();
		return -1;
	}
	
	debug("Preparing to upload exploit data\n");
	if(upload_exploit() < 0) {
		error("Unable to upload exploit data\n");
		quit();
		return -1;
	}

	//////////////////////////////////////
	// Send iBSS
	debug("Preparing to upload iBSS\n");
	if(upload_ibss() < 0) {
		error("Unable to upload iBSS\n");
		quit();
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload iBSS payload\n");
	if(upload_ibss_payload("iBSS") < 0) {
		error("Unable to upload iBSS payload\n");
		quit();
		return -1;
	}

	debug("Executing iBSS payload\n");
	if(execute_ibss_payload() < 0) {
		error("Unable to execute iBSS payload\n");
		quit();
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client);
	if (client == NULL) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	//////////////////////////////////////
	// Send iBEC
	/*
	debug("Preparing to upload iBEC payload\n");
	if(upload_ibec_payload() < 0) {
		error("Unable to upload iBEC payload\n");
		quit();
		return -1;
	}

	debug("Executing iBEC payload\n");
	if(execute_ibec_payload() < 0) {
		error("Unable to exiBSiBSSSecute iBEC payload\n");
		quit();
		return -1;
	}
	*/

	//////////////////////////////////////
	// Send ramdisk
	/*
	debug("Preparing to upload ramdisk\n");
	if(upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		quit();
		return -1;
	}

	debug("Executing ramdisk\n");
	if(execute_ramdisk() < 0) {
		error("Unable to execute ramdisk\n");
		quit();
		return -1;
	}
	*/

	//////////////////////////////////////
	// End
	debug("Disconnecting from device\n");
	quit();
	return 0;
}
