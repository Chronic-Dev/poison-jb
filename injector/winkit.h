#ifndef USB_H
#define USB_H

#include "libirecovery.h"

#include "AppleMobileDevice.h"


irecv_error_t usb_init();
irecv_error_t usb_exit();

irecv_error_t usb_open(irecv_client_t client, void** device);
irecv_error_t usb_close(irecv_client_t client);

irecv_error_t usb_reset_device(irecv_client_t client);
irecv_error_t usb_set_debug_level(irecv_client_t client, int debug);

int usb_get_device_list(irecv_client_t client, void*** devices);
irecv_error_t usb_get_device_descriptor(void** device, irecv_descriptor_t* descriptor);
irecv_error_t usb_free_device_list(void*** devices, int mode);

int usb_get_string_descriptor_ascii(irecv_client_t client, unsigned char* buffer, unsigned int size);
irecv_error_t usb_get_configuration(irecv_client_t client, int* configuration);
irecv_error_t usb_set_configuration(irecv_client_t client, int configuration);

irecv_error_t usb_claim_interface(irecv_client_t client, int interface);
irecv_error_t usb_release_interface(irecv_client_t client);
irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int interface, int alt_interface);

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout);
irecv_error_t usb_control_transfer(irecv_client_t client, char request, char subrequest, int something, int index, unsigned char* buffer, unsigned int size, int timeout);

#endif
