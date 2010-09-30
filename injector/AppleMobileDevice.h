#pragma once

#ifndef APPLEMOBILEDEVICE_H
#define APPLEMOBILEDEVICE_H

#include <windows.h>
#include "stdint.h"

enum {
	kDFUMode = 1,
	kRecoveryMode = 2
};

class AppleMobileDevice
{
private:

	TCHAR* iBootPath;
	TCHAR* DfuPath;
	TCHAR* DfuPipePath;

	HANDLE hIB;
	HANDLE hDFU;
	HANDLE hDFUPipe;

	AppleMobileDevice* next;

	static void initializeList();

public:

	static AppleMobileDevice* Enumerate(AppleMobileDevice* last);
	static void WaitFor(int type);

	AppleMobileDevice(const TCHAR* path);

	typedef struct USBControlRequest {
		uint8_t bmRequestType;
		uint8_t bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;

		char data[];
	} USBControlRequest;

	virtual BOOL Open(void);
	virtual BOOL Close(void);

	virtual int Mode();

	virtual void AppleMobileDevice::UsbReset();

	virtual int UsbControlRequest(uint8_t bmRequestType,
									 uint8_t bRequest,
									 uint16_t wValue,
									 uint16_t wIndex,
									 unsigned char *data,
									 uint16_t wLength,
									 unsigned int timeout);

	virtual ~AppleMobileDevice(void);
};

#endif
