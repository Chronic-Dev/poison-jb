#include "libusb1.h"
#include "libirecovery.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

IOUSBDeviceInterface182** dev;
IOUSBInterfaceInterface182** interface;

irecv_error_t usb_init() {
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_exit() {
	return IRECV_E_SUCCESS;
}


irecv_error_t usb_open(irecv_client_t client, irecv_device_t device) {
	IOReturn res;
	res = (*dev)->USBDeviceOpen(dev);
	
	if (res == kIOReturnSuccess) {
		return IRECV_E_SUCCESS;
	} else {
		return IRECV_E_UNABLE_TO_CONNECT;
	}
}

irecv_error_t usb_close(irecv_client_t client) {
	if (opened) {
		opened = FALSE;
		(*interface)->USBInterfaceClose(interface);
		irecv_error_t err = ((*dev)->USBDeviceClose(dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
		(*dev)->Release(dev);
		return err;
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
	kern_return_t res;
	mach_port_t port;
	CFMutableDictionaryRef _serviceDict;
	CFNumberRef _usbID;
	io_iterator_t usbIterator;
	io_service_t usbItem;
	int _id;
	
	res = IOMasterPort(MACH_PORT_NULL, &port);
	_serviceDict = IOServiceMatching(kIOUSBDeviceClassName);
	
	// vendor id
	_id = device.descriptor->vendor;
	_usbID = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &_id);
	CFDictionarySetValue(_serviceDict, CFSTR(kUSBVendorID), _usbID);
	CFRelease(_usbID);
	
	// product id
	_id = device.descriptor->product; 
	_usbID = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &_id);
	CFDictionarySetValue(_serviceDict, CFSTR(kUSBProductID), _usbID);
	CFRelease(_usbID);
	
	IOServiceGetMatchingServices(port, _serviceDict, &usbIterator);
	
	while(usbItem = IOIteratorNext(usbIterator)) {
		printf("==> IOUSBDevice at 0x%X\n", (unsigned int)usbItem);
	}


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
	irecv_error_t ret = IRECV_E_UNKNOWN_ERROR;
	
	IOUSBFindInterfaceRequest interfaceRequest;
	io_iterator_t iterator;
	io_service_t iface;
	IOCFPlugInInterface **iodev;

	interfaceRequest.bInterfaceClass = 255;
	interfaceRequest.bInterfaceSubClass = 255;
	interfaceRequest.bInterfaceProtocol = interface;
	interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	ret = (*dev)->CreateInterfaceIterator(dev, &interfaceRequest, &iterator);
	if(ret)
		return 0;

	while(iface = IOIteratorNext(iterator)) {
		SInt32 score;
		IOCFPlugInInterface **plugInInterface = NULL;

		ret = IOCreatePlugInInterfaceForService(iface,
			kIOUSBInterfaceUserClientTypeID,
			kIOCFPlugInInterfaceID,
			&plugInInterface,
			&score);

		if ((kIOReturnSuccess == ret) && (plugInInterface != NULL) ) {
			HRESULT res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID*)&interface);
			(*plugInInterface)->Release(plugInInterface);
			if(!res && interface) {
				uint8_t num;
				if((*interface)->USBInterfaceOpen(interface) == kIOReturnSuccess) {
					ret = IRECV_E_SUCCESS;
				}
			}
		}

		IOObjectRelease(iface);
	}

	IOObjectRelease(iterator);

	return ret;
}

irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int interface, int alt_interface) {
	if (interface) (*interface)->SetAlternateInterface(interface, iface);
		
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_release_interface(irecv_client_t client, int interface) {
	if (interface) (*interface)->Release(interface);

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
		
	if((err = ((*dev)->DeviceRequestTO(dev, &req))) != kIOReturnSuccess) {
		return IRECV_E_UNKNOWN_ERROR;
	}
		
	return req.wLenDone;
}

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout) {
	IOReturn err;
	
	if (request & 0x80 != 0) {
		// reading
		err = (*interface)->ReadPipeTO (interface, request, buffer, size, timeout, timeout);
	} else {
		// writing
		err = (*interface)->WritePipeTO (interface, request, buffer, *size, timeout, timeout);
		if (err!=kIOReturnSuccess) {
			*size = 0;
		}
	}
	return (err==kIOReturnSuccess)?IRECV_E_SUCCESS:IRECV_E_UNKNOWN_ERROR;
}
