################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../lnkx.cmd 

ASM_SRCS += \
../DSP_restore.asm \
../DSP_utils.asm \
../autoinit.asm \
../boot.asm \
../cbrev32.asm \
../cfft32_noscale.asm \
../twiddle32.asm \
../vectors.asm 

C_SRCS += \
../aic3204.c \
../audio.c \
../ezdsp5535_gpio.c \
../ezdsp5535_i2c.c \
../ezdsp5535_i2s.c \
../ezdsp5535_sar.c \
../main.c \
../pll.c \
../sysinit.c 

OBJS += \
./DSP_restore.obj \
./DSP_utils.obj \
./aic3204.obj \
./audio.obj \
./autoinit.obj \
./boot.obj \
./cbrev32.obj \
./cfft32_noscale.obj \
./ezdsp5535_gpio.obj \
./ezdsp5535_i2c.obj \
./ezdsp5535_i2s.obj \
./ezdsp5535_sar.obj \
./main.obj \
./pll.obj \
./sysinit.obj \
./twiddle32.obj \
./vectors.obj 

ASM_DEPS += \
./DSP_restore.pp \
./DSP_utils.pp \
./autoinit.pp \
./boot.pp \
./cbrev32.pp \
./cfft32_noscale.pp \
./twiddle32.pp \
./vectors.pp 

C_DEPS += \
./aic3204.pp \
./audio.pp \
./ezdsp5535_gpio.pp \
./ezdsp5535_i2c.pp \
./ezdsp5535_i2s.pp \
./ezdsp5535_sar.pp \
./main.pp \
./pll.pp \
./sysinit.pp 

C_DEPS__QUOTED += \
"aic3204.pp" \
"audio.pp" \
"ezdsp5535_gpio.pp" \
"ezdsp5535_i2c.pp" \
"ezdsp5535_i2s.pp" \
"ezdsp5535_sar.pp" \
"main.pp" \
"pll.pp" \
"sysinit.pp" 

OBJS__QUOTED += \
"DSP_restore.obj" \
"DSP_utils.obj" \
"aic3204.obj" \
"audio.obj" \
"autoinit.obj" \
"boot.obj" \
"cbrev32.obj" \
"cfft32_noscale.obj" \
"ezdsp5535_gpio.obj" \
"ezdsp5535_i2c.obj" \
"ezdsp5535_i2s.obj" \
"ezdsp5535_sar.obj" \
"main.obj" \
"pll.obj" \
"sysinit.obj" \
"twiddle32.obj" \
"vectors.obj" 

ASM_DEPS__QUOTED += \
"DSP_restore.pp" \
"DSP_utils.pp" \
"autoinit.pp" \
"boot.pp" \
"cbrev32.pp" \
"cfft32_noscale.pp" \
"twiddle32.pp" \
"vectors.pp" 

ASM_SRCS__QUOTED += \
"../DSP_restore.asm" \
"../DSP_utils.asm" \
"../autoinit.asm" \
"../boot.asm" \
"../twiddle32.asm" \
"../vectors.asm" 

C_SRCS__QUOTED += \
"../aic3204.c" \
"../audio.c" \
"../ezdsp5535_gpio.c" \
"../ezdsp5535_i2c.c" \
"../ezdsp5535_i2s.c" \
"../ezdsp5535_sar.c" \
"../main.c" \
"../pll.c" \
"../sysinit.c" 


