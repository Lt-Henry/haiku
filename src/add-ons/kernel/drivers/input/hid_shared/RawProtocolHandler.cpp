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


RawProtocolHandler::RawProtocolHandler(HIDReport &report) :
	ProtocolHandler(report.Device(), "input/hid/" DEVICE_PATH_SUFFIX "/", 0),
	fReport(report)
{
	TRACE("raw HID handler at %s\n",PublishPath());
}

void
RawProtocolHandler::AddHandlers(HIDDevice &device, HIDCollection &collection,
	ProtocolHandler *&handlerList)
{
	HIDParser &parser = device.Parser();
	uint32 maxReportCount = parser.CountReports(HID_REPORT_TYPE_INPUT);
	if (maxReportCount == 0)
		return;

	uint32 inputReportCount = 0;
	HIDReport *inputReports[maxReportCount];
	collection.BuildReportList(HID_REPORT_TYPE_INPUT, inputReports,
		inputReportCount);

	for (uint32 i = 0; i < inputReportCount; i++) {
		HIDReport *inputReport = inputReports[i];

		ProtocolHandler *newHandler = new(std::nothrow) RawProtocolHandler(
			*inputReport);

		if (newHandler == NULL) {
			TRACE("failed to allocated raw protocol handler\n");
			continue;
		}

		newHandler->SetNextHandler(handlerList);
		handlerList = newHandler;
	}
}

status_t
RawProtocolHandler::Control(uint32 *cookie, uint32 op, void *buffer,
									size_t length) {

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
			info.id = fReport.ID();

			status_t status = user_strlcpy((char *)buffer, (char *)&info, sizeof(hid_info));
			return status;
		}
		break;

	}

	return B_ERROR;
}

status_t
RawProtocolHandler::Read(uint32 *cookie, off_t position,void *buffer,
								size_t *numBytes) {

	char	tmp[256];
	//TRACE("read report\n");
	status_t status = _ReadReport(tmp,cookie);
	uint8 *report = fReport.CurrentReport();
	size_t reportSize = fReport.ReportSize();
	size_t p = snprintf(tmp,256,"ID:%" B_PRIu32 " report size:%" B_PRIuSIZE "\n",fReport.ID(),reportSize);

	for (size_t n=0; n<reportSize; n++) {
		p+=snprintf(tmp+p,256-p,"%x ",report[n]);
	}

	p+=snprintf(tmp+p,256-p,"\n");
	*numBytes = 1 + strlen(tmp);
	status = user_strlcpy((char *)buffer, tmp, *numBytes);
	fReport.DoneProcessing();

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


	return B_OK;
}
