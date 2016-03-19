;****************************************************************
; Function:    DSP_restore
;
; Usage: void DSP_restore(Complex *fftx, short *array, int nx, int fft_scale)
;	for(i = 0; i < nx; i++)					// overlap ADD
;	{
;		array[j] = (fftx[i].re) >> fft_scale;
;	}
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.asg	AR0, fftx_ptr			; point to IFFT
	.asg	AR1, array_ptr			; pointer for restored array

	.text

	.def _DSP_restore
_DSP_restore:

	T0 -= #1						; nx
	BRC0 = T0						; loop executes nx times

	T1 = T1 - #16
	T1 = -T1 ||
	localrepeat {
		AC0 = dbl(*fftx_ptr) || fftx_ptr += #4
		*array_ptr+ = HI(saturate(AC0 << T1))
	}

	return						;return to calling function

	.end
;----------------------------------------------------------------
;End of file

