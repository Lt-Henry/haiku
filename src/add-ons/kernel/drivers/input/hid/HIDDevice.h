
#ifndef _HID_DEVICE_H_
#define _HID_DEVICE_H_

#include <OS.h>

enum HIDBus
{
	B_HID_BUS_NONE,
	B_HID_BUS_VIRTUAL,
	B_HID_BUS_USB,
	B_HID_BUS_I2C,
	B_HID_BUS_SPI
};

class HIDDevice
{
	public:
	
	HIDDevice(HIDBus bus): fBus(bus){}
	virtual ~HIDDevice(){};
	
	void				SetParentCookie(int32 cookie) { fParentCookie = cookie; };
	int32				ParentCookie() const { return fParentCookie; }
	
	HIDBus				Bus() { return fBus; }
	
	protected:

	HIDBus				fBus;
	int32				fParentCookie;
};

#endif
