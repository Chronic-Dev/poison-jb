#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <setupapi.h>

#include "winusb.h"
//#include "libirecovery.h"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

static const GUID GUID_DEVINTERFACE_DFU = {0xB8085869L, 0xFEB9, 0x404B, {0x8C, 0xB1, 0x1E, 0x5C, 0x14, 0xFA, 0x8C, 0x54}};
static const GUID GUID_DEVINTERFACE_IBOOT = {0xED82A167L, 0xD61A, 0x4AF6, {0x9A, 0xB6, 0x11, 0xE5, 0x22, 0x36, 0xC5, 0x76}};

irecv_client_t usb_init() {
	irecv_client_t client = (irecv_client_t) malloc(sizeof(struct AppleMobileDevice));

	client->iBootPath = NULL;

	SP_DEVICE_INTERFACE_DATA currentInterface;
	HDEVINFO usbDevices;

	// Get iBoot path
	usbDevices = SetupDiGetClassDevs(&GUID_DEVINTERFACE_IBOOT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if(!usbDevices) {
		fprintf(stderr, "winusb: SetupDiGetClassDevs error %d\n", GetLastError());
		return;
	}

	currentInterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	for(DWORD i = 0; SetupDiEnumDeviceInterfaces(usbDevices, NULL, &GUID_DEVINTERFACE_IBOOT, i, &currentInterface); i++) {
		DWORD requiredSize = 0;
		PSP_DEVICE_INTERFACE_DETAIL_DATA details;

		SetupDiGetDeviceInterfaceDetail(usbDevices, &currentInterface, NULL, 0, &requiredSize, NULL);
		details = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(requiredSize);

		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if(!SetupDiGetDeviceInterfaceDetail(usbDevices, &currentInterface, details, requiredSize, NULL, NULL)) {
			fprintf(stderr, "winusb: SetupDiGetDeviceInterfaceDetail error %d\n", GetLastError());
			free(details);
		} else {
			TCHAR* result = (TCHAR*) malloc(requiredSize - sizeof(DWORD));
			memcpy((void*) result, details->DevicePath, requiredSize - sizeof(DWORD));
			free(details);

			if(_tcsstr(result, path) == NULL)
				continue;

			client->iBootPath = result;

			break;
		}
	}

	SetupDiDestroyDeviceInfoList(usbDevices);

	// Get DFU paths
	usbDevices = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DFU, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if(!usbDevices) {
		fprintf(stderr, "winusb: SetupDiGetClassDevs error %d\n", GetLastError());
		return;
	}

	currentInterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	for(DWORD i = 0; SetupDiEnumDeviceInterfaces(usbDevices, NULL, &GUID_DEVINTERFACE_DFU, i, &currentInterface); i++) {
		DWORD requiredSize = 0;
		PSP_DEVICE_INTERFACE_DETAIL_DATA details;

		SetupDiGetDeviceInterfaceDetail(usbDevices, &currentInterface, NULL, 0, &requiredSize, NULL);
		details = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(requiredSize);

		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if(!SetupDiGetDeviceInterfaceDetail(usbDevices, &currentInterface, details, requiredSize, NULL, NULL)) {
			fprintf(stderr, "winusb: SetupDiGetDeviceInterfaceDetail error %d\n", GetLastError());
			free(details);
		} else {
			TCHAR* result = (TCHAR*) malloc(requiredSize - sizeof(DWORD));
			memcpy((void*) result, details->DevicePath, requiredSize - sizeof(DWORD));
			free(details);

			if(_tcsstr(result, path) == NULL)
				continue;

			client->DfuPath = result;

			break;
		}
	}

	SetupDiDestroyDeviceInfoList(usbDevices);

	client->DfuPipePath = (TCHAR*) malloc((_tcslen(client->DfuPath) + 10) * sizeof(TCHAR));
	wsprintf(client->DfuPipePath, TEXT("%s\\PIPE%d"), client->DfuPath, 0);

	client->hIB = NULL;
	client->hDFU = NULL;
	client->hDFUPipe = NULL;

	return IRECV_E_SUCCESS;
}

void usb_exit(irecv_client_t client) {
	free(client->iBootPath);
	free(client->DfuPath);
	free(client->DfuPipePath);

	usb_close(client);
	free(client);
}

irecv_error_t usb_open(irecv_client_t client, irecv_device_t device) {
	BOOL ret = TRUE;
	usb_close(client);

	if(client->iBootPath && !(client->hIB = CreateFile(client->iBootPath,
										GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,
										OPEN_EXISTING,
										0,
										NULL))) {
		fprintf(stderr, "winusb: CreateFile error %d\n", GetLastError());
		ret = FALSE;
	}
	if(client->DfuPath && !(client->hDFU = CreateFile(client->DfuPath,
										GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,
										OPEN_EXISTING,
										0,
										NULL))) {
		fprintf(stderr, "winusb: CreateFile error %d\n", GetLastError());
		ret = FALSE;
	}
	if(client->DfuPipePath && !(client->hDFUPipe = CreateFile(client->DfuPipePath,
												GENERIC_READ | GENERIC_WRITE,
												FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,
												OPEN_EXISTING,
												0,
												NULL))) {
		fprintf(stderr, "winusb: CreateFile error %d\n", GetLastError());
		ret = FALSE;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_close(irecv_client_t client) {
	BOOL ret = TRUE;

	if(client->hIB) {
		if(!CloseHandle(client->hIB)) {
			fprintf(stderr, "winusb: CloseHandle error %d\n", GetLastError());
			ret = FALSE;
		}
		client->hIB = NULL;
	}
	if(client->hDFU) {
		if(!CloseHandle(client->hDFU)) {
			fprintf(stderr, "winusb: CloseHandle error %d\n", GetLastError());
			ret = FALSE;
		}
		client->hDFU = NULL;
	}
	if(client->hDFUPipe) {
		if(!CloseHandle(client->hDFUPipe)) {
			fprintf(stderr, "winusb: CloseHandle error %d\n", GetLastError());
			ret = FALSE;
		}
		client->hDFUPipe = NULL;
	}

	return IRECV_E_SUCCESS;
}

irecv_error_t usb_reset_device(irecv_client_t client) {
	DWORD count;
	int ret = DeviceIoControl(client->hDFU, 0x22000C, NULL, 0, NULL, 0, &count, NULL);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_set_debug_level(irecv_client_t client, int debug) {
	return IRECV_E_SUCCESS;
}

int usb_get_device_list(irecv_client_t client, irecv_device_t** devices) {
	return 0;
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

irecv_error_t usb_control_transfer(irecv_client_t client, char type, char request, int value, int index, unsigned char* data, unsigned int length, int timeout) {
	if (data == NULL) length = 0;

	usb_control_request* packet = (usb_control_request*) malloc(sizeof(usb_control_request) + length);
	packet->bmRequestType = type;
	packet->bRequest = request;
	packet->wValue = value;
	packet->wIndex = index;
	packet->wLength = length;

	DWORD count = 0;

	if (type >= 0x80) {
		if(!ReadFile(client->hDFUPipe, packet, sizeof(usb_control_request) + length, &count, NULL)) {
			fprintf(stderr, "winusb: ReadFile error %d\n", GetLastError());
			free(packet);
			return -1;
		}
		memcpy(data, packet->data, length);

	} else {
		memcpy(packet->data, data, length);
		if(!WriteFile(client->hDFUPipe, packet, sizeof(usb_control_request) + length, &count, NULL)) {
			fprintf(stderr, "winusb: WriteFile error %d\n", GetLastError());
			free(packet);
			return -1;
		}
	}

	free(packet);
	return IRECV_E_SUCCESS;
}

irecv_error_t usb_bulk_transfer(irecv_client_t client, char request, unsigned char* buffer, int maxsize, unsigned int* size, int timeout) {
	return IRECV_E_SUCCESS;
}


