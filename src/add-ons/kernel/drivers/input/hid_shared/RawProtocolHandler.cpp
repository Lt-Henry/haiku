/*
 * Copyright 2022 Enrique Medina Gremaldos <quiqueiii@gmail.com>
 * Distributed under the terms of the MIT license.
 */

#include "Driver.h"
#include "RawProtocolHandler.h"

#include "HIDCollection.h"
#include "HIDDevice.h"
#include "HIDReport.h"
#include "HIDReportItem.h"

#include <new>
#include <kernel.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <usb/USB_hid.h>

#include <keyboard_mouse_driver.h>
#include <HID.h>


RawProtocolHandler::RawProtocolHandler(HIDDevice &device) :
	ProtocolHandler(&device, "input/hid/" DEVICE_PATH_SUFFIX "/", 0),
	fDevice(device),
	fBusy(0)
{
	TRACE("raw HID handler at %s\n",PublishPath());
	fConditionVariable.Init(this,"HID raw handler");
}

void
RawProtocolHandler::AddHandlers(HIDDevice &device, ProtocolHandler *&handlerList)
{

	ProtocolHandler *newHandler = new(std::nothrow) RawProtocolHandler(device);

	if (newHandler == NULL) {
		TRACE("failed to allocated raw protocol handler\n");
		return;
	}

	newHandler->SetNextHandler(handlerList);
	handlerList = newHandler;

}

status_t
RawProtocolHandler::Control(uint32 *cookie, uint32 op, void *buffer,
									size_t length) {

	TRACE("ioctl:%" B_PRIu32 "\n",op);
	switch (op) {

		case B_GET_DEVICE_NAME:
		{
			const char name[] = DEVICE_NAME" HID Raw";
			return IOGetDeviceName(name,buffer,length);
		}
		break;

		case B_HID_IO_GET_INFO: {
			hid_info info;
			info.bus = B_HID_BUS_USB;
			info.vid = fDevice.Vendor();
			info.pid = fDevice.Product();
			info.report_size = fDevice.ReportDescriptorLength();
			status_t status = user_memcpy(buffer, &info, sizeof(hid_info));
			return status;
		}
		break;

		case B_HID_IO_GET_REPORT_DESCRIPTOR: {
			size_t reportDescriptorLength = fDevice.ReportDescriptorLength();
			if (reportDescriptorLength > length) {
				return B_BUFFER_OVERFLOW;
			}

			return user_memcpy(buffer, fDevice.ReportDescriptor(), reportDescriptorLength);
		}
		break;
	}

	return B_ERROR;
}

status_t
RawProtocolHandler::Read(uint32 *cookie, off_t position,void *buffer,
								size_t *numBytes) {

	status_t status;
	
	ConditionVariableEntry conditionVariableEntry;
	fConditionVariable.Add(&conditionVariableEntry);
	
	if(fDevice.MaybeScheduleTransfer(NULL) != B_OK) {
		conditionVariableEntry.Wait(B_RELATIVE_TIMEOUT, 0);
		return B_ERROR;
	}
	
	status = conditionVariableEntry.Wait(B_RELATIVE_TIMEOUT, 10000000);
	if (status!=B_OK) {
		return status;
	}
	//atomic_set(&fBusy ,1);

	//while (atomic_get(&fBusy) != 0)
		//snooze(1000);
	
	//fDevice.WaitForTransfer(0,&report,&length);

	char	tmp[256];
	size_t p = 0;
	for (size_t n=0; n<fReportLength; n++) {
		p+=snprintf(p + tmp,256-p,"%02x ",fReportBuffer[n]);
	}

	p+=snprintf(tmp+p,256-p,"\n");
	*numBytes = 1 + strlen(tmp);
	status = user_strlcpy((char *)buffer, tmp, *numBytes);

	return status;
}

status_t
RawProtocolHandler::Close(uint32 *cookie)
{
	TRACE("yolo\n");
	return B_OK;
}

void
RawProtocolHandler::TransferCallback(uint8 *buffer, size_t length)
{
	fReportLength = length;
	//fReportBuffer = buffer;
	memcpy(fReportBuffer,buffer,length);
	//atomic_set(&fBusy,0);
	fConditionVariable.NotifyAll();
}

status_t
RawProtocolHandler::_ReadReport(void *buffer, uint32 *cookie)
{
/*
	status_t result = fReport.WaitForReport(B_INFINITE_TIMEOUT);

	if (result != B_OK) {
		if (fReport.Device()->IsRemoved()) {
			TRACE("device has been removed\n");
			return B_DEV_NOT_READY;
		}

		if ((*cookie & PROTOCOL_HANDLER_COOKIE_FLAG_CLOSED) != 0)
			return B_CANCELED;

		if (result != B_INTERRUPTED) {
			// interrupts happen when other reports come in on the same
			// input as ours
			TRACE_ALWAYS("error waiting for report: %s\n", strerror(result));
		}

		// signal that we simply want to try again
		return B_INTERRUPTED;
	}

*/
	return B_OK;
}
