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
 * \date 2014-07-17
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */
/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_SPIFLASH
 * \brief	header file of emsk onbard W25Q128BV spiflash driver
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_SPIFLASH
 * @{
 */
#ifndef _EMSK_SPIFLASH_H_
#define _EMSK_SPIFLASH_H_

#define FLASH_PAGE_SIZE		0x100
#define FLASH_SECTOR_SIZE	0x1000

#include "embARC_toolchain.h"
#include "embARC_error.h"

typedef struct {
	unsigned int head;		/*!< 0x68656164 ='head' */
	unsigned int cpu_type;		/*!< = 0 - all images, reserved for future */
	unsigned int start;		/*!< start address of application image in spi flash */
	unsigned int size;		/*!< size of image in bytes */
	unsigned int ramaddr;		/*!< address of ram for loading image */
	unsigned int ramstart;		/*!< start address of application in RAM !!!! */
	unsigned int checksum;		/*!< summ of all bytes in image */
} image_t;

extern uint32_t flash_read_id(void);
extern uint32_t flash_read_status(void);
extern int32_t flash_read(unsigned int address, unsigned int size, void *data);
extern int32_t flash_write_enable(void);
extern int32_t flash_wait_ready(void);
extern int32_t flash_erase(uint32_t address, uint32_t size);
extern int32_t flash_write(uint32_t address, uint32_t size, const void *data);
extern int32_t flash_init(void);


#endif /* _EMSK_SPIFLASH_H_ */

/** @} end of group BOARD_EMSK_DRV_SPIFLASH */
