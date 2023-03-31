
#include "Driver.h"

#include <lock.h>
#include <util/AutoLock.h>

#include <new>
#include <stdio.h>
#include <string.h>

int32 api_version = B_CUR_DRIVER_API_VERSION;

DeviceList *gDeviceList = NULL;
static mutex sDriverLock;


// device hooks

static status_t
device_open(const char *dname, uint32 flags, void** cookie)
{
	return B_ERROR;
}

static status_t
device_close (void *cookie)
{
	return B_ERROR;
}

static status_t
device_free(void *cookie)
{
	return B_ERROR;
}

static status_t
device_control (void *cookie, uint32 msg, void *arg1, size_t len)
{
	return B_ERROR;
}

static status_t
device_read(void* cookie, off_t pos, void* buf, size_t* count)
{
	return B_ERROR;
}

static status_t
device_write(void *cookie, off_t pos, const void *buf, size_t *count)
{
	return B_ERROR;
}

// driver hooks

status_t
init_hardware(void)
{
	TRACE("init_hardware\n");
	return B_OK;
}

status_t
init_driver(void)
{
	TRACE("init_driver\n");
	return B_OK;
}

void
uninit_driver(void)
{
	TRACE("uninit_driver\n");
}

const char**
publish_devices()
{
	return NULL;
}

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

// module hooks

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
			gDeviceList = NULL;
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
	TRACE("add_device:%" B_PRIu32 "\n",bus);
	return B_OK;
}

status_t
remove_device(uint32 bus)
{
	TRACE("remove_device:%" B_PRIu32 "\n",bus);
	return B_OK;
}

struct hid_module_info hid_driver_module = {
	{
		B_HID_MODULE_NAME,
		B_KEEP_LOADED,
		&std_ops
	},
	add_device,
	remove_device
};

module_info *modules[] = {
	(module_info *)&hid_driver_module,
	NULL
};
