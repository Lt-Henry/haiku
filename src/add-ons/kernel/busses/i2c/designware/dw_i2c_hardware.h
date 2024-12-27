/*
 * Copyright 2024, Enrique Medina, quique@necos.es.
 *
 * Distributed under the terms of the MIT License.
 */
#ifndef _DW_I2C_HARDWARE_H
#define _DW_I2C_HARDWARE_H


#define DW_IC_CON				0x00
#define DW_IC_TAR				0x04
#define DW_IC_SAR				0x08
#define DW_IC_DATA_CMD			0x10
#define DW_IC_SS_SCL_HCNT		0x14
#define DW_IC_SS_SCL_LCNT		0x18
#define DW_IC_FS_SCL_HCNT		0x1c
#define DW_IC_FS_SCL_LCNT		0x20
#define DW_IC_HS_SCL_HCNT		0x24
#define DW_IC_HS_SCL_LCNT		0x28
#define DW_IC_INTR_STAT			0x2c

#define DW_IC_ENABLE			0x6c
#define DW_IC_STATUS			0x70

#endif // _DW_I2C_HARDWARE_H
