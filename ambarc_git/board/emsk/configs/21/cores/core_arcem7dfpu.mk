## Core Configuation Folder ##
EMSK_21_CORE_DIR = $(EMSK_21_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x41
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE)

## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core1 -Xcode_density -Xdiv_rem=radix2 -Xswap -Xnorm -Xmpy16 -Xmpy -Xmpyd \
			-Xshift_assist -Xbarrel_shifter -Xdsp_complex \
			-Xfpus_div -Xfpu_mac -Xfpuda -Xtimer0 -Xtimer1 -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=ARCv2EM -mno-sdata -mlittle-endian -mmpy -mmpy16 \
			-mdiv-rem -mbarrel-shifter -mnorm -mswap \
			-mfpu=fpus_div -mfpu=fpuda -mfpu=fpus_fma

LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mav2em -x assembler-with-cpp

