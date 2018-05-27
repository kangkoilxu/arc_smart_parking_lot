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
 * \date 2014-07-15
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * \brief C Implementation of exception and interrupt management
 */

#include "arc_exception.h"
#include "arc_cache.h"

#define	 DBG_LESS
#include "embARC_debug.h"

/**
 * \addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 * \var exc_entry_table
 * \brief exception entry table
 *
 * install exception entry table to ARC_AUX_INT_VECT_BASE in startup.
 * According to ARCv2 ISA, vectors are fetched in instruction space and thus
 * may be present in ICCM, Instruction Cache, or
 * main memory accessed by instruction fetch logic.
 * So it is put into a specific section .vector.
 *
 * Please note that the exc_entry_table maybe cached in ARC. Some functions is
 * defined in .s files.
 *
 */
__attribute__ ((aligned(1024), section(".vector")))
EXC_ENTRY exc_entry_table[NUM_EXC_ALL];
/**
 * \var exc_int_handler_table
 * \brief the cpu exception and interrupt exception handler table
 * called in exc_entry_default and exc_entry_int
 */
EXC_HANDLER exc_int_handler_table[NUM_EXC_ALL];

/**
 * \var exc_nest_count
 * \brief the counter for exc/int processing, =0 no int/exc
 * >1 in int/exc processing
 * @}
 */
unsigned int exc_nest_count;

/**
 * \brief	convert exception vector number to name
 * \param	vector	vector number
 * \return	vector name description
 */
static char * conv_exc2name(int vector)
{
	switch (vector) {
		case EXC_NO_MEM_ERR:
			return "memory error";
		case EXC_NO_INS_ERR:
			return "instruction error";
		case EXC_NO_MAC_CHK:
			return "machine check";
		case EXC_NO_TLB_MISS_I:
			return "tlbmissi";
		case EXC_NO_TLB_MISS_D:
			return "tlbmissd";
		case EXC_NO_PRO_VIO:
			return "protv";
		case EXC_NO_PRI_VIO:
			return "privilegev";
		case EXC_NO_SWI:
			return "swi";
		case EXC_NO_TRAP:
			return "trap";
		case EXC_NO_EXT:
			return "extension";
		case EXC_NO_DIV_ZER0:
			return "divzero";
		case EXC_NO_DC_ERR:
			return "dcerr";
		case EXC_NO_MAL_ALIGN:
			return "maligned";
		case EXC_NO_RESERVE0:
		case EXC_NO_RESERVE1:
		default:
			return "*unknown*";
	}
}


/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  default cpu exception handler
 * \param p_excinf pointer to the exception frame
 */
static void exc_handler_default(void *p_excinf)
{
	unsigned int excpt_cause_reg = 0;
	unsigned int excpt_ret_reg = 0;
	int vector_no = 0;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	vector_no = (excpt_cause_reg >> 16) & 0xff;

	dbg_printf(DBG_LESS_INFO, "default cpu exception handler\r\n");
	dbg_printf(DBG_LESS_INFO, "excpt:%s, last sp:0x%08x, ecr:0x%08x, eret:0x%08x\r\n",
		conv_exc2name(vector_no), (unsigned int)p_excinf, excpt_cause_reg, excpt_ret_reg);

	Asm("kflag 1");
}


/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  default interrupt handler
 * \param[in] p_excinf	information for interrupt handler
 */
static void int_handler_default(void *p_excinf)
{
	unsigned int int_cause_reg = 0;
	unsigned int excpt_ret_reg = 0;

	int_cause_reg = _arc_aux_read(AUX_IRQ_CAUSE);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	dbg_printf(DBG_LESS_INFO, "default interrupt handler\n\r");
	dbg_printf(DBG_LESS_INFO, "last sp:0x%08x, icause:0x%08x\n\r", (unsigned int)p_excinf, int_cause_reg);

	Asm("kflag 1");
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  install a CPU exception entry
 * \param[in] excno exception number
 * \param[in] entry the entry of exception to install
 */
int exc_entry_install(const unsigned int excno, EXC_ENTRY entry)
{
	unsigned int status;

	if (excno < NUM_EXC_ALL && entry != NULL
		&& exc_entry_table[excno] != entry) {
		status = cpu_lock_save();
		/* directly write to mem, as arc gets exception handler from mem not from cache */
		/* FIXME, here maybe icache is dirty, need to be invalidated */
		exc_entry_table[excno] = entry;

		if (_arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
		/* dcache is available */
			dcache_flush_line((uint32_t)&exc_entry_table[excno]);
		}

		if (_arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
		/* icache is available */
			icache_invalidate_line((uint32_t)&exc_entry_table[excno]);
		}
		cpu_unlock_restore(status);
		return 0;
	}
	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  get the installed CPU exception entry
 * \param[in] excno exception number
 * \return the installed CPU exception entry
 */
EXC_ENTRY exc_entry_get(const unsigned int excno)
{
	if (excno < NUM_EXC_ALL) {
		return exc_entry_table[excno];
	}
	return NULL;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  install an exception handler
 * \param[in] excno	exception number
 * \param[in] handler the handler of exception to install
 */
int exc_handler_install(const unsigned int excno, EXC_HANDLER handler)
{
	if (excno < NUM_EXC_CPU && handler != NULL) {
		exc_int_handler_table[excno] = handler;
		return 0;
	}

	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  get the installed exception handler
 * \param[in] excno	exception number
 * \return the installed exception handler or NULL
 */
EXC_HANDLER exc_handler_get(const unsigned int excno)
{
	if (excno < NUM_EXC_CPU) {
		return exc_int_handler_table[excno];
	}

	return NULL;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  intialize the exception and interrupt handling
 */
void exc_int_init(void)
{
	int i;
	unsigned int status;

	status = cpu_lock_save();
	exc_entry_table[0] = _arc_reset;
	/* init cpu exceptions */
	for (i = 1; i < NUM_EXC_CPU; i++)
	{
		exc_entry_table[i] = exc_entry_cpu;
		exc_int_handler_table[i] = (EXC_HANDLER)exc_handler_default;
	}

	for (;i < NUM_EXC_ALL; i++)
	{

		/* interrupt level triggered, disabled, priority is the lowest */
		_arc_aux_write(AUX_IRQ_SELECT, i);
		_arc_aux_write(AUX_IRQ_ENABLE, 0);
		_arc_aux_write(AUX_IRQ_TRIGGER, 0);
		_arc_aux_write(AUX_IRQ_PRIORITY, INT_PRI_MAX - INT_PRI_MIN);
#if INT_PRI_MAX == INT_PRI_MIN && ARC_FEATURE_RGF_NUM_BANKS > 1 && ARC_FEATURE_FIRQ == 1
		exc_entry_table[i] = exc_entry_firq;
#else
		exc_entry_table[i] = exc_entry_int;
#endif
		exc_int_handler_table[i] = (EXC_HANDLER)int_handler_default;
	}
	_arc_aux_write(AUX_IRQ_CTRL, 0);

	/* exc_entry_table and exc_int_handler_table cannot be in dcache */
	if (_arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
		/* dcache is available */
		dcache_flush();
	}

	if (_arc_aux_read(AUX_BCR_I_CACHE) > 0x2) {
	/* icache is available */
		icache_invalidate();
	}
	cpu_unlock_restore(status);

}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  install an interrupt handler
 * \param[in] intno	interrupt number
 * \param[in] handler interrupt handler to install
 */
int int_handler_install(const unsigned int intno, INT_HANDLER handler)
{
	/*!< \todo parameter check ? */
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL && handler != NULL) {
		exc_int_handler_table[intno] = handler;
		return 0;
	}

	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  get the installed an interrupt handler
 * \param[in] intno interrupt number
 * \return the installed interrupt handler or NULL
 */
INT_HANDLER int_handler_get(const unsigned int intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return exc_int_handler_table[intno];
	}

	return NULL;
}

/**
 * \brief set interrupt priority
 *
 * \param[in] intno interrupt number
 * \param[in] intpri interrupt priority
 */
void int_pri_set(const unsigned int intno, int intpri)
{
	unsigned int status;

	status = cpu_lock_save();
	intpri = intpri - INT_PRI_MIN;
#if ARC_FEATURE_RGF_NUM_BANKS > 1 && ARC_FEATURE_FIRQ == 1
	if (intpri == 0) { /* intno is firq */
		exc_entry_install(intno, exc_entry_firq);
	} else {
		exc_entry_install(intno, exc_entry_int);
	}
#endif
	_arc_aux_write(AUX_IRQ_SELECT, intno);
	_arc_aux_write(AUX_IRQ_PRIORITY, (unsigned int)intpri);

	cpu_unlock_restore(status);
}