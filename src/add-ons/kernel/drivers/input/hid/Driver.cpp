
#include "Driver.hpp"

#include <lock.h>
#include <util/AutoLock.h>

#include <new>
#include <stdio.h>
#include <string.h>

DeviceList *gDeviceList = NULL;
static mutex sDriverLock;


/* device */

/* driver */
static device_hooks sDeviceHooks = {
	device_open,
	device_close,
	device_free,
	device_control,
	device_read,
	device_write
};

device_hooks*
find_device(const char* name)
{
	return &sDeviceHooks;
}

/* module */

static status_t
std_ops(int32 op, ...)
{

	switch (op) {
		case B_MODULE_INIT:
			TRACE("module_init\n");
			gDeviceList = new(std::nothrow) DeviceList();
			if (gDeviceList == NULL) {
				return B_NO_MEMORY;
			}
			mutex_init(&sDriverLock, "hid driver lock");
			return B_OK;

		case B_MODULE_UNINIT:
			TRACE("module_uninit\n");
			delete gDeviceList;
			gDevicesList = NULL;
			mutex_destroy(&sDriverLock);
			return B_OK;

		default:
			break;
	}

	return B_ERROR;
}


status_t
add_device(uint32 bus)
{
	return B_OK;
}

status_t
remove_device(uint32 bus)
{
	return B_OK;
}

struct hid_module_info hid_driver_module = {
	{
		B_HID_MODULE_NAME,
		0,
		&std_ops
	},
	add_device,
	remove_device
};


module_info *modules[] = {
	(module_info *)&hid_driver_module,
	NULL
};
