#include <libusb-1.0/libusb.h>
#include "libusb1.h"

static libusb_context* context = NULL;

irecv_error_t usb_init() {
	libusb_init(NULL);
	irecv_client_t client = (irecv_client_t) malloc(sizeof(struct irecv_client));
	if (client == NULL) {
		libusb_close(usb_handle);
		libusb_exit(libirecovery_context);
		return IRECV_E_OUT_OF_MEMORY;
	}

	memset(client, '\0', sizeof(struct irecv_client));
	client->interface = 0;
	client->handle = usb_handle;
	client->mode = usb_descriptor.idProduct;
	return client;
}

irecv_error_t usb_exit() {
	libusb_exit(NULL);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_open(irecv_client_t client) {
	int i = 0;
	char serial[256];
	struct libusb_device* usb_device = NULL;
	struct libusb_device** usb_device_list = NULL;
	struct libusb_device_handle* usb_handle = NULL;
	struct libusb_device_descriptor usb_descriptor;

	libusb_init(&libirecovery_context);
	if(libirecovery_debug) {
		irecv_set_debug_level(libirecovery_debug);
	}

	irecv_error_t error = IRECV_E_SUCCESS;
	int usb_device_count = libusb_get_device_list(libirecovery_context, &usb_device_list);
	for (i = 0; i < usb_device_count; i++) {
		usb_device = usb_device_list[i];
		libusb_get_device_descriptor(usb_device, &usb_descriptor);
		if (usb_descriptor.idVendor == APPLE_VENDOR_ID) {
			/* verify this device is in a mode we understand */
			if (usb_descriptor.idProduct == kRecoveryMode1 ||
				usb_descriptor.idProduct == kRecoveryMode2 ||
				usb_descriptor.idProduct == kRecoveryMode3 ||
				usb_descriptor.idProduct == kRecoveryMode4 ||
				usb_descriptor.idProduct == kDfuMode) {

				debug("opening device %04x:%04x...\n", usb_descriptor.idVendor, usb_descriptor.idProduct);

				libusb_open(usb_device, &usb_handle);
				if (usb_handle == NULL) {
					libusb_free_device_list(usb_device_list, 1);
					libusb_close(usb_handle);
					libusb_exit(libirecovery_context);
					return IRECV_E_UNABLE_TO_CONNECT;
				}
				libusb_free_device_list(usb_device_list, 1);

				error = irecv_set_configuration(client, 1);
				if (error != IRECV_E_SUCCESS) {
					return error;
				}

				error = irecv_set_interface(client, 0, 0);
				if (error != IRECV_E_SUCCESS) {
					return error;
				}

				/* cache usb serial */
				libusb_get_string_descriptor_ascii(client->handle, usb_descriptor.iSerialNumber, client->serial, 255);

				*pclient = client;
				return IRECV_E_SUCCESS;
			}
		}
	}

	return IRECV_E_UNABLE_TO_CONNECT;
}

irecv_error_t usb_close(irecv_client_t client) {
	libusb_close(client->handle);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_reset_device(irecv_client_t client) {
	libusb_reset_device(client->handle);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_debug_level(irecv_client_t client, int debug) {
	irecv_set_debug_level(1);
	return IRECV_E_SUCCESS;
}

int usb_get_device_list(irecv_client_t client, irecv_device_t** devices) {
	int i = 0;
	irecv_device_t device = NULL;
	struct libusb_device** libusb_devices = NULL;
	int count = libusb_get_device_list(NULL, &libusb_devices);
	if(count <= 0) {
		return -1;
	}

	for(i = 0; i < count; i++) {

	}

	return count;
}

irecv_error_t usb_get_device_descriptor(irecv_device_t device) {
	int descriptor = 0;
	struct libusb_device* devicex;
	struct libusb_device_descriptor* descriptorx;
	libusb_get_device_descriptor(devicex, descriptorx);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_free_device_list(irecv_device_t* devices, int mode) {
	struct libusb_device** devicesx;
	libusb_free_device_list(devicesx, mode);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_string_descriptor_ascii(irecv_client_t client, unsigned char* buffer, unsigned int size) {
	libusb_get_string_descriptor_ascii(client->handle, 1, buffer, size);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_configuration(irecv_client_t client, int* configuration) {
	libusb_get_configuration(client->handle, configuration);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_configuration(irecv_client_t client, int configuration) {
	libusb_set_configuration(client->handle, configuration);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_claim_interface(irecv_client_t client, int interface) {
	libusb_claim_interface(client->handle, interface);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int interface, int alt_interface) {
	libusb_set_interface_alt_setting(client->handle, interface, alt_interface);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_release_interface(irecv_client_t client, int interface) {
	libusb_release_interface(client->handle, interface);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_control_transfer(irecv_client_t client, char request, char subrequest, int something, int index, unsigned char* buffer, unsigned int size, int timeout) {
	// returns bytes read
	libusb_control_transfer(client->handle, request, subrequest, something, index, buffer, size, timeout);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout) {
	libusb_bulk_transfer(client->handle, request, buffer, maxsize, size, timeout);
	return IRECV_E_SUCCESS;
}


