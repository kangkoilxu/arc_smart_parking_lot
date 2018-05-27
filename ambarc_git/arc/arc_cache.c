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
 * \ingroup ARC_HAL_MISC_CACHE
 * \brief implementation of cache related functions
 */

#include "arc_cache.h"

#define DIV_ROUND_UP(x, y)  (((x) + (y) - 1) / (y))


struct cache_config {
	uint8_t ver;			/* version */
	uint8_t assoc;			/* Cache Associativity */
	uint16_t line; 			/* cache line/block size */
	uint32_t capacity;		/* capacity */
};


static struct cache_config icache_config, dcache_config;

/**
 * \brief invalidate multi instruction cache lines
 *
 * \param[in] start_addr start address in instruction cache
 * \param[in] size  the bytes to be invalidated
 * \return 0, succeeded, -1, failed
 */
int icache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t num_lines;

	if (size == 0 || size > icache_config.capacity) {
		return -1;
	}


	num_lines = DIV_ROUND_UP(size, icache_config.line);

	while (num_lines--) {
		_arc_aux_write(AUX_IC_IVIL, start_addr);
		Asm("nop_s");
		Asm("nop_s");
		Asm("nop_s");
		start_addr += icache_config.line;
	}
	return 0;
}

/**
 * \brief lock multi lines in instruction cache
 *
 * \param[in] start_addr start address in instruction cache
 * \param[in] size  the bytes to be locked
 * \return 0, succeeded, -1, failed (cache already locked or other reasones)
 */
int icache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t num_lines;

	if (size == 0 || size > icache_config.capacity) {
		return -1;
	}

	num_lines = DIV_ROUND_UP(size, icache_config.line);

	while (num_lines--) {
		_arc_aux_write(AUX_IC_LIL, start_addr);
		if(_arc_aux_read(AUX_IC_CTRL) & IC_CTRL_OP_SUCCEEDED) {
			start_addr += icache_config.line;
		} else {
			return -1;	/* the operation failed */
		}
	}
	return 0;
}

/**
 * \brief directly write icache internal ram
 *
 * \param[in] cache_addr, icache internal address(way+index+offset)
 * \param[in] tag	cache tag to write (tag+lock bit+valid bit)
 * \param[in] data	cache data to write
 * \return 0, succeeded, -1, failed
 */
int icache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data)
{
	if (_arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS)
		return -1;
	_arc_aux_write(AUX_IC_RAM_ADDR, cache_addr);
	_arc_aux_write(AUX_IC_TAG, tag );
	_arc_aux_write(AUX_IC_DATA, data);

	return 0;
}

/**
 * \brief directly read icache internal ram
 *
 * \param[in] cache_addr, icache internal address(way+index+offset)
 * \param[out] tag	cache tag to read (tag+index+lock bit+valid bit)
 * \param[out] data	cache data to read
 * \return 0, succeeded, -1, failed
 */
int icache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data)
{
	if (_arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS)
		return -1;
	_arc_aux_write(AUX_IC_RAM_ADDR, cache_addr);
	*tag = _arc_aux_read(AUX_IC_TAG);
	*data = _arc_aux_read(AUX_IC_DATA);

	return 0;
}

/**
 * \brief indirectly read icache internal ram
 *
 * \param[in] mem_addr, memory address
 * \param[out] tag	cache tag to read
 * \param[out] data	cache data to read
 * \return 0, succeeded, -1, failed
 */
int icache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data)
{

	if (!(_arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS))
		return -1;
	_arc_aux_write(AUX_IC_RAM_ADDR, mem_addr);
	if(_arc_aux_read(AUX_IC_CTRL) & IC_CTRL_OP_SUCCEEDED) {
		*tag = _arc_aux_read(AUX_IC_TAG);
		*data = _arc_aux_read(AUX_IC_DATA);
	} else {
		return -1;	/* the specified memory is not in icache */
	}
	return 0;
 }

/**
 * \brief invalidate multi data cache lines
 *
 * \param[in] start_addr start address in data cache
 * \param[in] size  the bytes to be invalidated
 * \return 0, succeeded, -1, failed
 */
int dcache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t num_lines;

	if (size == 0 || size > dcache_config.capacity) {
		return -1;
	}

	num_lines = DIV_ROUND_UP(size, dcache_config.line);

	while (num_lines--) {
		_arc_aux_write(AUX_DC_IVDL, start_addr);
			/* wait for flush completion */
		start_addr += dcache_config.line;
	}

	return 0;

}

/**
 * \brief flush multi lines in data cache
 *
 * \param[in] start_addr start address
 * \param[in] size	the bytes to be flushed
 * \return 0, succeeded, -1, failed
 */
int dcache_flush_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t num_lines;

	if (size == 0 || size > dcache_config.capacity) {
		return -1;
	}

	num_lines = DIV_ROUND_UP(size, dcache_config.line);

	while (num_lines--) {
		_arc_aux_write(AUX_DC_FLDL, start_addr);
		Asm("nop_s");
		Asm("nop_s");
		Asm("nop_s");
		/* wait for flush completion */
		start_addr += dcache_config.line;
	}

	while (_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS);
	return 0;
}

/**
 * \brief lock multi lines in data cache
 *
 * \param[in] start_addr start address in data cache
 * \param[in] size  the bytes to be locked
 * \return 0, succeeded, -1, failed
 */
int dcache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t num_lines;

	if (size == 0 || size > dcache_config.capacity) {
		return -1;
	}

	num_lines = DIV_ROUND_UP(size, dcache_config.line);

	while (num_lines--) {
		_arc_aux_write(AUX_DC_LDL, start_addr);
		if(_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_OP_SUCCEEDED) {
			start_addr += dcache_config.line;
		} else {
			return -1;	/* the operation failed */
		}
	}
	return 0;
}

/**
 * \brief directly write dcache internal ram
 *
 * \param[in] cache_addr, dcache internal address(way+index+offset)
 * \param[in] tag	cache tag to write
 * \param[in] data	cache data to write
 * \return 0, succeeded, -1, failed
 */
int dcache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data)
{
	if (_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS)
		return -1;
	_arc_aux_write(AUX_DC_RAM_ADDR, cache_addr);
	_arc_aux_write(AUX_DC_TAG, tag);
	_arc_aux_write(AUX_DC_DATA, data);

	return 0;
}

/**
 * \brief directly read dcache internal ram
 *
 * \param[in] cache_addr, dcache internal address(way+index+offset)
 * \param[out] tag	cache tag to read
 * \param[out] data	cache data to read
 * \return 0, succeeded, -1, failed
 */
int dcache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data)
{
	if (_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS)
		return -1;
	_arc_aux_write(AUX_DC_RAM_ADDR, cache_addr);
	*tag = _arc_aux_read(AUX_DC_TAG);
	*data = _arc_aux_read(AUX_DC_DATA);

	return 0;
}

/**
 * \brief indirectly read dcache internal ram
 *
 * \param[in] mem_addr, memory address(tag+index+offset)
 * \param[out] tag	cache tag to read
 * \param[out] data	cache data to read
 * \return 0, succeeded, -1, failed
 */
int dcache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data)
{
	if (!(_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS))
		return -1;
	_arc_aux_write(AUX_DC_RAM_ADDR, mem_addr);
	if(_arc_aux_read(AUX_DC_CTRL) & DC_CTRL_OP_SUCCEEDED) {
		*tag = _arc_aux_read(AUX_DC_TAG);
		*data = _arc_aux_read(AUX_DC_DATA);
	} else {
		return -1;	/* the specified memory is not in dcache */
	}
	return 0;
 }

/**
 * \brief  initialize cache
 * 1. invalidate icache and dcache
 * 2. Only support ARCv2 cache
 */
void arc_cache_init(void)
{
	uint32_t build_cfg;

	build_cfg = _arc_aux_read(AUX_BCR_I_CACHE);

	icache_config.ver = build_cfg & 0xff;

	if (icache_config.ver == 0x04) { /* ARCv2 */
		icache_config.assoc = 1 << ((build_cfg >> 8) & 0xf);
		icache_config.capacity = 512 << ((build_cfg >> 12) & 0xf);
		icache_config.line = 8 << ((build_cfg >> 16) & 0xf);

		icache_enable(IC_CTRL_IC_ENABLE);
		icache_invalidate();
	}

	build_cfg = _arc_aux_read(AUX_BCR_D_CACHE);

	dcache_config.ver = build_cfg & 0xff;

	if (dcache_config.ver == 0x04) { /* ARCv2 */
		dcache_config.assoc = 1 << ((build_cfg >> 8) & 0xf);
		dcache_config.capacity = 512 << ((build_cfg >> 12) & 0xf);
		dcache_config.line = 8 << ((build_cfg >> 16) & 0xf);

		dcache_enable(DC_CTRL_DISABLE_FLUSH_LOCKED |
			DC_CTRL_INDIRECT_ACCESS | DC_CTRL_INVALID_FLUSH);
		dcache_invalidate();
	}
}
