################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DSP_utils.obj: ../DSP_utils.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DSP_utils.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

aic3204.obj: ../aic3204.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="aic3204.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

autoinit.obj: ../autoinit.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="autoinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

boot.obj: ../boot.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="boot.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

divs.obj: ../divs.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="divs.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_gpio.obj: ../ezdsp5535_gpio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_gpio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2c.obj: ../ezdsp5535_i2c.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2s.obj: ../ezdsp5535_i2s.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2s.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_lcd.obj: ../ezdsp5535_lcd.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_lcd.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_sar.obj: ../ezdsp5535_sar.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_sar.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_uart.obj: ../ezdsp5535_uart.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_uart.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pll.obj: ../pll.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="pll.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sine_tbl.obj: ../sine_tbl.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="sine_tbl.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sysinit.obj: ../sysinit.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="sysinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

timer.obj: ../timer.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="timer.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/SweepTest/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


