#include "libusb1.h"
#include "libirecovery.h"
#include <libusb-1.0/libusb.h>

static libusb_context* context = NULL;

irecv_error_t usb_init() {
	libusb_init(NULL);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_exit() {
	libusb_exit(NULL);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_open(irecv_client_t client, irecv_device_t device) {
	struct libusb_device* devicex;
	libusb_open(devicex, &client->handle);
	return IRECV_E_SUCCESS;
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

irecv_error_t usb_get_device_list(irecv_client_t client, irecv_device_t** devices) {
	// returns device count
	struct libusb_device** devicesx;
	libusb_get_device_list(NULL, &devicesx);
	return IRECV_E_SUCCESS;
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


