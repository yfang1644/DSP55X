void cfft32_NOSCALE(Complex *x, u16 nx);
void cbrev32(Complex *x, Complex *y, u16 nx);

void DSP_load_dual(Complex *fftx, s16 *ch0, s16 *ch1, u16 nx);
void DSP_load_mono(Complex *fftx, s16 *ch0, u16 nx);

void DSP_postfft(Complex *fftx, u16 nx);
void DSP_dual_cancel(Complex *fftx, u16 nx);
void DSP_mono_cancel(Complex *fftx, u16 nx);
void DSP_filter(Complex *fftx, s16 *f, u16 nx);
void DSP_mirror(Complex *fftx, u16 nx);
void DSP_restore(Complex *fftx, s16 *ch0, u16 nx);
s16 DSP_lms(s16 *dbuf, s32 *filter, int mu, int error, int nh);
s16 DSPF_lms(s16 *dbuf, s32 *filter, int mu, int error, int nh);
s32 DSP_minval(s32 *array, u16 cnt);
u32 DSP_power(s16 *array, u16 cnt);
void proc_adaptive(u16 taps);

u32 proc_spec_cancel(u16 function);
u32 proc_spec_cancel_(s16 *flt, s16 dual, s16 cancel);

void voice_activity_detect(s16 *signal);

void proc_filter(u16 filter_no);

