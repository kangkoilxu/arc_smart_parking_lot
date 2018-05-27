## Core Configuation Folder ##
EMSK_11_CORE_DIR = $(EMSK_11_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x41
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE)

## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core1 -Xswap -Xcode_density -Xshift_assist -Xbarrel_shifter \
	        -Xnorm -Xdiv_rem -Xmpy -Xmpy16 -Xmpy_cycles=5 -Xtimer0 -Xtimer1 -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=ARCv2EM -mno-sdata -mlittle-endian -mmpy-option=6 \
			-mbarrel-shifter -mdiv-rem -mnorm -mswap
LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mav2em -x assembler-with-cpp
