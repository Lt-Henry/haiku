/*

	Copyright (C) 2026 Enrique Medina Gremaldos <quiqueiii@gmail.com>
	Distributed under the terms of the MIT license.
*/
#ifndef _RAPL_DRIVER_H_
#define _RAPL_DRIVER_H_

#define RAPL_DRIVER_NAME	"rapl"
#define RAPL_DEVICE_PATH "power/rapl/%d"
#define RAPL_MAX_PACKAGES	16

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


enum {
	RAPL_DOMAIN_PKG	 = 1u << 0,	/* whole package */
	RAPL_DOMAIN_PP0	 = 1u << 1,	/* cores */
	RAPL_DOMAIN_PP1	 = 1u << 2,	/* uncore / integrated GPU */
	RAPL_DOMAIN_DRAM = 1u << 3,	/* memory controller */
	RAPL_DOMAIN_PSYS = 1u << 4	/* whole platform (Skylake+) */
};


#define	TRACE(x...)			dprintf(RAPL_DRIVER_NAME ": " x)
#define TRACE_ALWAYS(x...)	dprintf(RAPL_DRIVER_NAME ": " x)

#endif
