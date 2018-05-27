##
# \defgroup	MK_BOARD	Board Makefile Configurations
# \brief	makefile related to board configurations
##

##
# BOARD
# select the target board
# scan the sub-dirs of board to get the supported boards
SUPPORTED_BOARDS = $(basename $(notdir $(wildcard $(EMBARC_ROOT)/board/*/*.mk)))
BOARD ?= emsk

override BOARD := $(strip $(BOARD))

BOARD_CSRCDIR	+= $(EMBARC_ROOT)/board
BOARD_ASMSRCDIR	+= $(EMBARC_ROOT)/board
BOARD_INCDIR	+= $(EMBARC_ROOT)/board

EXTRA_BOARD_DEFINES +=

## CPU_FREQ & DEV_FREQ defined in each board support file ##

## Set Valid Board
VALID_BOARD = $(call check_item_exist, $(BOARD), $(SUPPORTED_BOARDS))

## Try Check BOARD is valid
ifeq ($(VALID_BOARD), )
$(info BOARD - $(SUPPORTED_BOARDS) are supported)
$(error BOARD $(BOARD) is not supported, please check it!)
endif

#board definition
BOARD_ID = $(call uc,BOARD_$(VALID_BOARD))
#device usage settings
#must be before include
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/board/$(VALID_BOARD)/$(VALID_BOARD).mk
include $(EMBARC_ROOT)/board/$(VALID_BOARD)/$(VALID_BOARD).mk

##
# \brief	add defines for board
##
BOARD_DEFINES = $(EXTRA_BOARD_DEFINES) -D$(BOARD_ID) -DHW_VERSION=$(BD_VER) -DBOARD_CPU_FREQ=$(CPU_FREQ) -DBOARD_DEV_FREQ=$(DEV_FREQ)
