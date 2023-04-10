/*
 * Copyright 2022 Enrique Medina Gremaldos <quiqueiii@gmail.com>
 * Distributed under the terms of the MIT license.
 */

#ifndef HID_RAW_PROTOCOL_HANDLER_H
#define HID_RAW_PROTOCOL_HANDLER_H


#include <InterfaceDefs.h>

#include "ProtocolHandler.h"

#include <condition_variable.h>

class HIDDevice;
class HIDCollection;
class HIDReportItem;


class RawProtocolHandler : public ProtocolHandler {
public:
							RawProtocolHandler(HIDDevice &device);

	static	void			AddHandlers(HIDDevice &device,
									ProtocolHandler *&handlerList);

	virtual	status_t		Control(uint32 *cookie, uint32 op, void *buffer,
									size_t length);

	virtual	status_t		Read(uint32 *cookie, off_t position,
									void *buffer, size_t *numBytes);

	virtual	status_t		Close(uint32 *cookie);

	virtual	void			TransferCallback(uint8 *buffer, size_t length);

private:
	status_t				_ReadReport(void *buffer, uint32 *cookie);
			//HIDReport &		fReport;
			HIDDevice &		fDevice;
			int32	fBusy;
			uint8	fReportBuffer[64];
			size_t	fReportLength;
			ConditionVariable		fConditionVariable;
};

#endif
