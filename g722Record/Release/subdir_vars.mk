################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../lnkx.cmd 

ASM_SRCS += \
../DSP_utils.asm \
../autoinit.asm \
../boot.asm \
../idiv.asm \
../lmpy.asm \
../udiv.asm \
../vectors.asm 

C_SRCS += \
../aic3204.c \
../audio.c \
../coef2sam.c \
../common.c \
../csl_mmcsd.c \
../csl_rtc.c \
../csl_usb.c \
../csl_usbAux.c \
../dct4_a.c \
../dct4_s.c \
../decoder.c \
../diskio.c \
../divul.c \
../encoder.c \
../ezdsp5535_gpio.c \
../ezdsp5535_i2c.c \
../ezdsp5535_i2s.c \
../ezdsp5535_lcd.c \
../ezdsp5535_sdcard.c \
../ezdsp5535_uart.c \
../ff.c \
../file.c \
../huff_tab.c \
../main.c \
../pll.c \
../rtcM41Txx_i2c.c \
../sam2coef.c \
../schedule.c \
../sysinit.c \
../tables.c \
../timer.c \
../usb_dma.c 

OBJS += \
./DSP_utils.obj \
./aic3204.obj \
./audio.obj \
./autoinit.obj \
./boot.obj \
./coef2sam.obj \
./common.obj \
./csl_mmcsd.obj \
./csl_rtc.obj \
./csl_usb.obj \
./csl_usbAux.obj \
./dct4_a.obj \
./dct4_s.obj \
./decoder.obj \
./diskio.obj \
./divul.obj \
./encoder.obj \
./ezdsp5535_gpio.obj \
./ezdsp5535_i2c.obj \
./ezdsp5535_i2s.obj \
./ezdsp5535_lcd.obj \
./ezdsp5535_sdcard.obj \
./ezdsp5535_uart.obj \
./ff.obj \
./file.obj \
./huff_tab.obj \
./idiv.obj \
./lmpy.obj \
./main.obj \
./pll.obj \
./rtcM41Txx_i2c.obj \
./sam2coef.obj \
./schedule.obj \
./sysinit.obj \
./tables.obj \
./timer.obj \
./udiv.obj \
./usb_dma.obj \
./vectors.obj 

ASM_DEPS += \
./DSP_utils.pp \
./autoinit.pp \
./boot.pp \
./idiv.pp \
./lmpy.pp \
./udiv.pp \
./vectors.pp 

C_DEPS += \
./aic3204.pp \
./audio.pp \
./coef2sam.pp \
./common.pp \
./csl_mmcsd.pp \
./csl_rtc.pp \
./csl_usb.pp \
./csl_usbAux.pp \
./dct4_a.pp \
./dct4_s.pp \
./decoder.pp \
./diskio.pp \
./divul.pp \
./encoder.pp \
./ezdsp5535_gpio.pp \
./ezdsp5535_i2c.pp \
./ezdsp5535_i2s.pp \
./ezdsp5535_lcd.pp \
./ezdsp5535_sdcard.pp \
./ezdsp5535_uart.pp \
./ff.pp \
./file.pp \
./huff_tab.pp \
./main.pp \
./pll.pp \
./rtcM41Txx_i2c.pp \
./sam2coef.pp \
./schedule.pp \
./sysinit.pp \
./tables.pp \
./timer.pp \
./usb_dma.pp 

C_DEPS__QUOTED += \
"aic3204.pp" \
"audio.pp" \
"coef2sam.pp" \
"common.pp" \
"csl_mmcsd.pp" \
"csl_rtc.pp" \
"csl_usb.pp" \
"csl_usbAux.pp" \
"dct4_a.pp" \
"dct4_s.pp" \
"decoder.pp" \
"diskio.pp" \
"divul.pp" \
"encoder.pp" \
"ezdsp5535_gpio.pp" \
"ezdsp5535_i2c.pp" \
"ezdsp5535_i2s.pp" \
"ezdsp5535_lcd.pp" \
"ezdsp5535_sdcard.pp" \
"ezdsp5535_uart.pp" \
"ff.pp" \
"file.pp" \
"huff_tab.pp" \
"main.pp" \
"pll.pp" \
"rtcM41Txx_i2c.pp" \
"sam2coef.pp" \
"schedule.pp" \
"sysinit.pp" \
"tables.pp" \
"timer.pp" \
"usb_dma.pp" 

OBJS__QUOTED += \
"DSP_utils.obj" \
"aic3204.obj" \
"audio.obj" \
"autoinit.obj" \
"boot.obj" \
"coef2sam.obj" \
"common.obj" \
"csl_mmcsd.obj" \
"csl_rtc.obj" \
"csl_usb.obj" \
"csl_usbAux.obj" \
"dct4_a.obj" \
"dct4_s.obj" \
"decoder.obj" \
"diskio.obj" \
"divul.obj" \
"encoder.obj" \
"ezdsp5535_gpio.obj" \
"ezdsp5535_i2c.obj" \
"ezdsp5535_i2s.obj" \
"ezdsp5535_lcd.obj" \
"ezdsp5535_sdcard.obj" \
"ezdsp5535_uart.obj" \
"ff.obj" \
"file.obj" \
"huff_tab.obj" \
"idiv.obj" \
"lmpy.obj" \
"main.obj" \
"pll.obj" \
"rtcM41Txx_i2c.obj" \
"sam2coef.obj" \
"schedule.obj" \
"sysinit.obj" \
"tables.obj" \
"timer.obj" \
"udiv.obj" \
"usb_dma.obj" \
"vectors.obj" 

ASM_DEPS__QUOTED += \
"DSP_utils.pp" \
"autoinit.pp" \
"boot.pp" \
"idiv.pp" \
"lmpy.pp" \
"udiv.pp" \
"vectors.pp" 

ASM_SRCS__QUOTED += \
"../DSP_utils.asm" \
"../autoinit.asm" \
"../boot.asm" \
"../idiv.asm" \
"../lmpy.asm" \
"../udiv.asm" \
"../vectors.asm" 

C_SRCS__QUOTED += \
"../aic3204.c" \
"../audio.c" \
"../coef2sam.c" \
"../common.c" \
"../csl_mmcsd.c" \
"../csl_rtc.c" \
"../csl_usb.c" \
"../csl_usbAux.c" \
"../dct4_a.c" \
"../dct4_s.c" \
"../decoder.c" \
"../diskio.c" \
"../divul.c" \
"../encoder.c" \
"../ezdsp5535_gpio.c" \
"../ezdsp5535_i2c.c" \
"../ezdsp5535_i2s.c" \
"../ezdsp5535_lcd.c" \
"../ezdsp5535_sdcard.c" \
"../ezdsp5535_uart.c" \
"../ff.c" \
"../file.c" \
"../huff_tab.c" \
"../main.c" \
"../pll.c" \
"../rtcM41Txx_i2c.c" \
"../sam2coef.c" \
"../schedule.c" \
"../sysinit.c" \
"../tables.c" \
"../timer.c" \
"../usb_dma.c" 


