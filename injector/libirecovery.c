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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libirecovery.h"

#ifdef __APPLE__
#	include "iokit.h"
#else
#	include "libusb1.h"
#endif

#define BUFFER_SIZE 0x1000
#define debug(...) if(debug) fprintf(stderr, __VA_ARGS__)

static int debug = 0;

int irecv_write_file(const char* filename, const void* data, unsigned int size);
int irecv_read_file(const char* filename, char** data, unsigned int* size);

void irecv_init() {
	usb_init();
}

void irecv_exit() {
	usb_exit();
}

irecv_error_t irecv_open_attempts(irecv_client_t* pclient, int attempts) {
	int i = 0;
	for (i = 0; i < attempts; i++) {
		if (irecv_open(pclient) != IRECV_E_SUCCESS) {
			debug("Connection failed. Waiting 1 sec before retry.");
			sleep(1);
		} else {
			return IRECV_E_SUCCESS;
		}		
	}

	return IRECV_E_UNABLE_TO_CONNECT;       
}

irecv_error_t irecv_open(irecv_client_t* pclient) {
	int i = 0;
	int count = 0;
	irecv_device_t device = NULL;
	irecv_device_t* devices = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	*pclient = NULL;
	if(debug) {
		irecv_set_debug_level(debug);
	}
	irecv_descriptor_t descriptor;
	count = usb_get_device_list(*pclient, &devices);
	for (i = 0; i < count; i++) {
		device = devices[i];
		//usb_get_device_descriptor(device, &descriptor);
		if (descriptor->vendor == APPLE_VENDOR_ID) {
			/* verify this device is in a mode we understand */
			if (descriptor->product == kRecoveryMode1 ||
					descriptor->product == kRecoveryMode2 ||
					descriptor->product == kRecoveryMode3 ||
					descriptor->product == kRecoveryMode4 ||
					descriptor->product == kDfuMode) {

				debug("opening device %04x:%04x...\n", descriptor->vendor, descriptor->product);

				irecv_client_t client = (irecv_client_t) malloc(sizeof(struct irecv_client));
				if (client == NULL) {
					usb_close(client);
					usb_exit(client);
					return IRECV_E_OUT_OF_MEMORY;
				}

				usb_open(client, device);
				if (client->handle == NULL) {
					usb_free_device_list(devices, 1);
					usb_close(client);
					usb_exit();
					return IRECV_E_UNABLE_TO_CONNECT;
				}
				usb_free_device_list(devices, 1);

				memset(client, '\0', sizeof(struct irecv_client));
				client->interface = 0;
				client->mode = (unsigned short) descriptor->product;
				if (client->mode != kDfuMode) {
					error = irecv_set_configuration(client, 1);
					if (error != IRECV_E_SUCCESS) {
						return error;
					}

					error = irecv_set_interface(client, 1, 1);
					if (error != IRECV_E_SUCCESS) {
						return error;
					}
				}

				/* cache usb serial */
				usb_get_string_descriptor_ascii(client, client->serial, client->serial, 255);

				*pclient = client;
				return IRECV_E_SUCCESS;
			}
		}
	}

	return IRECV_E_UNABLE_TO_CONNECT;
}

irecv_error_t irecv_set_configuration(irecv_client_t client, int configuration) {
	int current = 0;
	debug("Setting to configuration %d\n", configuration);

	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	usb_get_configuration(client, &current);
	if (current != configuration) {
		if (usb_set_configuration(client, configuration) < 0) {
			return IRECV_E_USB_CONFIGURATION;
		}
	}

	client->config = configuration;
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_set_interface(irecv_client_t client, int interface, int alt_interface) {
	debug("Setting to interface %d:%d\n", interface, alt_interface);

	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	if (client->interface == interface) {
		return IRECV_E_SUCCESS;
	}

	if (usb_claim_interface(client, interface) < 0) {
		return IRECV_E_USB_INTERFACE;
	}

	if (usb_set_interface_alt_setting(client, interface, alt_interface) < 0) {
		return IRECV_E_USB_INTERFACE;
	}

	client->interface = interface;
	client->alt_interface = alt_interface;
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_reset(irecv_client_t client) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	usb_reset_device(client);

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_event_subscribe(irecv_client_t client, irecv_event_type type, irecv_event_cb_t callback, void* user_data) {
	switch(type) {
	case IRECV_RECEIVED:
		client->received_callback = callback;
		break;

	case IRECV_PROGRESS:
		client->progress_callback = callback;

	case IRECV_CONNECTED:
		client->connected_callback = callback;

	case IRECV_PRECOMMAND:
		client->precommand_callback = callback;
		break;

	case IRECV_POSTCOMMAND:
		client->postcommand_callback = callback;
		break;

	case IRECV_DISCONNECTED:
		client->disconnected_callback = callback;

	default:
		return IRECV_E_UNKNOWN_ERROR;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_event_unsubscribe(irecv_client_t client, irecv_event_type type) {
	switch(type) {
	case IRECV_RECEIVED:
		client->received_callback = NULL;
		break;

	case IRECV_PROGRESS:
		client->progress_callback = NULL;

	case IRECV_CONNECTED:
		client->connected_callback = NULL;

	case IRECV_PRECOMMAND:
		client->precommand_callback = NULL;
		break;

	case IRECV_POSTCOMMAND:
		client->postcommand_callback = NULL;
		break;

	case IRECV_DISCONNECTED:
		client->disconnected_callback = NULL;

	default:
		return IRECV_E_UNKNOWN_ERROR;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_close(irecv_client_t client) {
	if (client != NULL) {
		if(client->disconnected_callback != NULL) {
			irecv_event_t event;
			event.size = 0;
			event.data = NULL;
			event.progress = 0;
			event.type = IRECV_DISCONNECTED;
			client->disconnected_callback(client, &event);
		}

		if (client->handle != NULL) {
			if (client->mode != kDfuMode) {
				usb_release_interface(client, client->interface);
			}
			usb_close(client);
			client->handle = NULL;
		}

		free(client);
		client = NULL;
	}

	return IRECV_E_SUCCESS;
}

void irecv_set_debug_level(int level) {
	//debug = level;
	//if(context) {
		///usb_set_debug(client->context, debug);
	//}
}

static irecv_error_t irecv_send_command_raw(irecv_client_t client, char* command) {
	unsigned int length = strlen(command);
	if (length >= 0x100) {
		length = 0xFF;
	}

	if (length > 0) {
		int ret = usb_control_transfer(client, 0x40, 0, 0, 0, (unsigned char*) command, length + 1, 1000);
		/*
		if ((ret < 0) || (ret != (length + 1))) {
			if (ret == LIBUSB_ERROR_PIPE)
				return IRECV_E_PIPE;
			if (ret == LIBUSB_ERROR_TIMEOUT)
				return IRECV_E_TIMEOUT;
			return IRECV_E_UNKNOWN_ERROR;
		}
		*/
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_send_command(irecv_client_t client, char* command) {
	irecv_error_t error = 0;

	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	unsigned int length = strlen(command);
	if (length >= 0x100) {
		length = 0xFF;
	}

	irecv_event_t event;
	if(client->precommand_callback != NULL) {
		event.size = length;
		event.data = command;
		event.type = IRECV_PRECOMMAND;
		if(client->precommand_callback(client, &event)) {
			return IRECV_E_SUCCESS;
		}
	}

	error = irecv_send_command_raw(client, command);
	if (error != IRECV_E_SUCCESS) {
		debug("Failed to send command %s\n", command);
		if (error != IRECV_E_PIPE)
			return error;
	}

	if(client->postcommand_callback != NULL) {
		event.size = length;
		event.data = command;
		event.type = IRECV_POSTCOMMAND;
		if(client->postcommand_callback(client, &event)) {
			return IRECV_E_SUCCESS;
		}
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_send_file(irecv_client_t client, const char* filename, int dfuNotifyFinished) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		return IRECV_E_FILE_NOT_FOUND;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*) malloc(length);
	if (buffer == NULL) {
		fclose(file);
		return IRECV_E_OUT_OF_MEMORY;
	}

	long bytes = fread(buffer, 1, length, file);
	fclose(file);

	if (bytes != length) {
		free(buffer);
		return IRECV_E_UNKNOWN_ERROR;
	}

	irecv_error_t error = irecv_send_buffer(client, buffer, length, dfuNotifyFinished);
	free(buffer);
	return error;
}

irecv_error_t irecv_get_status(irecv_client_t client, unsigned int* status) {
	if (client == NULL || client->handle == NULL) {
		*status = 0;
		return IRECV_E_NO_DEVICE;
	}

	unsigned char buffer[6];
	memset(buffer, '\0', 6);
	if (usb_control_transfer(client, 0xA1, 3, 0, 0, buffer, 6, 1000) != 6) {
		*status = 0;
		return IRECV_E_USB_STATUS;
	}

	*status = (unsigned int) buffer[4];
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_send_buffer(irecv_client_t client, char* buffer, unsigned long length, int dfuNotifyFinished) {
	irecv_error_t error = 0;
	int recovery_mode = (client->mode != kDfuMode);

	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	int packet_size = recovery_mode ? 0x2000: 0x800;
	int last = length % packet_size;
	int packets = length / packet_size;
	if (last != 0) {
		packets++;
	} else {
		last = packet_size;
	}

	/* initiate transfer */
	if (recovery_mode) {
		error = usb_control_transfer(client, 0x41, 0, 0, 0, NULL, 0, 1000);
		if (error != IRECV_E_SUCCESS) {
			return error;
		}
	}

	int i = 0;
	double progress = 0;
	unsigned long count = 0;
	unsigned int status = 0;
	int bytes = 0;
	for (i = 0; i < packets; i++) {
		int size = (i + 1) < packets ? packet_size : last;

		/* Use bulk transfer for recovery mode and control transfer for DFU and WTF mode */
		if (recovery_mode) {
			error = usb_bulk_transfer(client, 0x04, &buffer[i * packet_size], size, &bytes, 1000);
		} else {
			bytes = usb_control_transfer(client, 0x21, 1, 0, 0, &buffer[i * packet_size], size, 1000);
		}

		if (bytes != size) {
			return IRECV_E_USB_UPLOAD;
		}

		if (!recovery_mode) {
			error = irecv_get_status(client, &status);
		}

		if (error != IRECV_E_SUCCESS) {
			return error;
		}

		if (!recovery_mode && status != 5) {
			return IRECV_E_USB_UPLOAD;
		}

		count += size;
		if(client->progress_callback != NULL) {
			irecv_event_t event;
			event.progress = ((double) count/ (double) length) * 100.0;
			event.type = IRECV_PROGRESS;
			event.data = "Uploading";
			event.size = count;
			client->progress_callback(client, &event);
		} else {
			debug("Sent: %d bytes - %lu of %lu\n", bytes, count, length);
		}
	}

	if (dfuNotifyFinished && !recovery_mode) {
		usb_control_transfer(client, 0x21, 1, 0, 0, (unsigned char*) buffer, 0, 1000);
		for (i = 0; i < 3; i++) {
			error = irecv_get_status(client, &status);
			if (error != IRECV_E_SUCCESS) {
				return error;
			}
		}
		irecv_reset(client);
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_receive(irecv_client_t client) {
	char buffer[BUFFER_SIZE];
	memset(buffer, '\0', BUFFER_SIZE);
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	int bytes = 0;
	while (usb_bulk_transfer(client, 0x81, (unsigned char*) buffer, BUFFER_SIZE, &bytes, 1000) == 0) {
		if (bytes > 0) {
			if (client->received_callback != NULL) {
				irecv_event_t event;
				event.size = bytes;
				event.data = buffer;
				event.type = IRECV_RECEIVED;
				if (client->received_callback(client, &event) != 0) {
					return IRECV_E_SUCCESS;
				}
			}
		} else break;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_getenv(irecv_client_t client, const char* variable, char** value) {
	char command[256];
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	*value = NULL;

	if(variable == NULL) {
		return IRECV_E_UNKNOWN_ERROR;
	}

	memset(command, '\0', sizeof(command));
	snprintf(command, sizeof(command)-1, "getenv %s", variable);
	irecv_error_t error = irecv_send_command_raw(client, command);
	if(error == IRECV_E_PIPE)
		return IRECV_E_SUCCESS;
	if(error != IRECV_E_SUCCESS)
		return error;

	char* response = (char*) malloc(256);
	if (response == NULL) {
		return IRECV_E_OUT_OF_MEMORY;
	}

	memset(response, '\0', 256);
	int ret = usb_control_transfer(client, 0xC0, 0, 0, 0, (unsigned char*) response, 255, 500);
	if (ret < 0)
		return IRECV_E_UNKNOWN_ERROR;

	*value = response;
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_get_cpid(irecv_client_t client, unsigned int* cpid) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	char* cpid_string = strstr(client->serial, "CPID:");
	if (cpid_string == NULL) {
		*cpid = 0;
		return IRECV_E_UNKNOWN_ERROR;
	}
	sscanf(cpid_string, "CPID:%d", cpid);

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_get_bdid(irecv_client_t client, unsigned int* bdid) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	char* bdid_string = strstr(client->serial, "BDID:");
	if (bdid_string == NULL) {
		*bdid = 0;
		return IRECV_E_UNKNOWN_ERROR;
	}
	sscanf(bdid_string, "BDID:%d", bdid);

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_get_ecid(irecv_client_t client, unsigned long long* ecid) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	char* ecid_string = strstr(client->serial, "ECID:");
	if (ecid_string == NULL) {
		*ecid = 0;
		return IRECV_E_UNKNOWN_ERROR;
	}
	sscanf(ecid_string, "ECID:%qX", ecid);

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_send_exploit(irecv_client_t client) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	usb_control_transfer(client, 0x21, 2, 0, 0, NULL, 0, 1000);
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_execute_script(irecv_client_t client, const char* filename) {
	irecv_error_t error = IRECV_E_SUCCESS;
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	char* file_data = NULL;
	unsigned int file_size = 0;
	if(irecv_read_file(filename, &file_data, &file_size) < 0) {
		return IRECV_E_FILE_NOT_FOUND;
	}

	char* line = strtok(file_data, "\n");
	while(line != NULL) {
		if(line[0] != '#') {
			error = irecv_send_command(client, line);
			if(error != IRECV_E_SUCCESS) {
				return error;
			}

			error = irecv_receive(client);
			if(error != IRECV_E_SUCCESS) {
				return error;
			}
		}
		line = strtok(NULL, "\n");
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_setenv(irecv_client_t client, const char* variable, const char* value) {
	char command[256];
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	if(variable == NULL || value == NULL) {
		return IRECV_E_UNKNOWN_ERROR;
	}

	memset(command, '\0', sizeof(command));
	snprintf(command, sizeof(command)-1, "setenv %s %s", variable, value);
	irecv_error_t error = irecv_send_command_raw(client, command);
	if(error != IRECV_E_SUCCESS) {
		return error;
	}

	return IRECV_E_SUCCESS;
}

const char* irecv_strerror(irecv_error_t error) {
	switch (error) {
	case IRECV_E_SUCCESS:
		return "Command completed successfully";

	case IRECV_E_NO_DEVICE:
		return "Unable to find device";

	case IRECV_E_OUT_OF_MEMORY:
		return "Out of memory";

	case IRECV_E_UNABLE_TO_CONNECT:
		return "Unable to connect to device";

	case IRECV_E_INVALID_INPUT:
		return "Invalid input";

	case IRECV_E_FILE_NOT_FOUND:
		return "File not found";

	case IRECV_E_USB_UPLOAD:
		return "Unable to upload data to device";

	case IRECV_E_USB_STATUS:
		return "Unable to get device status";

	case IRECV_E_USB_INTERFACE:
		return "Unable to set device interface";

	case IRECV_E_USB_CONFIGURATION:
		return "Unable to set device configuration";

	case IRECV_E_PIPE:
		return "Broken pipuint32_te";

	case IRECV_E_TIMEOUT:
		return "Timeout talking to device";

	default:
		return "Unknown error";
	}

	return NULL;
}

int irecv_write_file(const char* filename, const void* data, unsigned int size) {
	size_t bytes = 0;
	FILE* file = NULL;

	debug("Writing data to %s\n", filename);
	file = fopen(filename, "wb");
	if (file == NULL) {
		return -1;
	}

	bytes = fwrite(data, 1, size, file);
	fclose(file);

	if (bytes != size) {
		//error("ERROR: Unable to write entire file: %s: %d of %d\n", filename, bytes, size);
		return -1;
	}

	return size;
}

int irecv_read_file(const char* filename, char** data, unsigned int* size) {
	size_t bytes = 0;
	size_t length = 0;
	FILE* file = NULL;
	char* buffer = NULL;
	debug("Reading data from %s\n", filename);

	*size = 0;
	*data = NULL;

	file = fopen(filename, "rb");
	if (file == NULL) {
		return -1;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	rewind(file);

	buffer = (char*) malloc(length);
	if(buffer == NULL) {
		fclose(file);
		return -1;
	}
	bytes = fread(buffer, 1, length, file);
	fclose(file);

	if(bytes != length) {
		free(buffer);
		return -1;
	}

	*size = length;
	*data = buffer;
	return 0;
}

irecv_error_t irecv_reset_counters(irecv_client_t client) {
	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	usb_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 1000);
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_recv_buffer(irecv_client_t client, char* buffer, unsigned long length) {
	irecv_error_t error = 0;
	int recovery_mode = (client->mode != kDfuMode);

	if (client == NULL || client->handle == NULL) {
		return IRECV_E_NO_DEVICE;
	}

	int packet_size = recovery_mode ? 0x2000: 0x800;
	int last = length % packet_size;
	int packets = length / packet_size;
	if (last != 0) {
		packets++;
	} else {
		last = packet_size;
	}

	int i = 0;
	int bytes = 0;
	//double progresuint32_ts = 0;
	unsigned long count = 0;
	unsigned int status = 0;
	for (i = 0; i < packets; i++) {
		unsigned short size = (i+1) < packets ? packet_size : last;

		bytes = usb_control_transfer(client, 0xA1, 2, 0, 0, &buffer[i * packet_size], size, 1000);

		if (bytes != size) {
			return IRECV_E_USB_UPLOAD;
		}

		count += size;
		if(client->progress_callback != NULL) {
			irecv_event_t event;
			event.progress = ((double) count/ (double) length) * 100.0;
			event.type = IRECV_PROGRESS;
			event.data = "Downloading";
			event.size = count;
			client->progress_callback(client, &event);
		} else {
			debug("Sent: %d bytes - %lu of %lu\n", bytes, count, length);
		}
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_finish_transfer(irecv_client_t client) {
	int i = 0;
	unsigned int status = 0;
	usb_control_transfer(client, 0x21, 1, 0, 0, 0, 0, 1000);
	for(i = 0; i < 3; i++){
		irecv_get_status(client, &status);
	}
	irecv_reset(client);
	return IRECV_E_SUCCESS;
}

irecv_error_t irecv_get_device(irecv_client_t client, irecv_device_t* device) {
	int device_id = DEVICE_UNKNOWN;
	unsigned int bdid = 0;
	unsigned int cpid = 0;

	if (irecv_get_cpid(client, &cpid) < 0) {
		return IRECV_E_UNKNOWN_ERROR;
	}

	switch (cpid) {
	case CPID_IPHONE2G:
		// iPhone1,1 iPhone1,2 and iPod1,1 all share the same ChipID
		//   so we need to check the BoardID
		if (irecv_get_bdid(client, &bdid) < 0) {
			break;
		}

		switch (bdid) {
		case BDID_IPHONE2G:
			device_id = DEVICE_IPHONE2G;
			break;

		case BDID_IPHONE3G:
			device_id = DEVICE_IPHONE3G;
			break;

		case BDID_IPOD1G:
			device_id = DEVICE_IPOD1G;
			break;

		default:
			device_id = DEVICE_UNKNOWN;
			break;
		}
		break;

	case CPID_IPHONE3GS:
		device_id = DEVICE_IPHONE3GS;
		break;

	case CPID_IPOD2G:
		device_id = DEVICE_IPOD2G;
		break;

	case CPID_IPOD3G:
		device_id = DEVICE_IPOD3G;
		break;

	case CPID_IPAD1G:
		// iPhone3,1 iPad4,1 and iPad1,1 all share the same ChipID
		//   so we need to check the BoardID
		if (irecv_get_bdid(client, &bdid) < 0) {
			break;
		}


		switch (bdid) {
		case BDID_IPAD1G:
			device_id = DEVICE_IPAD1G;
			break;

		case BDID_IPHONE4:
			device_id = DEVICE_IPHONE4;
			break;

		case BDID_IPOD4G:
			device_id = DEVICE_IPOD4G;
			break;

		default:
			device_id = DEVICE_UNKNOWN;
			break;
		}
		break;

	default:
		device_id = DEVICE_UNKNOWN;
		break;
	}

	*device = &irecv_devices[device_id];
	return IRECV_E_SUCCESS;
}

irecv_client_t irecv_reconnect(irecv_client_t client) {
	irecv_error_t error = 0;
	irecv_client_t new_client = NULL;

	if(client->handle) {
		irecv_close(client);
	}

	sleep(2); // let the time for the device to come up

	error = irecv_open_attempts(&new_client, 10);
	if(error != IRECV_E_SUCCESS) {
		return NULL;
	}

	return new_client;
}
