
#ifndef _HID_DRIVER_H
#define _HID_DRIVER_H

#include <Drivers.h>
#include <KernelExport.h>
#include <OS.h>
#include <HID.h>

#include "DeviceList.h"

#define DRIVER_NAME	"hid"

extern DeviceList *gDeviceList;

#define TRACE(x...)	dprintf(DRIVER_NAME ": " x)

#endif
