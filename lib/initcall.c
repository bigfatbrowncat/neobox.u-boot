/*
 * Copyright (c) 2013 The Chromium OS Authors.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <initcall.h>
#include <efi.h>

DECLARE_GLOBAL_DATA_PTR;

#define TICKS_TO_US(ticks)	((ticks) / (COUNTER_FREQUENCY / 1000000))
#define US_TO_MS(ticks)		((ticks) / 1000)
#define US_TO_US(ticks)		((ticks) % 1000)

#ifdef DEBUG
static inline void call_get_ticks(ulong *ticks) { *ticks = get_ticks(); }
#else
static inline void call_get_ticks(ulong *ticks) { }
#endif

#if defined(__clang__) && defined(CONFIG_ROCKCHIP_RK3568)
/*
 * RK3568/RK3566 workaround: clang's register allocator selects x8 for
 * the indirect branch (blr x8) which hangs the Cortex-A55 cores.
 * Explicit asm using x1 for the function pointer.
 */
asm (
".globl initcall_run_list\n"
".type initcall_run_list,%function\n"
".p2align 2\n"
"initcall_run_list:\n"
"\tstp\tx29, x30, [sp, #-0x30]!\n"
"\tmov\tx29, sp\n"
"\tstp\tx19, x20, [sp, #0x10]\n"
"\tmov\tx20, x0\n"
"\tldr\tx0, [x18, #0x1a8]\n"
"\tstr\tx21, [sp, #0x20]\n"
"\tcbnz\tx0, .Lic_aftertick\n"
"\tmov\tx19, x18\n"
"\tbl\tget_ticks\n"
"\tstr\tx0, [x19, #0x1a8]\n"
".Lic_aftertick:\n"
"\tmov\tx19, x20\n"
".Lic_loadfunc:\n"
"\tldr\tx1, [x19]\n"
"\tcbnz\tx1, .Lic_reloc\n"
"\tmov\tw0, #0\n"
"\tb\t.Lic_epi\n"
".Lic_reloc:\n"
"\tldr\tx2, [x18, #8]\n"
"\tand\tx21, x2, #1\n"
"\ttbz\tw2, #0, .Lic_loadflags\n"
"\tldr\tx21, [x18, #0x88]\n"
".Lic_loadflags:\n"
"\tldr\txzr, [x18, #8]\n"
"\tblr\tx1\n"
"\tmov\tw3, w0\n"
"\tcbz\tw0, .Lic_advance\n"
".Lic_err:\n"
"\tldr\tx2, [x19]\n"
"\tmov\tx1, x20\n"
"\tadr\tx0, .Lic_str\n"
"\tsub\tx2, x2, x21\n"
"\tbl\tprintf\n"
"\tmov\tw0, #-1\n"
".Lic_epi:\n"
"\tldr\tx21, [sp, #0x20]\n"
"\tldp\tx19, x20, [sp, #0x10]\n"
"\tldp\tx29, x30, [sp], #0x30\n"
"\tret\n"
".Lic_advance:\n"
"\tadd\tx19, x19, #8\n"
"\tb\t.Lic_loadfunc\n"
".size initcall_run_list,.-initcall_run_list\n"
".section .rodata\n"
".Lic_str:\n"
".asciz \"initcall sequence %p failed at call %p (err=%d)\\n\"\n"
);
#else
int initcall_run_list(const init_fnc_t init_sequence[])
{
	const init_fnc_t *init_fnc_ptr;
	ulong start = 0, end = 0, sum = 0;

	if (!gd->sys_start_tick)
		gd->sys_start_tick = get_ticks();

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		unsigned long reloc_ofs = 0;
		int ret;

		if (gd->flags & GD_FLG_RELOC)
			reloc_ofs = gd->reloc_off;
#ifdef CONFIG_EFI_APP
		reloc_ofs = (unsigned long)image_base;
#endif
		debug("initcall: %p", (char *)*init_fnc_ptr - reloc_ofs);
		if (gd->flags & GD_FLG_RELOC)
			debug(" (relocated to %p)\n", (char *)*init_fnc_ptr);
		else
			debug("\n");
		call_get_ticks(&start);
		ret = (*init_fnc_ptr)();
		call_get_ticks(&end);

		if (start != end) {
			sum = TICKS_TO_US(end - gd->sys_start_tick);
			debug("\t\t\t\t\t\t\t\t#%8ld us #%4ld.%3ld ms\n",
			      TICKS_TO_US(end - start), US_TO_MS(sum), US_TO_US(sum));
		}
		if (ret) {
			printf("initcall sequence %p failed at call %p (err=%d)\n",
			       init_sequence,
			       (char *)*init_fnc_ptr - reloc_ofs, ret);
			return -1;
		}
	}
	return 0;
}
#endif
