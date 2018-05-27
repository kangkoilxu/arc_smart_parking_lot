## Core Configuation Folder ##
NSIM_1506_CORE_DIR = $(NSIM_1506_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x41
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE) -DNUM_EXC_INT=8

## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core1 -Xlib -Xtimer0 -Xtimer1 -Xmpy_option=6 -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start -Hhostlink
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=ARCv2EM -mno-sdata -mlittle-endian -mmpy-option=6 \
			-mbarrel-shifter -mnorm -mswap
LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mav2em -x assembler-with-cpp

## Metaware Debugger and NSIM Related Options ##

MDB_NSIM_OPT += -noprofile -notrace -nsim -prop=ident=0x270f0141 -on=cycles \
		-arcv2em -core1 -Xtimer0 -Xtimer1 -Xlib -Xmpy_option=6 \
		-interrupts=8 -ext_interrupts=6 -interrupt_priorities=2 \
		-iccm_base=0x00000000 -iccm_size=0x00020000 \
		-dccm_base=0x80000000 -dccm_size=0x00020000 \
		-dcache=16384,32,2,a -icache=16384,32,2,a \
		-rgf_num_banks=1 -rgf_banked_regs=32 -pc_width=32

NSIM_PROPS_FILE = $(NSIM_1506_CORE_DIR)/nsim_$(CUR_CORE).props

