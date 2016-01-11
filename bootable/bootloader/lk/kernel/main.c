/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <compiler.h>
#include <debug.h>
#include <string.h>
#include <app.h>
#include <arch.h>
#include <platform.h>
#include <target.h>
#include <lib/heap.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <kernel/dpc.h>
#include <platform/mt_gpt.h>

#if _MAKE_HTC_LK
#include <htc_bootloader_log.h>
#endif
#ifdef MTK_LK_IRRX_SUPPORT
#include <platform/mtk_ir_lk_core.h>
#endif



extern void *__ctor_list;
extern void *__ctor_end;
extern int __bss_start;
extern int _end;

/* boot_time is calculated form kmain to kernel jump */
volatile unsigned int boot_time = 0;


static int bootstrap2(void *arg);

#if 0 // (ENABLE_NANDWRITE)
void bootstrap_nandwrite(void);
#endif

static void call_constructors(void)
{
	void **ctor;
   
	ctor = &__ctor_list;
	while(ctor != &__ctor_end) {
		void (*func)(void);

		func = (void (*)())*ctor;

		func();
		ctor++;
	}
}

/* called from crt0.S */
void kmain(void) __NO_RETURN __EXTERNALLY_VISIBLE;
void kmain(void)
{
#if !defined(MACH_FPGA) && !defined(SB_LK_BRINGUP)
	boot_time = get_timer(0);
#endif

	// get us into some sort of thread context
	thread_init_early();

	// early arch stuff
	arch_early_init();

#if _MAKE_HTC_LK
	// do bootloader log init
	bldr_log_init(BOOT_DEBUG_LOG_BASE_PA, BOOT_DEBUG_LOG_SIZE);
#endif
	// do any super early platform initialization
	platform_early_init();
	printf(CRITICAL, "zzytest, after platform_early_init\n");
#if defined(MACH_FPGA) || defined(SB_LK_BRINGUP)
	boot_time = get_timer(0);
#endif

	// do any super early target initialization
	target_early_init();

	dprintf(CRITICAL, "zzytest, welcome to lk\n");
	dprintf(INFO, "welcome to lk\n\n");
	
	// deal with any static constructors
	dprintf(SPEW, "calling constructors\n");
	call_constructors();

	// bring up the kernel heap
	dprintf(SPEW, "initializing heap\n");
	heap_init();

	// initialize the threading system
	dprintf(SPEW, "initializing threads\n");
	thread_init();

	// initialize the dpc system
	dprintf(SPEW, "initializing dpc\n");
	dpc_init();

	// initialize kernel timers
	dprintf(SPEW, "initializing timers\n");
	timer_init();

#ifdef  MTK_LK_IRRX_SUPPORT
   mtk_ir_init(0);
#endif


#if 1 // (!ENABLE_NANDWRITE)
	// create a thread to complete system initialization
	dprintf(SPEW, "creating bootstrap completion thread\n");
	thread_resume(thread_create("bootstrap2", &bootstrap2, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));

	// enable interrupts
	exit_critical_section();

	// become the idle thread
	dprintf(CRITICAL, "zzytest, thread_become_idle\n");
	thread_become_idle();
#else
	dprintf(CRITICAL, "zzytest, bootstrap_nandwrite\n");
        bootstrap_nandwrite();
#endif
}

int main(void);

static int bootstrap2(void *arg)
{
	dprintf(CRITICAL, "zzytest, bootstrap2\n");
	dprintf(SPEW, "top of bootstrap2()\n");

	arch_init();

	// XXX put this somewhere else
#if 0 // WITH_LIB_BIO
	bio_init();
#endif
#if 0 // WITH_LIB_FS
	fs_init();
#endif

	// initialize the rest of the platform
	dprintf(SPEW, "initializing platform\n");
	platform_init();
	
	// initialize the target
	dprintf(SPEW, "initializing target\n");
	target_init();

	dprintf(SPEW, "calling apps_init()\n");
	apps_init();

	return 0;
}

#if 0 // (ENABLE_NANDWRITE)
void bootstrap_nandwrite(void)
{
	dprintf(SPEW, "top of bootstrap2()\n");

	arch_init();

	// initialize the rest of the platform
	dprintf(SPEW, "initializing platform\n");
	platform_init();

	// initialize the target
	dprintf(SPEW, "initializing target\n");
	target_init();

	dprintf(SPEW, "calling nandwrite_init()\n");
	nandwrite_init();

	return 0;
}
#endif
