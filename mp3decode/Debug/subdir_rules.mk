################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DSP_utils.obj: ../DSP_utils.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DSP_utils.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

aic3204.obj: ../aic3204.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="aic3204.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audio.obj: ../audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="audio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

autoinit.obj: ../autoinit.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="autoinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

bit.obj: ../bit.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="bit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

boot.obj: ../boot.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="boot.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

csl_rtc.obj: ../csl_rtc.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="csl_rtc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

decoder.obj: ../decoder.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="decoder.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

divli.obj: ../divli.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="divli.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_gpio.obj: ../ezdsp5535_gpio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_gpio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2c.obj: ../ezdsp5535_i2c.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_i2s.obj: ../ezdsp5535_i2s.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_i2s.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_lcd.obj: ../ezdsp5535_lcd.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_lcd.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ezdsp5535_timer.obj: ../ezdsp5535_timer.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="ezdsp5535_timer.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

file.obj: ../file.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="file.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

frame.obj: ../frame.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="frame.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

huffman.obj: ../huffman.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="huffman.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

layer12.obj: ../layer12.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="layer12.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

layer3.obj: ../layer3.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="layer3.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pll.obj: ../pll.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="pll.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

remul.obj: ../remul.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="remul.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

stream.obj: ../stream.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="stream.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

synth.obj: ../synth.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="synth.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sysinit.obj: ../sysinit.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="sysinit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

timer.obj: ../timer.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="timer.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=large -g --include_path="/home/fang/ti/ezdsp5535_v1/c55xx_csl/inc" --include_path="/home/fang/ti/ezdsp5535_v1/tests/libfatfs/include" --include_path="/home/fang/ti/ezdsp5535_v1/tests/mp3decode/include" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


