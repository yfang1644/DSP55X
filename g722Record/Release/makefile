################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv5/tools/compiler/c5500_4.4.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./vectors.obj" \
"./usb_dma.obj" \
"./udiv.obj" \
"./timer.obj" \
"./tables.obj" \
"./sysinit.obj" \
"./schedule.obj" \
"./sam2coef.obj" \
"./rtcM41Txx_i2c.obj" \
"./pll.obj" \
"./main.obj" \
"./lmpy.obj" \
"./idiv.obj" \
"./huff_tab.obj" \
"./file.obj" \
"./ff.obj" \
"./ezdsp5535_uart.obj" \
"./ezdsp5535_sdcard.obj" \
"./ezdsp5535_lcd.obj" \
"./ezdsp5535_i2s.obj" \
"./ezdsp5535_i2c.obj" \
"./ezdsp5535_gpio.obj" \
"./encoder.obj" \
"./divul.obj" \
"./diskio.obj" \
"./decoder.obj" \
"./dct4_s.obj" \
"./dct4_a.obj" \
"./csl_usbAux.obj" \
"./csl_usb.obj" \
"./csl_rtc.obj" \
"./csl_mmcsd.obj" \
"./common.obj" \
"./coef2sam.obj" \
"./boot.obj" \
"./autoinit.obj" \
"./audio.obj" \
"./aic3204.obj" \
"./DSP_utils.obj" \
"../lnkx.cmd" \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: g722Record.out

# Tool invocations
g722Record.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --define=c5535 --diag_warning=225 --ptrdiff_size=16 --algebraic -z -m"g722Record.map" --stack_size=0x200 --heap_size=0x400 --reread_libs --warn_sections --rom_model --sys_stacksize=0x200 -o "g722Record.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QUOTED) "g722Record.out"
	-$(RM) "aic3204.pp" "audio.pp" "coef2sam.pp" "common.pp" "csl_mmcsd.pp" "csl_rtc.pp" "csl_usb.pp" "csl_usbAux.pp" "dct4_a.pp" "dct4_s.pp" "decoder.pp" "diskio.pp" "divul.pp" "encoder.pp" "ezdsp5535_gpio.pp" "ezdsp5535_i2c.pp" "ezdsp5535_i2s.pp" "ezdsp5535_lcd.pp" "ezdsp5535_sdcard.pp" "ezdsp5535_uart.pp" "ff.pp" "file.pp" "huff_tab.pp" "main.pp" "pll.pp" "rtcM41Txx_i2c.pp" "sam2coef.pp" "schedule.pp" "sysinit.pp" "tables.pp" "timer.pp" "usb_dma.pp" 
	-$(RM) "DSP_utils.obj" "aic3204.obj" "audio.obj" "autoinit.obj" "boot.obj" "coef2sam.obj" "common.obj" "csl_mmcsd.obj" "csl_rtc.obj" "csl_usb.obj" "csl_usbAux.obj" "dct4_a.obj" "dct4_s.obj" "decoder.obj" "diskio.obj" "divul.obj" "encoder.obj" "ezdsp5535_gpio.obj" "ezdsp5535_i2c.obj" "ezdsp5535_i2s.obj" "ezdsp5535_lcd.obj" "ezdsp5535_sdcard.obj" "ezdsp5535_uart.obj" "ff.obj" "file.obj" "huff_tab.obj" "idiv.obj" "lmpy.obj" "main.obj" "pll.obj" "rtcM41Txx_i2c.obj" "sam2coef.obj" "schedule.obj" "sysinit.obj" "tables.obj" "timer.obj" "udiv.obj" "usb_dma.obj" "vectors.obj" 
	-$(RM) "DSP_utils.pp" "autoinit.pp" "boot.pp" "idiv.pp" "lmpy.pp" "udiv.pp" "vectors.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
