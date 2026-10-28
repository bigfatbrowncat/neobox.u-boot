/*
 * (C) Copyright 2011
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 *
 * (C) Copyright 2015
 * Kamil Lulko, <kamil.lulko@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _MACH_STM32_H_
#define _MACH_STM32_H_

#include <asm/arch-stm32/stm32f.h>

/*
 * Peripheral memory map
 */
#define STM32_SYSMEM_BASE	0x1FFF0000

#define STM32_GPIOA_BASE	(STM32_AHB1PERIPH_BASE + 0x0000)
#define STM32_GPIOB_BASE	(STM32_AHB1PERIPH_BASE + 0x0400)
#define STM32_GPIOC_BASE	(STM32_AHB1PERIPH_BASE + 0x0800)
#define STM32_GPIOD_BASE	(STM32_AHB1PERIPH_BASE + 0x0C00)
#define STM32_GPIOE_BASE	(STM32_AHB1PERIPH_BASE + 0x1000)
#define STM32_GPIOF_BASE	(STM32_AHB1PERIPH_BASE + 0x1400)
#define STM32_GPIOG_BASE	(STM32_AHB1PERIPH_BASE + 0x1800)
#define STM32_GPIOH_BASE	(STM32_AHB1PERIPH_BASE + 0x1C00)
#define STM32_GPIOI_BASE	(STM32_AHB1PERIPH_BASE + 0x2000)

/*
 * Register maps
 */
struct stm32_u_id_regs {
	u32 u_id_low;
	u32 u_id_mid;
	u32 u_id_high;
};

struct stm32_pwr_regs {
	u32 cr;
	u32 csr;
};

/*
 * Registers access macros
 */
#define STM32_U_ID_BASE		(STM32_SYSMEM_BASE + 0x7A10)
#define STM32_U_ID		((struct stm32_u_id_regs *)STM32_U_ID_BASE)
static const u32 sect_sz_kb[CONFIG_SYS_MAX_FLASH_SECT] = {
	[0 ... 3] =	16 * 1024,
	[4] =		64 * 1024,
	[5 ... 11] =	128 * 1024
};

#endif /* _MACH_STM31_H_ */
