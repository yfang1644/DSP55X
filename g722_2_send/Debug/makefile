################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv5/tools/compiler/c5500_4.4.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./vectors.obj" \
"./timer.obj" \
"./sysinit.obj" \
"./pll.obj" \
"./main.obj" \
"./ezdsp5535_spi.obj" \
"./ezdsp5535_sar.obj" \
"./ezdsp5535_lcd.obj" \
"./ezdsp5535_i2s.obj" \
"./ezdsp5535_i2c.obj" \
"./ezdsp5535_gpio.obj" \
"./boot.obj" \
"./autoinit.obj" \
"./audio.obj" \
"./aic3204.obj" \
"./Si446x_api.obj" \
"./DSP_utils.obj" \
-llibg722_2.lib \
-llibfat16.lib \
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
all: g722_2_send.out

# Tool invocations
g722_2_send.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O1 -g --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic -z -m"g722_2_send.map" --stack_size=0x200 --heap_size=0x400 -i"/home/fang/ti/ezdsp5535_v1/tests/libfat16/Debug" -i"/home/fang/ti/ezdsp5535_v1/tests/libg722_2/Debug" --reread_libs --warn_sections --rom_model --sys_stacksize=0x200 -o "g722_2_send.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QUOTED) "g722_2_send.out"
	-$(RM) "Si446x_api.pp" "aic3204.pp" "audio.pp" "ezdsp5535_gpio.pp" "ezdsp5535_i2c.pp" "ezdsp5535_i2s.pp" "ezdsp5535_lcd.pp" "ezdsp5535_sar.pp" "ezdsp5535_spi.pp" "main.pp" "pll.pp" "sysinit.pp" "timer.pp" 
	-$(RM) "DSP_utils.obj" "Si446x_api.obj" "aic3204.obj" "audio.obj" "autoinit.obj" "boot.obj" "ezdsp5535_gpio.obj" "ezdsp5535_i2c.obj" "ezdsp5535_i2s.obj" "ezdsp5535_lcd.obj" "ezdsp5535_sar.obj" "ezdsp5535_spi.obj" "main.obj" "pll.obj" "sysinit.obj" "timer.obj" "vectors.obj" 
	-$(RM) "DSP_utils.pp" "autoinit.pp" "boot.pp" "vectors.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
