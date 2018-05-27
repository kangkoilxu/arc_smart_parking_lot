/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2016.01
 * \date 2015-07-10
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	BOARD_NSIM_COMMON_TIMER	NSIM Common Timer Module
 * \ingroup	BOARD_NSIM_COMMON
 * \brief	provide basic board timer-related functions
 * \details
 * 		Provide a 1 MS (default) timer interrupt,
 *	provide a 64-bit counter value (no clear) count in the timer interrupt,
 *	provide MS-precision delay, with OS enabled-support delay
 */

/**
 * \file
 * \ingroup	BOARD_NSIM_COMMON_TIMER
 * \brief	provide nsim board timer-related functions
 */

/**
 * \addtogroup	BOARD_NSIM_COMMON_TIMER
 * @{
 */
#include "arc_builtin.h"
#include "arc.h"
#include "arc_timer.h"
#include "arc_exception.h"

#include "board.h"

#define MAX_SYS_COUNTER_VALUE		(0xffffffff)

#ifndef BOARD_SYS_TIMER_HZ
#define BOARD_SYS_TIMER_HZ		(1000)
#endif

/** nsim board timer interrupt reset count */
static uint32_t cyc_hz_count = (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);

/** nsim board timer counter in timer interrupt */
volatile uint64_t gl_nsim_sys_hz_cnt = 0;
/** nsim board 1ms counter */
volatile uint32_t gl_nsim_ms_cnt = 0;

#define HZ_COUNT_CONV(precision, base)	((precision)/(base))

/**
 * \brief	Update timer counter and other MS period operation
 * 	in cycling interrupt and must be called periodically. When the OS timer
 *	interrupt is in conflict with the bare-metal timer interrupt, 
 *	put this function into the OS timer interrupt
 * \param[in]	precision	interrupt-period precision in Hz
 */
void board_timer_update(uint32_t precision)
{
	static uint32_t sys_hz_update = 0;
	static uint32_t sys_ms_update = 0;
	uint32_t hz_conv = 0;

	/** count sys hz */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_HZ);
	sys_hz_update ++;
	if (sys_hz_update >= hz_conv) {
		sys_hz_update = 0;
		gl_nsim_sys_hz_cnt ++;
	}

	/** count ms */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_MS_HZ);
	sys_ms_update ++;
	if (sys_ms_update >= hz_conv) {
		sys_ms_update = 0;
		gl_nsim_ms_cnt ++;

	}
}

/**
 * \brief	nsim bare-metal timer interrupt.
 * 	the Interrupt frequency is based on the defined \ref BOARD_SYS_TIMER_HZ
 */
static void nsim_timer_isr(void *ptr)
{
	timer_int_clear(BOARD_SYS_TIMER_ID);

	board_timer_update(BOARD_SYS_TIMER_HZ);
}

/**
 * \brief	init bare-metal nsim board timer and interrupt
 * \details
 * 		This function is called in \ref board_init, and
 * 	it initializes the 1-MS timer interrupt for bare-metal mode
 */
void nsim_timer_init(void)
{
	if (timer_present(BOARD_SYS_TIMER_ID)) {
		int_disable(BOARD_SYS_TIMER_INTNO); /* disable first then enable */
		int_handler_install(BOARD_SYS_TIMER_INTNO, nsim_timer_isr);
		timer_start(BOARD_SYS_TIMER_ID, TIMER_CTRL_IE, cyc_hz_count);  /* start 1ms timer interrupt */

		int_enable(BOARD_SYS_TIMER_INTNO);
	}
}

/**
 * \brief	provide MS delay function
 * \details
 * 		this function needs a 1-MS timer interrupt to work.
 * 	For bare-metal, it is implemented in this file.
 * 	For OS, you must call \ref board_timer_update in
 * 	the OS 1-MS timer interrupt when the bare-metal timer interrupt
 * 	is not ready
 * \param[in]	ms		MS to delay
 * \param[in]	os_compat	Enable or disable
 *	When this delay is enabled, use the OS delay function, if one is provided.
 *	See \ref OSP_DELAY_OS_COMPAT_ENABLE and
 *	\ref OSP_DELAY_OS_COMPAT_DISABLE
 */
void board_delay_ms(uint32_t ms, uint8_t os_compat)
{
	uint32_t start_ms;

#ifdef ENABLE_OS
	if (os_compat == OSP_DELAY_OS_COMPAT_ENABLE) {
		/** \todo add different os delay functions */
		//return;
	}
#endif
	start_ms = OSP_GET_CUR_MS();
	while ((OSP_GET_CUR_MS() - start_ms) < ms);
}

/** @} end of group BOARD_NSIM_COMMON_TIMER */
