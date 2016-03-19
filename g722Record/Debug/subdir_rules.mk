################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DSP_utils.obj: ../DSP_utils.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DSP_utils.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

aic3204.obj: ../aic3204.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="aic3204.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audio.obj: ../audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="audio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

autoinit.obj: ../autoinit.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="autoinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

boot.obj: ../boot.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="boot.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

csl_rtc.obj: ../csl_rtc.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="csl_rtc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

csl_usb.obj: ../csl_usb.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="csl_usb.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

csl_usbAux.obj: ../csl_usbAux.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="csl_usbAux.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_gpio.obj: ../ezdsp5535_gpio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_gpio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2c.obj: ../ezdsp5535_i2c.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2s.obj: ../ezdsp5535_i2s.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2s.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_lcd.obj: ../ezdsp5535_lcd.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_lcd.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_uart.obj: ../ezdsp5535_uart.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_uart.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

file.obj: ../file.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="file.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pll.obj: ../pll.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="pll.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

rtcM41Txx_i2c.obj: ../rtcM41Txx_i2c.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="rtcM41Txx_i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

schedule.obj: ../schedule.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="schedule.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sysinit.obj: ../sysinit.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="sysinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

timer.obj: ../timer.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="timer.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

usb_dma.obj: ../usb_dma.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="usb_dma.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libg722_1/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/g722Record/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


