/*
 * Copyright 2005-2009 Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * USB hid raw device driver
 *
 * Authors (in chronological order):
 *		Lt_Henry
 */

#include "DeviceList.h"
#include "USBHIDDevice.h"

#include <KernelExport.h>
#include <Drivers.h>
#include <USB3.h>
#include <util/kernel_cpp.h>
#include <util/Vector.h>
#include <lock.h>

#include <stdio.h>

#define TRACE(x...)	dprintf("hid" ": " x)

#define USB_INTERFACE_CLASS_HID			3
#define USB_DEFAULT_CONFIGURATION		0


int32 api_version = B_CUR_DRIVER_API_VERSION;

usb_module_info *gUSBModule;
static int32 sParentCookie = 0;
static mutex sDriverLock;
DeviceList *gDeviceList = NULL;

/* usb module hooks */

status_t
usb_hid_device_added(usb_device device, void **cookie)
{
	TRACE("device_added\n");
	bool deviceFound = false;

	const usb_device_descriptor *deviceDescriptor
		= gUSBModule->get_device_descriptor(device);

	TRACE("vendor id: 0x%04x; product id: 0x%04x\n",
		deviceDescriptor->vendor_id, deviceDescriptor->product_id);

	const usb_configuration_info *config
		= gUSBModule->get_nth_configuration(device, USB_DEFAULT_CONFIGURATION);
	if (config == NULL) {
		TRACE("cannot get default configuration\n");
		return B_ERROR;
	}

	// ensure default configuration is set
	status_t result = gUSBModule->set_configuration(device, config);
	if (result != B_OK) {
		TRACE("set_configuration() failed 0x%08" B_PRIx32 "\n", result);
		return result;
	}

	// refresh config
	config = gUSBModule->get_configuration(device);
	if (config == NULL) {
		TRACE("cannot get current configuration\n");
		return B_ERROR;
	}

	for (size_t i = 0; i < config->interface_count; i++) {
		const usb_interface_info *interface = config->interface[i].active;
		uint8 interfaceClass = interface->descr->interface_class;
		TRACE("interface %" B_PRIuSIZE ": class: %u; subclass: %u; protocol: "
			"%u\n",	i, interfaceClass, interface->descr->interface_subclass,
			interface->descr->interface_protocol);

		if (interfaceClass == USB_INTERFACE_CLASS_HID) {
			mutex_lock(&sDriverLock);
			
			HIDDevice* hidDevice = NULL;
			
			hidDevice = new(std::nothrow) USBHIDDevice();
			if (hidDevice == NULL) {
				TRACE("no memory for hidDevice\n");
				mutex_unlock(&sDriverLock);
				return B_ERROR;
			}
			
			hidDevice->SetParentCookie(sParentCookie);
			
			char path[128];
			sprintf(path,"/dev/input/hid/usb%" B_PRId32,sParentCookie);
			TRACE("publishing HID device %s\n",path);
			gDeviceList->AddDevice(path, hidDevice);
			*cookie = (void *)(addr_t)sParentCookie;
			sParentCookie++;
			deviceFound=true;
			mutex_unlock(&sDriverLock);
		}

	}

	if (!deviceFound)
		return B_ERROR;

	return B_OK;
}

status_t
usb_hid_device_removed(void *cookie)
{
	mutex_lock(&sDriverLock);
	int32 parentCookie = (int32)(addr_t)cookie;
	TRACE("device_removed(%" B_PRId32 ")\n", parentCookie);
	
	for (int32 i = 0; i < gDeviceList->CountDevices(); i++) {
		HIDDevice* device = (HIDDevice *)gDeviceList->DeviceAt(i);
		
		if (!device)
			continue;
			
		if (device->ParentCookie() == parentCookie) {
			gDeviceList->RemoveDevice(NULL,device);
			break;
		}
	}

	mutex_unlock(&sDriverLock);

	return B_OK;
}

/* device hooks */

static status_t
usb_hid_open(const char *name, uint32 flags, void **_cookie)
{
	return B_OK;
}

static status_t
usb_hid_read(void *_cookie, off_t position, void *buffer, size_t *numBytes)
{
	return B_OK;
}

static status_t
usb_hid_write(void *_cookie, off_t position, const void *buffer,
	size_t *numBytes)
{
	return B_OK;
}

static status_t
usb_hid_control(void *_cookie, uint32 op, void *buffer, size_t length)
{
	return B_OK;
}

static status_t
usb_hid_close(void *_cookie)
{
	return B_OK;
}

static status_t
usb_hid_free(void *_cookie)
{
	return B_OK;
}

/* driver hooks */

status_t
init_hardware(void)
{
	TRACE("init_hardware\n");
	return B_OK;
}


const char **
publish_devices(void)
{
	TRACE("publish_devices\n");
	const char **publishList = gDeviceList->PublishDevices();

	int32 index = 0;
	while (publishList[index] != NULL) {
		TRACE("publishing %s\n", publishList[index]);
		index++;
	}

	return publishList;
}


device_hooks *
find_device(const char *name)
{
	TRACE("find_device:%s\n",name);

	static device_hooks hooks = {
		usb_hid_open,
		usb_hid_close,
		usb_hid_free,
		usb_hid_control,
		usb_hid_read,
		usb_hid_write,
		NULL,				/* select */
		NULL				/* deselect */
	};

	return &hooks;
}


status_t
init_driver(void)
{
	TRACE("init_driver\n");
	if (get_module(B_USB_MODULE_NAME, (module_info **)&gUSBModule) != B_OK)
		return B_ERROR;

	gDeviceList = new(std::nothrow) DeviceList();
	if (gDeviceList == NULL) {
		put_module(B_USB_MODULE_NAME);
		return B_NO_MEMORY;
	}

	mutex_init(&sDriverLock, "usb hid raw driver lock");

	static usb_support_descriptor genericHIDSupportDescriptor = {
		USB_INTERFACE_CLASS_HID, 0, 0, 0, 0
	};

	gUSBModule->register_driver("usb_hid_raw", &genericHIDSupportDescriptor,
			1, NULL);

	static usb_notify_hooks notifyHooks = {
		&usb_hid_device_added,
		&usb_hid_device_removed
	};

	gUSBModule->install_notify("usb_hid_raw", &notifyHooks);

	return B_OK;
}


void
uninit_driver(void)
{
	TRACE("uninit_driver\n");

	gUSBModule->uninstall_notify("usb_hid_raw");
	put_module(B_USB_MODULE_NAME);
	mutex_destroy(&sDriverLock);

	delete gDeviceList;
}
