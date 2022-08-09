

#ifndef _USB_HID_DEVICE_H_
#define _USB_HID_DEVICE_H_

#include "HIDDevice.h"

class USBHIDDevice: public HIDDevice
{
	public:

	USBHIDDevice() : HIDDevice(B_HID_BUS_USB) {}
};

#endif
