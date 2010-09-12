#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "partial.h"
#include "libirecovery.h"

int setupExploit(struct libusb_device_handle* handle);
int doExploit(struct libusb_device_handle *handle, char* ibss);

struct greenpois0n_device_t* check_device(irecv_client_t client) {
	int device = DEVICE_UNKNOWN;
	uint32_t bdid = 0;
	uint32_t cpid = 0;

	if (irecv_get_cpid(client, &cpid) < 0) {
		error("ERROR: Unable to get device CPID\n");
		return NULL;
	}

	switch (cpid) {
	case CPID_IPHONE2G:
		// iPhone1,1 iPhone1,2 and iPod1,1 all share the same ChipID
		//   so we need to check the BoardID
		if (irecv_get_bdid(client, &bdid) < 0) {
			error("ERROR: Unable to get device BDID\n");
			break;
		}

		switch (bdid) {
		case BDID_IPHONE2G:
			device = DEVICE_IPHONE2G;
			break;

		case BDID_IPHONE3G:
			device = DEVICE_IPHONE3G;
			break;

		case BDID_IPOD1G:
			device = DEVICE_IPOD1G;
			break;

		default:
			device = DEVICE_UNKNOWN;
			break;
		}
		break;

	case CPID_IPHONE3GS:
		device = DEVICE_IPHONE3GS;
		break;

	case CPID_IPOD2G:
		device = DEVICE_IPOD2G;
		break;

	case CPID_IPOD3G:
		device = DEVICE_IPOD3G;
		break;

	case CPID_IPAD1G:
		device = DEVICE_IPAD1G;
		break;

	default:
		device = DEVICE_UNKNOWN;
		break;
	}

	return &greenpois0n_devices[device];
}

int main(int argc, char* argv[]) {
	uint32_t cpid = 0;
	char* buffer = NULL;
	irecv_error_t error = 0;
	irecv_client_t client = NULL;
	struct greenpois0n_device_t* device = NULL;

	greenpois0n_debug = 1;
	/*
	buffer = malloc(0x2C000);
	if (buffer == NULL) {
		error("Out of memory\n");
		return -1;
	}
	*/

	debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}
	info("Connected to device\n");

	// check which mode the device is currently in so we know where to start
	debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}
	info("Found device in DFU mode\n");

	// figure out which processor this device contains
	error = irecv_get_cpid(client, &cpid);
	if(error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		return -1;
	}

	// make sure this device has an A4 processor
	if(cpid != 8930) {
		error("Sorry this device is not compatiable for this jailbreak");
		irecv_close(client);
		return -1;
	}

	// discover the device type
	device = check_device(client);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("ERROR: Unable to discover device type\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	// Download iBSS
	char* ibss = malloc(0x100);
	snprintf(ibss, 0x100, "iBSS.%s.RELEASE.dfu", device->model);

	char* ibss_path = malloc(0x200);
	snprintf(ibss_path, 0x200, "Firmware/dfu/%s", ibss);

	debug("Downloading iBSS from Apple's servers\n");
	if(download_file_from_zip(device->url, ibss_path, "image.bin") != 0) {
		error("Unable to download iBSS from firmware\n");
		irecv_close(client);
		free(ibss_path);
		free(ibss);
		return -1;
	}
	free(ibss);
	free(ibss_path);
	irecv_close(client);

	// Send Exploit
	if(doSHAtter()) {
		error("Unable to execute bootrom exploit\n");
		return -1;
	}

	// Send Payload1
	// Execute Payload
	// Send Payload2
	// Execute Payload2
	// Send Ramdisk
	// Run Ramdisk

	return 0;
}

int reconnect_device(irecv_client_t client) {
	irecv_error_t error = 0;

	sleep(3);
	irecv_close(client);
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		error("%s", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int newExploit() {
	char* buffer = NULL;
	irecv_error_t error = 0;
	irecv_client_t client = NULL;

	greenpois0n_debug = 1;
	buffer = malloc(0x2C000);
	if (buffer == NULL) {
		error("Out of memory\n");
		return -1;
	}

	debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		free(buffer);
		return -1;
	}

	debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		free(buffer);
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		free(buffer);
		return -1;
	}

	debug("Shifting upload pointer\n");
	error = irecv_recv_buffer(client, &buffer, 0x80);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		free(buffer);
		return -1;
	}

	debug("Resetting device\n");
	irecv_reset(client);

	debug("Finishing shift transaction\n");
	error = irecv_finish_transfer(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		irecv_close(client);
		free(buffer);
		return -1;
	}

	debug("Restarting device connection\n");
	if (reconnect_device(client)) {
		error("Unable to restart device connection\n");
		irecv_close(client);
		free(buffer);
		return -1;
	}

	debug("Disconnecting from device\n");
	irecv_close(client);
	return 0;
}
