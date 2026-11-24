/*

	Copyright (C) 2026 Enrique Medina Gremaldos <quiqueiii@gmail.com>
	Distributed under the terms of the MIT license.
*/

#include <Drivers.h>
#include <Errors.h>
#include <KernelExport.h>

#include <arch_cpu.h>
#include <cpu.h>
#include <smp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rapl.h"


#define IA32_MSR_RAPL_POWER_UNIT		0x606
#define IA32_MSR_PKG_POWER_INFO			0x614
#define IA32_MSR_PKG_ENERGY_STATUS		0x611
#define IA32_MSR_DRAM_ENERGY_STATUS		0x619
#define IA32_MSR_PP0_ENERGY_STATUS		0x639
#define IA32_MSR_PP1_ENERGY_STATUS		0x641
#define IA32_MSR_PLATFORM_ENERGY_STATUS	0x64d

#define AMD_MSR_RAPL_POWER_UNIT			0xc0010299
#define AMD_MSR_CORE_ENERGY_STATUS		0xc001029a
#define AMD_MSR_PKG_ENERGY_STATUS		0xc001029b

#define IA32_FEATURE_RAPL				(1 << 23)

#define POWER_UNIT_SHIFT				0
#define ENERGY_UNIT_SHIFT				8
#define TIME_UNIT_SHIFT					16
#define UNIT_MASK						0xf

#define ENERGY_STATUS_MASK				0xffffffffULL
#define POWER_INFO_THERMAL_SPEC_MASK	0x7fffULL
#define POWER_INFO_MIN_MASK				0x7fffULL
#define POWER_INFO_MAX_MASK				0x7fffULL

struct DomainName {
	uint32 domain;
	const char* name;
};

static const DomainName kDomainNames[] = {
	{RAPL_DOMAIN_PKG, "pkg"},
	{RAPL_DOMAIN_PP0, "pp0"},
	{RAPL_DOMAIN_PP1, "pp1"},
	{RAPL_DOMAIN_DRAM, "dram"},
	{RAPL_DOMAIN_PSYS, "psys"},
	{0, 0}
};

struct ModelDomains {
	uint8	model;
	uint32	domains;
};

static const ModelDomains kModelDomains[] = {
	{ 0x2a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Sandy Bridge
	{ 0x2d, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Sandy Bridge EP
	{ 0x3a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Ivy Bridge
	{ 0x3c, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell
	{ 0x3d, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell EP
	{ 0x3e, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell
	{ 0x3f, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell EP
	{ 0x45, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell ULT
	{ 0x46, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell GT3
	{ 0x47, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell GT3e
	{ 0x4a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Haswell ULT
	{ 0x4d, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell EP
	{ 0x4e, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell
	{ 0x4f, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell
	{ 0x55, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake SP
	{ 0x56, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell DE
	{ 0x57, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell DE
	{ 0x5a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell ULT
	{ 0x5c, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Broadwell GT3
	{ 0x5e, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake Y/U
	{ 0x5f, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake S
	{ 0x75, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake X
	{ 0x7a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake SP
	{ 0x85, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Skylake / Kaby Lake
	{ 0x8c, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Kaby Lake U
	{ 0x8e, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Kaby Lake / Coffee Lake
	{ 0x9e, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Coffee Lake
	{ 0x66, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Ice Lake
	{ 0x6a, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Ice Lake
	{ 0x6c, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Tiger Lake
	{ 0x7d, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Rocket Lake
	{ 0x97, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Alder Lake
	{ 0x9f, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM },	// Alder Lake
	{ 0xbf, RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0 | RAPL_DOMAIN_DRAM }	// Raptor Lake
};


struct Package {
	int32	id;				/* package id */
	int32	cpu;			/* main cpu */
	uint32	domains;
	double	powerUnit;
	double	energyUnit;
	double	timeUnit;
	char* deviceName;
};


struct MsrRequest {
	uint32	msr;
	uint64	value;
};


static uint32	sVendor;
static int32		sPackageCount;
static Package* sPackages[RAPL_MAX_PACKAGES];
static char**		sDeviceNames;
static bool			sSupported;


static int64 
pow64(int64 value, int32 exp)
{
	uint64 result = 1;
	
	for (int32 n = 0;n<exp;n++) {
		result = result * value;
	}
	
	return result;
}


static const char*
domain_name(uint32 domain)
{
	const DomainName* q = &kDomainNames[0];
	
	while (q->domain!=0) {
	
		if (q->domain == domain) {
			return q->name;
		}
		
		q++;
	}
	
	return NULL;
}

static uint32
domains_for_cpu(cpu_ent* cpu)
{
	if (sVendor == VENDOR_AMD)
		return RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0;

	uint8 model = cpu->arch.model + (cpu->arch.extended_model << 4);

	for (uint32 i = 0; i < sizeof(kModelDomains) / sizeof(kModelDomains[0]); i++) {
		if (kModelDomains[i].model == model)
			return kModelDomains[i].domains;
	}

	return RAPL_DOMAIN_PKG | RAPL_DOMAIN_PP0;
}


static int32
find_package_index(int32 packageID)
{
	for (int32 i = 0; i < sPackageCount; i++) {
		if (sPackages[i]->id == packageID)
			return i;
	}
	return -1;
}


static void
read_msr_on_cpu(void* data, int /* cpu */)
{
	MsrRequest* request = (MsrRequest*)data;
	request->value = x86_read_msr(request->msr);
}


static uint64
read_msr_on(int32 cpu, uint32 msr)
{
	if (smp_get_current_cpu() == cpu)
		return x86_read_msr(msr);

	MsrRequest request;
	request.msr = msr;
	request.value = 0;
	call_single_cpu_sync(cpu, read_msr_on_cpu, &request);
	return request.value;
}


static uint64
read_package_msr(const Package* package, uint32 msr)
{
	int32 current = gCPU[smp_get_current_cpu()].topology_id[CPU_TOPOLOGY_PACKAGE];
	if (current == package->id)
		return x86_read_msr(msr);

	return read_msr_on(package->cpu, msr);
}


status_t
detect_packages()
{
	
	int32 cpuCount = smp_get_num_cpus();
	
	TRACE("CPU count:%" B_PRId32 ", mapping packages...\n", cpuCount);

	uint32 unitMSR = sVendor == VENDOR_AMD ? AMD_MSR_RAPL_POWER_UNIT
		: IA32_MSR_RAPL_POWER_UNIT;

	for (int32 i = 0; i < cpuCount; i++) {
		if (gCPU[i].disabled)
			continue;

		int32 packageID = gCPU[i].topology_id[CPU_TOPOLOGY_PACKAGE];
		int32 index = find_package_index(packageID);
		
		if (index < 0) {
			
			Package* package = new (std::nothrow) Package;
			package->id = packageID;
			package->cpu = i;
			package->domains = domains_for_cpu(&gCPU[i]);

			uint64 units = read_msr_on(i, unitMSR);
			uint64 raw;
			
			package->powerUnit = 1.0 / (1ULL << ((units >> POWER_UNIT_SHIFT) & UNIT_MASK));
			raw =  ((units >> ENERGY_UNIT_SHIFT) & 0x1F);
			package->energyUnit = 1000000.0 * (1.0/(double)pow64(2,raw));
			package->timeUnit = 1.0 / (1ULL << ((units >> TIME_UNIT_SHIFT) & UNIT_MASK));

			TRACE_ALWAYS("package %" B_PRId32 " (cpu %" B_PRId32
				"), domains 0x%08" B_PRIx32 ", energy unit %.4f uJ\n",
				package->id, package->cpu, package->domains,
				package->energyUnit);
			
			sPackages[sPackageCount] = package;
			sPackageCount++;
		}
	}

	return B_OK;
}


static status_t
publish_package_devices()
{
	/* One string per package plus the terminating NULL. */
	//sDeviceNames = (char**)malloc(sizeof(char*) * (sPackageCount + 1));
	sDeviceNames = new (std::nothrow) char* [sPackageCount + 1];
	if (sDeviceNames == NULL)
		return B_NO_MEMORY;

	for (int32 i = 0; i < sPackageCount; i++) {
		size_t size = strlen(RAPL_DEVICE_PATH) + 12;

		Package* package = sPackages[i];
		
		//sDeviceNames[i] = (char*)malloc(size);
		sDeviceNames[i] = new (std::nothrow) char [size];
		
		if (sDeviceNames[i] == NULL) {
			while (i-- > 0)
				delete [] sDeviceNames[i];
			delete [] sDeviceNames;
			sDeviceNames = NULL;
			return B_NO_MEMORY;
		}

		snprintf(sDeviceNames[i], size, RAPL_DEVICE_PATH, package->id);
		package->deviceName = sDeviceNames[i];
	}
	
	sDeviceNames[sPackageCount] = NULL;
	
	return B_OK;
}


static status_t
rapl_open(const char* name, uint32 flags, void** cookie)
{
	
	for (int32 i = 0; i < sPackageCount; i++) {
		Package* package = sPackages[i];
		
		if (strcmp(name,package->deviceName) == 0) {
			*cookie = package;
			return B_OK;
		}
	}
	
	return B_ERROR;
}


static status_t
rapl_close(void* cookie)
{
	return B_OK;
}


static status_t
rapl_free(void* cookie)
{
	return B_OK;
}


static status_t
rapl_read(void* cookie, off_t position, void* buffer, size_t* numBytes)
{
	Package* package = (Package*) cookie;
	
	if (*numBytes < 1)
		return B_IO_ERROR;
	
	if (position == 0) {
		char text[128];
		char* str = text;
		size_t maxLen = sizeof(text);

		const uint32 domains [] = {
			RAPL_DOMAIN_PKG,
			RAPL_DOMAIN_PP0,
			RAPL_DOMAIN_PP1,
			RAPL_DOMAIN_DRAM,
			RAPL_DOMAIN_PSYS,
			0
		};
		
		uint32 n = 0;
		
		while (domains[n] != 0) {
			const char* name = domain_name(domains[n]);
			double value = 0.0;
			
			uint32 match = package->domains & domains[n];
			
			if (match == RAPL_DOMAIN_PKG) {
				uint32 msr = sVendor == VENDOR_AMD ? AMD_MSR_PKG_ENERGY_STATUS
					: IA32_MSR_PKG_ENERGY_STATUS;
				uint64 raw = read_package_msr(package, msr) & ENERGY_STATUS_MASK;
				value = raw;
			}
			
			if (match == RAPL_DOMAIN_PP0) {
				if (sVendor == VENDOR_AMD) {
				
					uint64 raw = 0;
					for (int32 i = 0; i < smp_get_num_cpus(); i++) {
						if (gCPU[i].disabled) 
							continue;
							
						if (gCPU[i].topology_id[CPU_TOPOLOGY_PACKAGE] == package->id) {
							raw += read_msr_on(i,AMD_MSR_CORE_ENERGY_STATUS)  & ENERGY_STATUS_MASK;
						}
						
					}
					value = raw;
					value = value * package->energyUnit;
		
				} else {
					uint64 raw = read_package_msr(package, IA32_MSR_PP0_ENERGY_STATUS) & ENERGY_STATUS_MASK;
					value = raw;
					value = value * package->energyUnit;
				}
			}
			
			if (match == RAPL_DOMAIN_PP1) {
				uint64 raw = read_package_msr(package, IA32_MSR_PP1_ENERGY_STATUS) & ENERGY_STATUS_MASK;
				value = raw;
				value = value * package->energyUnit;
			}
			
			if (match == RAPL_DOMAIN_DRAM) {
				uint64 raw = read_package_msr(package, IA32_MSR_DRAM_ENERGY_STATUS) & ENERGY_STATUS_MASK;
				value = raw;
				value = value * package->energyUnit;
			}
			
			if (match == RAPL_DOMAIN_PSYS) {
				uint64 raw = read_package_msr(package, IA32_MSR_PLATFORM_ENERGY_STATUS) & ENERGY_STATUS_MASK;
				value = raw;
				value = value * package->energyUnit;
			}
			
			if (match > 0) {
				snprintf(str, maxLen,"%s:%.4f\n", name, value);
				maxLen -= strlen(str);
				str += strlen(str);
			}
			
			n++;
		}
		
		maxLen = user_strlcpy((char*)buffer, text, *numBytes);
		
		if (maxLen < B_OK)
			return B_BAD_ADDRESS;
		*numBytes = maxLen;
	
	}
	else {
		*numBytes = 0;
	}
	
	return B_OK;
}


static status_t
rapl_control(void* cookie, uint32 op, void* data, size_t length)
{
	return B_NOT_SUPPORTED;
}


static status_t
rapl_write(void* cookie, off_t position, const void* buffer, size_t* numBytes)
{
	*numBytes = 0;
	return B_NOT_SUPPORTED;
}


static device_hooks kDeviceHooks = {
	rapl_open,
	rapl_close,
	rapl_free,
	rapl_control,
	rapl_read,
	rapl_write
};


int32 api_version = B_CUR_DRIVER_API_VERSION;


status_t
init_hardware(void)
{
	if (!x86_check_feature(IA32_FEATURE_MSR, FEATURE_COMMON))
		return B_ERROR;

	cpu_ent* cpu = &gCPU[0];
	uint32 family = cpu->arch.family + cpu->arch.extended_family;

	if (cpu->arch.vendor == VENDOR_INTEL) {
		if (cpu->arch.family != 6)
			return B_ERROR;
		if (!x86_check_feature(IA32_FEATURE_RAPL, FEATURE_6_EAX))
			return B_ERROR;
		sVendor = VENDOR_INTEL;
	} else if (cpu->arch.vendor == VENDOR_AMD) {
		/* Family 17h (Zen/Zen 2) and 19h (Zen 3/Zen 4) share the same
		 * RAPL MSR layout. AMD has no RAPL CPUID bit to check. */
		if (family != 0x17 && family != 0x19)
			return B_ERROR;
		sVendor = VENDOR_AMD;
	} else {
		return B_ERROR;
	}

	TRACE_ALWAYS("Found a CPU with RAPL support\n");
	
	sSupported = true;
	return B_OK;
}


status_t
init_driver(void)
{
	if (!sSupported)
		return B_NOT_INITIALIZED;

	status_t status = detect_packages();
	if (status != B_OK)
		return status;

	status = publish_package_devices();

	return status;
}


void
uninit_driver(void)
{
	if (sDeviceNames != NULL) {
		for (int32 i = 0; i < sPackageCount; i++) {
			delete [] sDeviceNames[i];
			delete sPackages[i];
		}
		delete [] sDeviceNames;
		sDeviceNames = NULL;
	}

	sSupported = false;
}


const char**
publish_devices(void)
{
	return (const char**)sDeviceNames;
}


device_hooks*
find_device(const char* name)
{
	for (int32 i = 0; i < sPackageCount; i++) {
		Package* package = sPackages[i];
		
		if (strcmp(name,package->deviceName) == 0) {
			return &kDeviceHooks;
		}
	}
	return NULL;
}
