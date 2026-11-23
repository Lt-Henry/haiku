/*

	Copyright (C) 2026 Enrique Medina Gremaldos <quiqueiii@gmail.com>
	Distributed under the terms of the MIT license.
*/
#ifndef _RAPL_DRIVER_H_
#define _RAPL_DRIVER_H_

#define RAPL_DRIVER_NAME	"rapl"
#define RAPL_DEVICE_PATH "power/rapl/%d"

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
