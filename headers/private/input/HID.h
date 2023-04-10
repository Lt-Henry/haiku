
#ifndef _HID_H_
#define _HID_H_

#include "device_manager.h"

enum hid_bus {
	B_HID_BUS_VIRTUAL = 0,
	B_HID_BUS_USB,
	B_HID_BUS_I2C,
	B_HID_BUS_SPI,
	B_HID_BUS_PS2,
	B_HID_BUS_NONE = 64
};

enum hid_ioctrl {
	B_HID_IO_GET_TYPE = B_DEVICE_OP_CODES_END,
	B_HID_IO_GET_INFO,
	B_HID_IO_GET_VID_PID,
	B_HID_IO_READ,
	B_HID_IO_GET_REPORT_DESCRIPTOR,
	B_HID_IO_GET_FEATURE,
	B_HID_IO_SET_FEATURE
};

typedef struct {
	uint8 bus;
	uint16 vid;
	uint16 pid;
} hid_info;

#endif
