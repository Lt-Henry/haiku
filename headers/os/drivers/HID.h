
#ifndef _HID_H_
#define _HID_H_

#include "device_manager.h"

#define B_HID_MODULE_NAME "drivers/input/hid/v1"

enum hid_bus {
	B_HID_BUS_VIRTUAL = 0,
	B_HID_BUS_USB,
	B_HID_BUS_I2C,
	B_HID_BUS_SPI,
	B_HID_BUS_PS2,
	B_HID_BUS_NONE = 64
};

struct hid_driver_info {
	status_t (*get_device_descriptor)(void);
};

struct hid_module_info {
	module_info module;
	status_t (*add_device)(uint32 bus);
	status_t (*remove_device)(uint32 bus);
};

/*
struct hid_module_info {
	module_info module;
	status_t (*register_bus)(uint32 bus);
	status_t (*unregister_bus)(uint32 bus);
	status_t (*add_device)(uint32 bus,void *data);
	status_t (*find_device)(uint32 bus,const char *name,void **device);
	status_t (*delete_device)(uint32 bus,void* data);
	status_t (*publish_devices)(uint32 bus,const char ***names);
	int32 (*count_devices)(uint32 bus);
	void * (*device_at)(uint32 bus, int32 index);
};
*/

enum hid_ioctrl {
	B_HID_IO_GET_TYPE = B_DEVICE_OP_CODES_END,
	B_HID_IO_GET_VID_PID,
	B_HID_IO_READ,
	B_HID_IO_GET_REPORT_DESCRIPTOR,
	B_HID_IO_GET_FEATURE,
	B_HID_IO_SET_FEATURE
};

#endif
