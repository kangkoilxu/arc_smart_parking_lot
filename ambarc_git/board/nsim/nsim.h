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
 *
 * \file
 * \ingroup	BOARD_NSIM_COMMON_INIT
 * \brief	nsim resource definitions
 */

/**
 * \addtogroup BOARD_NSIM_COMMON_INIT
 * @{
 */
#ifndef _NSIM_H_
#define _NSIM_H_

#include "arc_em.h"

#include "drivers/uart/nsim_uart_obj.h"
#include "drivers/ntshell/ntshell_io.h"
#include "common/nsim_timer.h"

#define CLK_CPU			(25000000U)		/*!< CPU initial clock freq, no meaning for nsim, it's not accurate. */

#define INTNO_TIMER0		16
#define INTNO_TIMER1		17

/* common macros must be defined by all boards */

#define BOARD_CONSOLE_UART_ID		NSIM_UART_0_ID
#define BOARD_ONBOARD_NTSHELL_ID	NSIM_NTSHELL_0_ID

#define BOARD_SYS_TIMER_ID		TIMER_0
#define BOARD_SYS_TIMER_INTNO		INTNO_TIMER0
#define BOARD_SYS_TIMER_HZ		(1000)

/** board timer 1ms means what HZ count */
#define BOARD_SYS_TIMER_MS_HZ		(1000)
/** board ms counter convention based on the global timer counter */
#define BOARD_SYS_TIMER_MS_CONV		(BOARD_SYS_TIMER_MS_HZ/BOARD_SYS_TIMER_HZ)

#define BOARD_OS_TIMER_ID		TIMER_0
#define BOARD_OS_TIMER_INTNO		INTNO_TIMER0

#define BOARD_CPU_CLOCK			CLK_CPU

#define OSP_DELAY_OS_COMPAT_ENABLE	(1)
#define OSP_DELAY_OS_COMPAT_DISABLE	(0)

#define OSP_GET_CUR_SYSHZ()		(gl_nsim_sys_hz_cnt)
#define OSP_GET_CUR_MS()		(gl_nsim_ms_cnt)

extern void board_init(void);
extern void board_timer_update(uint32_t precision);
extern void board_delay_ms(uint32_t ms, uint8_t os_compat);

#endif	/* _NSIM_H_ */

/** @} end of group BOARD_NSIM_COMMON_INIT */

