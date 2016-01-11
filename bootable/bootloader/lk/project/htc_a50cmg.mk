#
include htc/common.mk

LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := htc_a50cmg

MODULES += app/mt_boot \
           dev/lcm


MTK_EMMC_SUPPORT = yes
ARCH_HAVE_MT_RAMDUMP = yes
DEFINES += MTK_NEW_COMBO_EMMC_SUPPORT
MTK_BQ24296_SUPPORT = yes
MTK_KERNEL_POWER_OFF_CHARGING = yes
MTK_LCM_PHYSICAL_ROTATION = 0
CONFIG_CUSTOM_LK_LCM = "nt35521s_hd720_dsi_video_sy hx8394d_hd720_dsi_vdo_truly"
#nt35590_hd720_dsi_cmd_auo = yes

#FASTBOOT_USE_G_ORIGINAL_PROTOCOL = yes
MTK_SECURITY_SW_SUPPORT = yes
MTK_VERIFIED_BOOT_SUPPORT = yes
MTK_SEC_FASTBOOT_UNLOCK_SUPPORT = yes

DEBUG := 0
BOOT_LOGO:=hd720

#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
#DEFINES += MACH_FPGA=y
#DEFINES += SB_LK_BRINGUP=y
DEFINES += MTK_GPT_SCHEME_SUPPORT

ifeq ($(MAKE_HTC_LK),1)
DEFINES += HTC_REBOOT_INFO_PHYS_ADDR=0x83C30000
DEFINES += HTC_REBOOT_INFO_EMMC_PART_OFFSET=0x24600
CFLAGS += -DHTC_REBOOT_INFO_EMMC_PART_NAME=\"para\"

DEFINES += HTC_MEM_RESERVED_PHYS_ADDR=0x83B00000
DEFINES += HTC_MEM_RESERVED_SIZE=0x400000
endif

ifeq ($(ENABLE_BOOTLOADER_LOG),1)
DEFINES += \
	BL_LOG_PHYS=0x83B00000 \
	LAST_BL_LOG_PHYS=0x83B80000 \
	BL_LOG_SIZE=0x80000
endif

