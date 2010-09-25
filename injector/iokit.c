#include "iokit.h"
#include "libirecovery.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

mach_port_t master_port;

irecv_error_t usb_init() {
	IOReturn err;
	
	if(!master_port) {
		err = IOMasterPort(MACH_PORT_NULL, &master_port);
		if(err != kIOReturnSuccess)
			return IRECV_E_UNKNOWN_ERROR;
	}
	
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_exit() {
	return IRECV_E_SUCCESS;
}


irecv_error_t usb_open(irecv_client_t client, irecv_device_t device) {
	IOReturn res;
	if (!client->dev) {
		res = (*client->dev)->USBDeviceOpen(client->dev);
	} else {
		return IRECV_E_SUCCESS;
	}
	
	if (res == kIOReturnSuccess) {
		return IRECV_E_SUCCESS;
	} else {
		return IRECV_E_UNABLE_TO_CONNECT;
	}
}

irecv_error_t usb_close(irecv_client_t client) {
	if (client->dev) {
		(*client->interface)->USBInterfaceClose(client->interface);
		client->interface = NULL;
		irecv_error_t err = ((*client->dev)->USBDeviceClose(client->dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
		(*client->dev)->Release(client->dev);
		client->dev = NULL;
		return err;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_reset_device(irecv_client_t client) {
	if (client->dev) {
		return ((*client->dev)->ResetDevice(client->dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
	}
	
	return IRECV_E_SUCCESS; // fix the return code
}

irecv_error_t usb_set_debug_level(irecv_client_t client, int debug) {
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_device_list(irecv_client_t client, irecv_device_t** devices) {
	// returns device count
	IOReturn err;
	CFMutableDictionaryRef dict;
	io_iterator_t anIterator;
	
	err = IOServiceGetMatchingServices(master_port, IOServiceMatching(kIOUSBDeviceClassName), &anIterator);
	if(err != kIOReturnSuccess) return IRECV_E_UNKNOWN_ERROR;

	io_object_t usbDevice;
	while(usbDevice = IOIteratorNext(anIterator)) {
		SInt32 score;
		IOUSBDeviceInterface **device = NULL;
		IOCFPlugInInterface **plugInInterface = NULL;

		err = IOCreatePlugInInterfaceForService(usbDevice,
				kIOUSBDeviceUserClientTypeID,
				kIOCFPlugInInterfaceID,
				&plugInInterface,
				&score);

		if ((kIOReturnSuccess == err) && (plugInInterface != NULL) ) {
			HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID*)&device);
			(*plugInInterface)->Release(plugInInterface);
			if(!res && device) {
				// add found device to list
			}
		}

		IOObjectRelease(usbDevice);
	}

	IOObjectRelease(anIterator);
	
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_device_descriptor(irecv_device_t device) {


	return IRECV_E_SUCCESS;
}

irecv_error_t usb_free_device_list(irecv_device_t* devices, int mode) {

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_get_string_descriptor_ascii(irecv_client_t client, unsigned char* buffer, unsigned int size) {
	irecv_error_t ret;
	unsigned short langid = 0;
	unsigned char data[255];
	int di, si;
	memset(data, 0, sizeof(data));
	memset(buffer, 0, size);
	
	ret = usb_control_transfer(client, 0x80, 0x06, (0x03 << 8) | desc_index, langid, data, sizeof(data), 1000);
	
	if (ret < 0) return ret;
	if (data[1] != 0x03) return IRECV_E_UNKNOWN_ERROR;
	if (data[0] > ret) return IRECV_E_UNKNOWN_ERROR; 
	
	for (di = 0, si = 2; si < data[0]; si += 2) {
		if (di >= (size - 1)) break;
		if (data[si + 1]) {
			/* high byte */
			data[di++] = '?';
		} else {
			data[di++] = data[si];
		}
	}
	data[di] = 0;
	return di;
}

irecv_error_t usb_get_configuration(irecv_client_t client, int* configuration) {
	if((err = ((*client->dev)->GetConfiguration(client->dev, configuration))) != kIOReturnSuccess) {
		return IRECV_E_UNKNOWN_ERROR;
	}
	return IRECV_E_SUCCESS;	
}

irecv_error_t usb_set_configuration(irecv_client_t client, int configuration) {
	if((err = ((*client->dev)->SetConfiguration(client->dev, configuration))) != kIOReturnSuccess) {
		return IRECV_E_UNKNOWN_ERROR;
	}
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_claim_interface(irecv_client_t client, int interface) {
	irecv_error_t ret = IRECV_E_UNKNOWN_ERROR;
	IOReturn err;	
	IOUSBFindInterfaceRequest interfaceRequest;
	io_iterator_t iterator;
	io_service_t iface;
	
	interfaceRequest.bInterfaceClass = 255;
	interfaceRequest.bInterfaceSubClass = 255;
	interfaceRequest.bInterfaceProtocol = interface;
	interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	err = (*client->dev)->CreateInterfaceIterator(client->dev, &interfaceRequest, &iterator);
	if (err!=kIOReturnSuccess) return IRECV_E_UNKNOWN_ERROR;

	while(iface = IOIteratorNext(iterator)) {
		SInt32 score;
		IOCFPlugInInterface **plugInInterface = NULL;
		IOUSBInterfaceInterface182** currentInterface
			
		err = IOCreatePlugInInterfaceForService(iface,
			kIOUSBInterfaceUserClientTypeID,
			kIOCFPlugInInterfaceID,
			&plugInInterface,
			&score);

		if ((kIOReturnSuccess == err) && (plugInInterface != NULL) ) {
			HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID*)&currentInterface);
			(*plugInInterface)->Release(plugInInterface);
			if(!res && currentInterface) {
				uint8_t num;
				if((*currentInterface)->USBInterfaceOpen(currentInterface) == kIOReturnSuccess) {
					ret = IRECV_E_SUCCESS;
					client->interface = currentInterface;
				}
			}
		}

		IOObjectRelease(iface);
	}

	IOObjectRelease(iterator);

	return ret;
}

irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int iface, int alt_iface) {
	if (client->interface) (*client->interface)->SetAlternateInterface(client->interface, iface);
		
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_release_interface(irecv_client_t client, int interface) {
	if (client->interface) (*client->interface)->Release(client->interface);

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_control_transfer(irecv_client_t client, char request, char subrequest, int something, int index, unsigned char* buffer, unsigned int size, int timeout) {
	IOReturn err;
	
	IOUSBDevRequestTO req;
	req.bmRequestType = request;
	req.bRequest = subrequest;
	req.wValue = something;
	req.wIndex = index;
	req.wLength = length;
	req.pData = buffer;
	req.noDataTimeout = timeout; 
    req.completionTimeout = timeout; 
		
	if((err = ((*client->dev)->DeviceRequestTO(client->dev, &req))) != kIOReturnSuccess) {
		return IRECV_E_UNKNOWN_ERROR;
	}
		
	return req.wLenDone;
}

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout) {
	IOReturn err;
	
	if (request & 0x80 != 0) {
		// reading
		err = (*client->interface)->ReadPipeTO (client->interface, request, buffer, size, timeout, timeout);
	} else {
		// writing
		err = (*client->interface)->WritePipeTO (client->interface, request, buffer, *size, timeout, timeout);
		if (err!=kIOReturnSuccess) {
			*size = 0;
		}
	}
	return (err==kIOReturnSuccess)?IRECV_E_SUCCESS:IRECV_E_UNKNOWN_ERROR;
}
