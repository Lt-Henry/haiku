/*
 * Copyright 2026, Slimbook, dev@slimbook.com.
 * Distributed under the terms of the MIT license.
 */


#define DRIVER_NAME "wmi_qc71"
#include "WMIPrivate.h"
#include "QC71EC.h"

#define ACPI_QC71_WMI_MGMT_GUID		"ABBC0F6F-8EA1-11D1-00A0-C90629100000"
#define ACPI_QC71_WMI_EVENT_GUID	"ABBC0F72-8EA1-11D1-00A0-C90629100000"
#define QC71_WMBC_GETSETULONG_ID	4

class WMIQC71 {
public:
								WMIQC71(device_node *node);
								~WMIQC71();

			status_t			ReadEC(uint16 addr, uint8 *data);
			status_t			WriteEC(uint16 addr, uint8 data);
			
			void ToggleFnLock();
			void TogglePerformanceMode();

private:
	static	void				_NotifyHandler(acpi_handle handle,
									uint32 notify, void *context);
			void				_Notify(acpi_handle handle, uint32 notify);
			
			
private:
			device_node*		fNode;
			wmi_device_interface* wmi;
			wmi_device			wmi_cookie;
			struct mutex		fECLock;
			const char*			fEventGuidString;

};

WMIQC71::WMIQC71(device_node *node)
	:
	fNode(node),
	fEventGuidString(NULL)
{
	CALLED();
	
	mutex_init(&fECLock, "QC71 EC mutex");

	device_node *parent;
	parent = gDeviceManager->get_parent_node(node);
	gDeviceManager->get_driver(parent, (driver_module_info **)&wmi,
		(void **)&wmi_cookie);

	gDeviceManager->put_node(parent);
	
	// install event handler
	if (wmi->install_event_handler(wmi_cookie, ACPI_QC71_WMI_EVENT_GUID,
		_NotifyHandler, this) == B_OK) {
		fEventGuidString = ACPI_QC71_WMI_EVENT_GUID;
	}
	
	uint8 data;
	status_t status = ReadEC(QC71_CTRL_1_ADDR, &data);
	
	if (status == B_OK) {
		status = WriteEC(QC71_CTRL_1_ADDR,data | QC71_CTRL_1_MANUAL_MODE);
		
		if (status!= B_OK)
			TRACE_ALWAYS("failed to set qc71 manual mode\n");
		else
			TRACE_ALWAYS("set qc71 in manual mode\n");
	}
	
}

WMIQC71::~WMIQC71()
{
	mutex_destroy(&fECLock);
}

void
WMIQC71::_Notify(acpi_handle handle, uint32 notify)
{
	CALLED();
	
	acpi_data response = { ACPI_ALLOCATE_BUFFER, NULL };
	wmi->get_event_data(wmi_cookie, notify, &response);

	acpi_object_type* object = (acpi_object_type*)response.pointer;
	uint32 result = 0;
	if (object != NULL) {
		if (object->object_type == ACPI_TYPE_INTEGER)
			result = object->integer.integer;
		free(object);
	}
	
	TRACE_ALWAYS("wmi QC71 key 0x%" B_PRIx32 "\n", result);
	
	switch (result) {
		/* caps lock */
		case 0x01:
		break;
		
		/* num lock */
		case 0x02:
		break;

		/* scroll lock */
		case 0x03:
		break;

		/* touchpad on */
		case 0x04:
		break;

		/* touchpad off */
		case 0x05:
		break;

		/* increase screen brightness */
		case 0x14:
		break;

		/* decrease screen brightness */
		case 0x15:
		break;

		/* radio on */
		case 0x1a:
		break;

		/* radio off */
		case 0x1b:
		break;

		/* mute/unmute */
		case 0x35:
		break;

		/* decrease volume */
		case 0x36:
		break;

		/* increase volume */
		case 0x37:
		break;

		/* lightbar on */
		case 0x39:
		break;

		/* lightbar off */
		case 0x3a:
		break;

		/* keyboard backlight 0% */
		case 0x3b:
		break;

		/* keyboard backlight 25% */
		case 0x3c:
		break;

		/* keyboard backlight 50% */
		case 0x3d:
		break;

		/* keyboard backlight 75% */
		case 0x3e:
		break;

		/* keyboard backlight 100% */
		case 0x3f:
		break;

		/* enable super key (win key) lock */
		case 0x40:
		break;

		/* decrease volume */
		case 0x41:
		break;

		/* enable/disable airplane mode */
		case 0xa4:
		break;

		/* super key (win key) lock state changed */
		case 0xa5:
		break;

		/* lightbar state changed */
		case 0xa6:
		break;

		/* fan boost state changed */
		case 0xa7:
		break;

		/* charger unplugged/plugged in */
		case 0xab:
		break;

		/* perf mode button pressed */
		case 0xb0:
			TRACE("Performance change request\n");
			TogglePerformanceMode();
		break;

		/* increase keyboard backlight */
		case 0xb1:
		break;

		/* decrease keyboard backlight */
		case 0xb2:
		break;

		/* keyboard backlight cycle */
		case 0xb3:
		break;

		/* toggle Fn lock (Fn+ESC)*/
		case 0xb8:
			TRACE("FnLock change requested\n");
			ToggleFnLock();
		break;

		/* perf mode button pressed */
		case 0xbc:
			TRACE("Performance change request\n");
			TogglePerformanceMode();
		break;

		/* webcam toggle on/off */
		case 0xcf:
		break;

		/* keyboard backlight brightness changed */
		case 0xf0:
		break;

	}
	
}

void
WMIQC71::_NotifyHandler(acpi_handle handle, uint32 value, void *context)
{
	WMIQC71* device = (WMIQC71*)context;
	device->_Notify(handle, value);
}

status_t
WMIQC71::ReadEC(uint16 addr, uint8 *data)
{
	uint8 params[] = { 
		(uint8)(addr & 0xFF),
		(uint8)(addr >> 8),
		0,
		0,
		0,
		1,
		0,
		0
	};
	
	MutexLocker locker(fECLock);
	
	acpi_data inBuffer = { sizeof(params), params };
	acpi_data outBuffer = { ACPI_ALLOCATE_BUFFER, NULL };
	
	status_t status = wmi->evaluate_method(wmi_cookie, 0, QC71_WMBC_GETSETULONG_ID, &inBuffer,
		&outBuffer);
		
	if (status != B_OK)
		return status;

	acpi_object_type* object = (acpi_object_type*)outBuffer.pointer;
	
	if (object != NULL) {
		if (object->object_type == ACPI_TYPE_BUFFER) {
			uint8* value = (uint8 *)object->buffer.buffer;
			*data = value[0];
			}
		free(object);
		
		return B_OK;
	}
	return B_ERROR;
}

status_t
WMIQC71::WriteEC(uint16 addr, uint8 data)
{
	uint8 params[] = { 
		(uint8)(addr & 0xFF),
		(uint8)(addr >> 8),
		(uint8)(data & 0xFF),
		0,
		0,
		0,
		0,
		0
	};
	
	MutexLocker locker(fECLock);
	
	acpi_data inBuffer = { sizeof(params), params };
	acpi_data outBuffer = { ACPI_ALLOCATE_BUFFER, NULL };
	
	status_t status = wmi->evaluate_method(wmi_cookie, 0, QC71_WMBC_GETSETULONG_ID, &inBuffer,
		&outBuffer);
		
	if (status != B_OK)
		return status;

	acpi_object_type* object = (acpi_object_type*)outBuffer.pointer;
	
	if (object != NULL) {
		free(object);
		
		return B_OK;
	}
	return B_ERROR;
}


void WMIQC71::ToggleFnLock()
{
	uint8 data;
	status_t status = ReadEC(QC71_BIOS_CTRL_1_ADDR, &data);
	
	if (status != B_OK)
		return;

	uint8 fn = (data ^ QC71_BIOS_CTRL_1_FN_LOCK_STATUS);
	
	TRACE_ALWAYS("Fn lock: %" B_PRIx32 "\n", fn);
	
	status = WriteEC(QC71_BIOS_CTRL_1_ADDR, fn);
	
	if (status != B_OK)
		return;
}


void WMIQC71::TogglePerformanceMode()
{
	uint8 data;
	status_t status = ReadEC(QC71_FAN_CTRL_ADDR, &data);
	
	if (status != B_OK)
		return;
	
	uint8 mask = (QC71_FAN_CTRL_SILENT_MODE | QC71_FAN_CTRL_TURBO);
	
	uint8 value = data & mask;
	uint8 target = 0;
	
	switch (value) {
		case QC71_FAN_CTRL_SILENT_MODE:
			target = 0;
			TRACE_ALWAYS("Switching to balanced mode\n");
		break;
		
		case 0:
			target = QC71_FAN_CTRL_TURBO;
			TRACE_ALWAYS("Switching to turbo mode\n");
		break;
		
		case QC71_FAN_CTRL_TURBO:
			target = QC71_FAN_CTRL_SILENT_MODE;
			TRACE_ALWAYS("Switching to powersave mode\n");
		break;
		
		default:
			target = QC71_FAN_CTRL_SILENT_MODE;
			TRACE_ALWAYS("Unknown state, switching to powersave mode\n");
	}
	
	value = data & ~mask;
	value = value | target;
	
	TRACE_ALWAYS("current 0x%" B_PRIx32 "\n", data);
	TRACE_ALWAYS("next 0x%" B_PRIx32 "\n", value);
	status = WriteEC(QC71_FAN_CTRL_ADDR, value);
	
	if (status != B_OK)
		return;
}

//	#pragma mark - driver module API


static float
wmi_qc71_support(device_node *parent)
{
	// make sure parent is really a wmi device
	const char *bus;
	if (gDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false))
		return -1;

	if (strcmp(bus, "wmi"))
		return 0.0;

	// check whether it's an qc71 wmi device
	const char *guid;
	if (gDeviceManager->get_attr_string(parent, WMI_GUID_STRING_ITEM, &guid,
		false) != B_OK || strcmp(guid, ACPI_QC71_WMI_MGMT_GUID) != 0) {
		return 0.0;
	}

	TRACE_ALWAYS("found an qc71 wmi device\n");

	return 0.6;
}


static status_t
wmi_qc71_register_device(device_node *node)
{
	CALLED();
	device_attr attrs[] = {
		{ B_DEVICE_PRETTY_NAME, B_STRING_TYPE, { .string = "WMI QC71" }},
		{ NULL }
	};

	return gDeviceManager->register_node(node, WMI_QC71_DRIVER_NAME, attrs,
		NULL, NULL);
}


static status_t
wmi_qc71_init_driver(device_node *node, void **driverCookie)
{
	CALLED();

	WMIQC71* device = new(std::nothrow) WMIQC71(node);
	if (device == NULL)
		return B_NO_MEMORY;
	*driverCookie = device;

	return B_OK;
}


static void
wmi_qc71_uninit_driver(void *driverCookie)
{
	CALLED();
	WMIQC71* device = (WMIQC71*)driverCookie;
	delete device;
}


static status_t
wmi_qc71_register_child_devices(void *cookie)
{
	CALLED();
	return B_OK;
}


driver_module_info gWMIQC71DriverModule = {
	{
		WMI_QC71_DRIVER_NAME,
		0,
		NULL
	},

	wmi_qc71_support,
	wmi_qc71_register_device,
	wmi_qc71_init_driver,
	wmi_qc71_uninit_driver,
	wmi_qc71_register_child_devices,
	NULL,	// rescan
	NULL,	// removed
};

