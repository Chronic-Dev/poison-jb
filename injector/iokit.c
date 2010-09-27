#include "iokit.h"
#include "libirecovery.h"
#include <CoreFoundation/CoreFoundation.h>

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


irecv_error_t usb_open(irecv_client_t client, void** device) {
	IOReturn res;
	
    IOUSBDeviceInterface182** dev = (IOUSBDeviceInterface182 **) device;
    
	if (!client->dev) {
        client->dev = dev;
		res = (*client->dev)->USBDeviceOpen(client->dev);
        client->handle = client->dev;
        client->serial = calloc(sizeof(char), 255);
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
    irecv_error_t err = IRECV_E_SUCCESS;
    
	if (client->usb_interface != NULL) { 
	    err = ((*client->usb_interface)->USBInterfaceClose(client->usb_interface) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
		client->usb_interface = NULL;
	}
		
	if (client->dev != NULL) {
		err = ((*client->dev)->USBDeviceClose(client->dev) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
		(*client->dev)->Release(client->dev);
		client->dev = NULL;
	}

    return err;
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

int usb_get_device_list(irecv_client_t client, void*** devices) {
	// returns device count
	IOReturn err;
	CFMutableDictionaryRef dict;
	io_iterator_t anIterator;
    unsigned int device_count = 1;
    
    void*** devs = malloc(sizeof(irecv_descriptor_t));
    *devices = NULL;
	
	err = IOServiceGetMatchingServices(master_port, IOServiceMatching(kIOUSBDeviceClassName), &anIterator);
	if(err != kIOReturnSuccess) return 0;

	io_object_t usbDevice;
	while (usbDevice = IOIteratorNext(anIterator)) {
		SInt32 score;
		IOUSBDeviceInterface182 **device = NULL;
		IOCFPlugInInterface **plugInInterface = NULL;
		
		io_connect_t dataPort;
		err = IOServiceOpen(usbDevice, mach_task_self(), 0, &dataPort);

		err = IOCreatePlugInInterfaceForService(
		    usbDevice,
			kIOUSBDeviceUserClientTypeID,
			kIOCFPlugInInterfaceID,
			&plugInInterface,
			&score
		);

		if ((err == kIOReturnSuccess) && (plugInInterface != NULL)) {		    
			HRESULT res = (*plugInInterface)->QueryInterface(
			    plugInInterface, 
			    CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID182),
			    (LPVOID *) &device
			);
			
			(*plugInInterface)->Release(plugInInterface);
			
			if(!res && device) {
                device_count += 1;
                
                devs = realloc(devs, device_count * sizeof(irecv_descriptor_t));
                devs[device_count - 2] = device;
                devs[device_count - 1] = NULL;
			}
		}

		IOObjectRelease(usbDevice);
	}

	IOObjectRelease(anIterator);
	
    *devices = devs;
	return device_count - 1;
}

irecv_error_t usb_get_device_descriptor(void** device, irecv_descriptor_t* descriptor) {
    IOUSBDeviceInterface182 **dev = (IOUSBDeviceInterface182 **) device;
    
    *descriptor = malloc(sizeof(**descriptor));
    (*dev)->GetDeviceVendor(dev, &(*descriptor)->vendor);
    (*dev)->GetDeviceClass(dev, &(*descriptor)->class);
    (*dev)->GetDeviceSubClass(dev, &(*descriptor)->subclass);
    (*dev)->GetDeviceProtocol(dev, &(*descriptor)->protocol);
    (*dev)->GetDeviceProduct(dev, &(*descriptor)->product);

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_free_device_list(void*** devices, int mode) {
    free(devices);
    
	return IRECV_E_SUCCESS;
}

int usb_get_string_descriptor_ascii(irecv_client_t client, unsigned char* buffer, unsigned int size) {
	irecv_error_t ret;
	unsigned short langid = 0;
	unsigned char data[255];
	int di, si;
	memset(data, 0, sizeof(data));
	memset(buffer, 0, size);
	
	ret = usb_control_transfer(client, 0x80, 0x06, (0x03 << 8) | 1, langid, data, sizeof(data), 1000);
	
	if (ret < 0) return ret;
	if (data[1] != 0x03) return IRECV_E_UNKNOWN_ERROR;
	if (data[0] > ret) return IRECV_E_UNKNOWN_ERROR; 
	
	for (di = 0, si = 2; si < data[0]; si += 2) {
		if (di >= (size - 1)) break;
		if (data[si + 1]) {
			/* high byte */
			data[di++] = '?';
        } else {
            data[di++] = data[si];
		}
	}
	data[di] = 0;
	
	return di;
}

irecv_error_t usb_get_configuration(irecv_client_t client, int* configuration) {
    IOReturn err;
    
	if((err = ((*client->dev)->GetConfiguration(client->dev, configuration))) != kIOReturnSuccess) {
		return IRECV_E_UNKNOWN_ERROR;
	}
	return IRECV_E_SUCCESS;	
}

irecv_error_t usb_set_configuration(irecv_client_t client, int configuration) {
	IOReturn err;
	
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
	
	interfaceRequest.bInterfaceClass = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	err = (*client->dev)->CreateInterfaceIterator(client->dev, &interfaceRequest, &iterator);
	if (err != kIOReturnSuccess) return IRECV_E_UNKNOWN_ERROR;
	
	while (iface = IOIteratorNext(iterator)) {
		SInt32 score;
		IOCFPlugInInterface** plugInInterface = NULL;
        IOUSBInterfaceInterface182** currentInterface;
			
		err = IOCreatePlugInInterfaceForService(iface,
			kIOUSBInterfaceUserClientTypeID,
			kIOCFPlugInInterfaceID,
			&plugInInterface,
			&score);
			
		if ((err == kIOReturnSuccess) && (plugInInterface != NULL)) {
			HRESULT res = (*plugInInterface)->QueryInterface(
			    plugInInterface,
			    CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID182),
			    (LPVOID *) &currentInterface
			);
			
			(*plugInInterface)->Release(plugInInterface);
			
			if (!res && currentInterface != NULL) {
				uint8_t num;
				
				if ((*currentInterface)->USBInterfaceOpen(currentInterface) == kIOReturnSuccess) {
					client->usb_interface = currentInterface;
					ret = IRECV_E_SUCCESS;

                    break;
				}
			}
		}

		IOObjectRelease(iface);
	}

	IOObjectRelease(iterator);

	return ret;
}

irecv_error_t usb_set_interface_alt_setting(irecv_client_t client, int iface, int alt_iface) {
    irecv_error_t err;
    
    if (client->usb_interface != NULL) err = ((*client->usb_interface)->SetAlternateInterface(client->usb_interface, iface) == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
    else err = IRECV_E_UNKNOWN_ERROR;
	
	return err;
}

irecv_error_t usb_release_interface(irecv_client_t client, int interface) {
	if (client->usb_interface) (*client->usb_interface)->Release(client->usb_interface);

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_control_transfer(irecv_client_t client, char request, char subrequest, int something, int index, unsigned char* buffer, unsigned int size, int timeout) {
	IOReturn err;
	
	IOUSBDevRequestTO req;
	req.bmRequestType = request;
	req.bRequest = subrequest;
	req.wValue = something;
	req.wIndex = index;
	req.wLength = size;
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
	
    if (client->usb_interface == NULL) return IRECV_E_NO_DEVICE;
	
	if (request & 0x80 != 0 && request != 1) {
		// reading
		err = (*client->usb_interface)->ReadPipeTO(client->usb_interface, request, buffer, size, timeout, timeout);
	} else {
		// writing
		err = (*client->usb_interface)->WritePipeTO(client->usb_interface, request, buffer, *size, timeout, timeout);
		if (err != kIOReturnSuccess) {
			*size = 0;
		} else {
            *size = maxsize;
	    }
	}
	
    fprintf(stderr, "bulk transfer result: 0x%X\n", err);
	
	return (err == kIOReturnSuccess) ? IRECV_E_SUCCESS : IRECV_E_UNKNOWN_ERROR;
}
