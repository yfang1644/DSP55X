################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DSP_restore.obj: ../DSP_restore.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DSP_restore.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DSP_utils.obj: ../DSP_utils.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DSP_utils.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

aic3204.obj: ../aic3204.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="aic3204.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audio.obj: ../audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="audio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

autoinit.obj: ../autoinit.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="autoinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

boot.obj: ../boot.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="boot.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

cbrev32.obj: ../cbrev32.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --preproc_with_compile --preproc_dependency="cbrev32.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

cfft32_noscale.obj: ../cfft32_noscale.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --preproc_with_compile --preproc_dependency="cfft32_noscale.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_gpio.obj: ../ezdsp5535_gpio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_gpio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2c.obj: ../ezdsp5535_i2c.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2s.obj: ../ezdsp5535_i2s.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2s.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_sar.obj: ../ezdsp5535_sar.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_sar.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pll.obj: ../pll.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="pll.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sysinit.obj: ../sysinit.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="sysinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

twiddle32.obj: ../twiddle32.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="twiddle32.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -O0 -g --include_path="/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/howling2/include" --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


