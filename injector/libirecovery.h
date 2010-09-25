/**
 * iRecovery - Utility for DFU 2.0, WTF and Recovery Mode
 * Copyright (C) 2008 - 2009 westbaer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef LIBIRECOVERY_H
#define LIBIRECOVERY_H

#ifdef __cplusplus
extern "C" {
#endif

#undef NULL
#define NULL 0

#define APPLE_VENDOR_ID 0x05AC

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


enum {
	kRecoveryMode1 = 0x1280,
	kRecoveryMode2 = 0x1281,
	kRecoveryMode3 = 0x1282,
	kRecoveryMode4 = 0x1283,
	kDfuMode = 0x1227
};

typedef enum {
	IRECV_E_SUCCESS = 0,
	IRECV_E_NO_DEVICE = -1,
	IRECV_E_OUT_OF_MEMORY = -2,
	IRECV_E_UNABLE_TO_CONNECT = -3,
	IRECV_E_INVALID_INPUT = -4,
	IRECV_E_FILE_NOT_FOUND = -5,
	IRECV_E_USB_UPLOAD = -6,
	IRECV_E_USB_STATUS = -7,
	IRECV_E_USB_INTERFACE = -8,
	IRECV_E_USB_CONFIGURATION = -9,
	IRECV_E_PIPE = -10,
	IRECV_E_TIMEOUT = -11,
	IRECV_E_UNKNOWN_ERROR = -255
} irecv_error_t;

typedef enum {
	IRECV_RECEIVED = 1,
	IRECV_PRECOMMAND = 2,
	IRECV_POSTCOMMAND = 3,
	IRECV_CONNECTED = 4,
	IRECV_DISCONNECTED = 5,
	IRECV_PROGRESS = 6
} irecv_event_type;

typedef struct {
	int size;
	char* data;
	double progress;
	irecv_event_type type;
} irecv_event_t;

typedef int irecv_context_t;

struct irecv_client;
typedef struct irecv_client* irecv_client_t;
typedef struct irecv_info* irecv_info_t;
typedef struct irecv_device* irecv_device_t;
typedef struct irecv_firmware* irecv_firmware_t;

typedef int(*irecv_event_cb_t)(irecv_client_t client, const irecv_event_t* event);

struct irecv_device {
	unsigned char length;
	unsigned char type;
	unsigned char class;
	unsigned char subclass;
	unsigned char protocol;
	unsigned char maxPacketSize;
	unsigned short vendor;
	unsigned short product;
	unsigned char serial[256];
};

struct irecv_client {
	int debug;
	int config;
	int interface;
	int alt_interface;
	unsigned short mode;
	unsigned char* serial;
	struct irecv_info_t* info;
	struct irecv_device_t* device;
	struct irecv_context_t* context;

#ifndef __APPLE__
	struct libusb_device_handle* handle;
#endif

	irecv_event_cb_t progress_callback;
	irecv_event_cb_t received_callback;
	irecv_event_cb_t connected_callback;
	irecv_event_cb_t precommand_callback;
	irecv_event_cb_t postcommand_callback;
	irecv_event_cb_t disconnected_callback;
};

struct irecv_firmware {
	char* version;
	char* url;
};

struct irecv_device {
	int index;
	const char* product;
	const char* model;
	unsigned int board_id;
	unsigned int chip_id;
	struct irecv_firmware* firmware;
};

static struct irecv_firmware irecv_iphone31_firmware[] = {
	{ "4.0",   "http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone4/061-7380.20100621,Vfgb5/iPhone3,1_4.0_8A293_Restore.ipsw" },
	{ "4.0.1", "http://appldnld.apple.com/iPhone4/061-8619.20100715.4Pnsx/iPhone3,1_4.0.1_8A306_Restore.ipsw" },
	{ "4.0.2", "http://appldnld.apple.com/iPhone4/061-8807.20100811.3Edre/iPhone3,1_4.0.2_8A400_Restore.ipsw" },
	{ "4.1",   "http://appldnld.apple.com/iPhone4/061-7939.20100908.Lcyg3/iPhone3,1_4.1_8B117_Restore.ipsw" },
	{ NULL, NULL }
};

static struct irecv_firmware irecv_ipod41_firmware[] = {
	{ "4.1",   "http://appldnld.apple.com/iPhone4/061-8490.20100901.hyjtR/iPod4,1_4.1_8B117_Restore.ipsw" },
	{ NULL, NULL }
};

static struct irecv_firmware irecv_ipad11_firmware[] = {
	{ "3.2",   "http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPad/061-7987.20100403.mjiTr/iPad1,1_3.2_7B367_Restore.ipsw" },
	{ "3.2.1", "http://appldnld.apple.com/iPad/061-8282.20100713.vgtgh/iPad1,1_3.2.1_7B405_Restore.ipsw" },
	{ "3.2.2", "http://appldnld.apple.com/iPad/061-8801.20100811.CvfR5/iPad1,1_3.2.2_7B500_Restore.ipsw" },
	{ NULL, NULL }
};

static struct irecv_device irecv_devices[] = {
	{  0, "iPhone1,1", "m68ap",  0,  8900, NULL },
	{  1, "iPod1,1",   "n45ap",  2,  8900, NULL },
	{  2, "iPhone1,2", "n82ap",  4,  8900, NULL },
	{  3, "iPod2,1",   "n72ap",  0,  8720, NULL },
	{  4, "iPhone2,1", "n88ap",  0,  8920, NULL },
	{  5, "iPod3,1",   "n18ap",  2,  8922, NULL },
	{  6, "iPad1,1",   "k48ap",  2,  8930, irecv_ipad11_firmware },
	{  7, "iPhone3,1", "n90ap",  0,  8930, irecv_iphone31_firmware },
	{  8, "iPod4,1",   "n81ap",  8,  8930, irecv_ipod41_firmware },
	{ -1,  NULL,        NULL,   -1,    -1, NULL }
};

static int debug;

void irecv_set_debug_level(int level);
const char* irecv_strerror(irecv_error_t error);
irecv_error_t irecv_open_attempts(irecv_client_t* pclient, int attempts);
irecv_error_t irecv_open(irecv_client_t* client);
irecv_error_t irecv_reset(irecv_client_t client);
irecv_error_t irecv_close(irecv_client_t client);
irecv_error_t irecv_receive(irecv_client_t client);
irecv_error_t irecv_send_exploit(irecv_client_t client);
irecv_error_t irecv_execute_script(irecv_client_t client, const char* filename);
irecv_error_t irecv_set_configuration(irecv_client_t client, int configuration);

irecv_error_t irecv_event_subscribe(irecv_client_t client, irecv_event_type type, irecv_event_cb_t callback, void *user_data);
irecv_error_t irecv_event_unsubscribe(irecv_client_t client, irecv_event_type type);

irecv_error_t irecv_send_file(irecv_client_t client, const char* filename, int dfuNotifyFinished);
irecv_error_t irecv_send_command(irecv_client_t client, char* command);
irecv_error_t irecv_send_buffer(irecv_client_t client, char* buffer, unsigned long length, int dfuNotifyFinished);

irecv_error_t irecv_getenv(irecv_client_t client, const char* variable, char** value);
irecv_error_t irecv_setenv(irecv_client_t client, const char* variable, const char* value);
irecv_error_t irecv_set_interface(irecv_client_t client, int interface, int alt_interface);
irecv_error_t irecv_get_cpid(irecv_client_t client, unsigned int* cpid);
irecv_error_t irecv_get_bdid(irecv_client_t client, unsigned int* bdid);
irecv_error_t irecv_get_ecid(irecv_client_t client, unsigned long long* ecid);

void irecv_init();
void irecv_exit();
irecv_client_t irecv_reconnect(irecv_client_t client);
irecv_error_t irecv_reset_counters(irecv_client_t client);
irecv_error_t irecv_finish_transfer(irecv_client_t client);
irecv_error_t irecv_recv_buffer(irecv_client_t client, char* buffer, unsigned long length);
irecv_error_t irecv_get_info(irecv_client_t client, irecv_info_t* info);

#ifdef __cplusplus
}
#endif

#endif
