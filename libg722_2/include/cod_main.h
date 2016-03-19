/*--------------------------------------------------------------------------*
 *                         COD_MAIN.H                                       *
 *--------------------------------------------------------------------------*
 *       Static memory in the encoder				                        *
 *--------------------------------------------------------------------------*/

#include "cnst.h"                          /* coder constant parameters */

#include "wb_vad.h"
#include "dtx.h"
void arrMpy(Word16 *x, Word16 g, Word16 lg);

/* LPC interpolation coef {0.45, 0.8, 0.96, 1.0}; in Q15 */
extern Word16 interpol_frac[];

/* isp tables for initialization */
extern Word16 isp_init[];
extern Word16 isf_init[];

/* High Band encoding */
extern Word16 HP_gain[];

extern Word16 nb_of_bits[];

typedef struct
{
    Word16 mem_decim[2 * L_FILT16k];       /* speech decimated filter memory */
    Word16 mem_sig_in[6];                  /* hp50 filter memory */
    Word16 mem_preemph;                    /* speech preemph filter memory */
    Word16 old_speech[L_TOTAL - L_FRAME];  /* old speech vector at 12.8kHz */
    Word16 old_wsp[PIT_MAX / OPL_DECIM];   /* old decimated weighted speech vector */
    Word16 old_exc[PIT_MAX + L_INTERPOL];  /* old excitation vector */
    Word16 mem_levinson[M + 2];            /* levinson routine memory */
    Word16 ispold[M];                      /* old isp (immittance spectral pairs) */
    Word16 ispold_q[M];                    /* quantized old isp */
    Word16 past_isfq[M];                   /* past isf quantizer */
    Word16 mem_wsp;                        /* wsp vector memory */
    Word16 mem_decim2[3];                  /* wsp decimation filter memory */
    Word16 mem_w0;                         /* target vector memory */
    Word16 mem_syn[M];                     /* synthesis memory */
    Word16 tilt_code;                      /* tilt of code */
    Word16 old_wsp_max;                    /* old wsp maximum value */
    Word16 old_wsp_shift;                  /* old wsp shift */
    Word16 Q_old;                          /* old scaling factor */
    Word16 Q_max[2];                       /* old maximum scaling factor */
    Word16 gp_clip[2];                     /* gain of pitch clipping memory */
    Word16 qua_gain[4];                    /* gain quantizer memory */

    Word16 old_T0_med;
    Word16 ol_gain;
    Word16 ada_w;
    Word16 ol_wght_flg;
    Word16 old_ol_lag[5];
    Word16 hp_wsp_mem[9];
    Word16 old_hp_wsp[L_FRAME / OPL_DECIM + (PIT_MAX / OPL_DECIM)];
    VadVars *vadSt;
    dtx_encState *dtx_encSt;
    Word16 first_frame;

    Word16 isfold[M];                      /* old isf (frequency domain) */
    Word32 L_gc_thres;                     /* threshold for noise enhancer */
    Word16 mem_syn_hi[M];                  /* modified synthesis memory (MSB) */
    Word16 mem_syn_lo[M];                  /* modified synthesis memory (LSB) */
    Word16 mem_deemph;                     /* speech deemph filter memory */
    Word16 mem_sig_out[6];                 /* hp50 filter memory for synthesis */
    Word16 mem_hp400[6];                   /* hp400 filter memory for synthesis */
    Word16 mem_oversamp[2 * L_FILT];       /* synthesis oversampled filter memory */
    Word16 mem_syn_hf[M];                  /* HF synthesis memory */
    Word16 mem_hf[2 * L_FILT16k];          /* HF band-pass filter memory */
    Word16 mem_hf2[2 * L_FILT16k];         /* HF band-pass filter memory */
    Word16 seed2;                          /* random memory for HF generation */
    Word16 vad_hist;

    Word16 gain_alpha;

} Coder_State;

void Init_coder(Coder_State *spe_state);

void coder(
     Word16 * mode,          /* input :  used mode                          */
     Word16 speech16k[],     /* input :  320 new speech samples (at 16 kHz) */
     Word16 prms[],          /* output:  output parameters                  */
     Word16 * ser_size,      /* output:  bit rate of the used mode          */
     Coder_State *spe_state, /* i/o   :  State structure                    */
     Word16 allow_dtx        /* input :  DTX ON/OFF                         */
);

void Reset_encoder(Coder_State *st, Word16 reset_all);

Word16 encoder_homing_frame_test(Word16 input_frame[]);

