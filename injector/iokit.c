#include "libusb1.h"
#include "libirecovery.h"
#include <libusb-1.0/libusb.h>

IOUSBDeviceInterface** dev;
IOUSBInterfaceInterface** interface;

irecv_error_t usb_init() {
	// initiate dev

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_exit() {
	
	
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_open(irecv_client_t client, irecv_device_t device) {
	IOReturn err;
	err = (*dev)->USBDeviceOpen(dev);
	if (err == kIOReturnSuccess) {
		return IRECV_E_SUCCESS;
	} else {
		return IRECV_E_UNABLE_TO_CONNECT;
	}
}

irecv_error_t usb_close(irecv_client_t client) {
	if (opened) {
		opened = FALSE;
		return ((*dev)->USBDeviceClose(dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_reset_device(irecv_client_t client) {
	if (opened) {
		return ((*dev)->ResetDevice(dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
	}
	
	return IRECV_E_SUCCESS; // fix the return code
}

irecv_error_t usb_set_debug_level(irecv_client_t client, int debug) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_device_list(irecv_client_t client, irecv_device_t** devices) {
	// returns device count


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_device_descriptor(irecv_device_t device) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_free_device_list(irecv_device_t* devices, int mode) {

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_string_descriptor_ascii(irecv_client_t client, unsigned char* buffer, unsigned int size) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_configuration(irecv_client_t client, int* configuration) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_configuration(irecv_client_t client, int configuration) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_claim_interface(irecv_client_t client, int interface) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int interface, int alt_interface) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_release_interface(irecv_client_t client, int interface) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_control_transfer(irecv_client_t client, char request, char subrequest, int something, int index, unsigned char* buffer, unsigned int size, int timeout) {
	IOReturn err;
	
	IOUSBDevRequest req;
	req.bmRequestType = request;
	req.bRequest = subrequest;
	req.wValue = something;
	req.wIndex = index;
	req.wLength = length;
	req.pData = buffer;
		
	if((err = ((*dev)->DeviceRequest(dev, &req))) != kIOReturnSuccess) {
		fprintf(stderr, "AppleMobileDevice: usb_control_msg error %d\n", err);
		return req.wLenDone;
	}
		
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout) {
	(*interface)->WritePipe(interface, pipeRef, buffer, maxsize);

	return IRECV_E_SUCCESS;
}
