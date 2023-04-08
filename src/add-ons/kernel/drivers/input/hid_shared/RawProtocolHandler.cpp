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
	fDevice(device)
{
	TRACE("raw HID handler at %s\n",PublishPath());
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
			info.vid = 0x00;
			info.pid = 0x00;
			//info.id = fReport.ID();
			TRACE("hid info of: %" B_PRIu32 "\n",info.id);

			status_t status = user_memcpy(buffer, &info, sizeof(hid_info));
			return status;
		}
		break;

	}

	return B_ERROR;
}

status_t
RawProtocolHandler::Read(uint32 *cookie, off_t position,void *buffer,
								size_t *numBytes) {
/*
	char	tmp[256];
	
	status_t status = _ReadReport(tmp,cookie);
	uint8 *report = fReport.CurrentReport();
	size_t reportSize = fReport.ReportSize();
	size_t p = 0;
	for (size_t n=0; n<reportSize; n++) {
		p+=snprintf(p + tmp,256-p,"%2x ",report[n]);
	}

	p+=snprintf(tmp+p,256-p,"\n");
	*numBytes = 1 + strlen(tmp);
	status = user_strlcpy((char *)buffer, tmp, *numBytes);
	fReport.DoneProcessing();

	return status;
	*/

	status_t status;

	uint8 *report;
	size_t length;

	if(fDevice.MaybeScheduleTransfer(NULL) != B_OK)
		return B_ERROR;

	fDevice.WaitForTransfer(0,&report,&length);

	char	tmp[256];
	size_t p = 0;
	for (size_t n=0; n<length; n++) {
		p+=snprintf(p + tmp,256-p,"%02x ",report[n]);
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
