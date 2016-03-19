/*------------------------------------------------------------------------*
 *                         COD_MAIN.C                                     *
 *------------------------------------------------------------------------*
 * Performs the main encoder routine                                      *
 *------------------------------------------------------------------------*/

/*___________________________________________________________________________
 |                                                                           |
 | Fixed-point C simulation of AMR WB ACELP coding algorithm with 20 ms      |
 | speech frames for wideband speech signals.                                |
 |___________________________________________________________________________|
*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "cnst.h"
#include "acelp.h"
#include "bits.h"
#include "cod_main.h"

/*-----------------------------------------------------*
 * Function synthesis()                                *
 *                                                     *
 * Synthesis of signal at 16kHz with HF extension.     *
 *                                                     *
 *-----------------------------------------------------*/

Word16 synthesis_enc(
     Word16 Aq[],              /* A(z)  : quantized Az               */
     Word16 exc[],             /* (i)   : excitation at 12kHz        */
     Word16 Q_new,             /* (i)   : scaling performed on exc   */
     Word16 synth16k[],        /* (o)   : 16kHz synthesis signal     */
     Coder_State * st          /* (i/o) : State structure            */
)
{
    Word16 i, fac, tmp, exp;
    Word16 ener, exp_ener;
    Word32 L_tmp;

    Word16 synth_hi[M + L_SUBFR], synth_lo[M + L_SUBFR];
    Word16 synth[L_SUBFR];
    Word16 HF[L_SUBFR16k];                 /* High Frequency vector      */
    Word16 Ap[M + 1];

    Word16 HF_SP[L_SUBFR16k];              /* High Frequency vector (from original signal) */

    Word16 HP_est_gain, HP_calc_gain, HP_corr_gain;
    Word16 dist_min, dist;
    Word16 HP_gain_ind = 0;
    Word16 gain1, gain2;

    /*------------------------------------------------------------*
     * speech synthesis                                           *
     * ~~~~~~~~~~~~~~~~                                           *
     * - Find synthesis speech corresponding to exc2[].           *
     * - Perform fixed deemphasis and hp 50hz filtering.          *
     * - Oversampling from 12.8kHz to 16kHz.                      *
     *------------------------------------------------------------*/


    Copy(st->mem_syn_hi, synth_hi, M);
    Copy(st->mem_syn_lo, synth_lo, M);

    Syn_filt_32(Aq, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);

    Copy(synth_hi + L_SUBFR, st->mem_syn_hi, M);
    Copy(synth_lo + L_SUBFR, st->mem_syn_lo, M);

    Deemph_32(synth_hi + M, synth_lo + M, synth, PREEMPH_FAC, L_SUBFR, &(st->mem_deemph));

    HP50_12k8(synth, L_SUBFR, st->mem_sig_out);

    /* Original speech signal as reference for high band gain quantisation */
    /* generate white noise vector */
    for (i = 0; i < L_SUBFR16k; i++)
    {
        HF_SP[i] = synth16k[i];
        HF[i] = Random(&(st->seed2)) >> 3;
    }

    /*------------------------------------------------------*
     * HF noise synthesis                                   *
     * ~~~~~~~~~~~~~~~~~~                                   *
     * - Generate HF noise between 5.5 and 7.5 kHz.         *
     * - Set energy of noise according to synthesis tilt.   *
     *     tilt > 0.8 ==> - 14 dB (voiced)                  *
     *     tilt   0.5 ==> - 6 dB  (voiced or noise)         *
     *     tilt < 0.0 ==>   0 dB  (noise)                   *
     *------------------------------------------------------*/


    /* energy of excitation */

    Scale_sig(exc, L_SUBFR, -3);
    Q_new = sub(Q_new, 3);

    L_tmp = Dot_product12(exc, exc, L_SUBFR, &exp_ener);
    ener = (Word16)(L_tmp >> 16);
    exp_ener = sub(exp_ener, add(Q_new, Q_new));

    /* set energy of white noise to energy of excitation */
    L_tmp = Dot_product12(HF, HF, L_SUBFR16k, &exp);
    tmp = (Word16)(L_tmp >> 16);

    if (tmp > ener)
    {
        tmp >>= 1;                         /* Be sure tmp < ener */
        exp++;
    }
    tmp = div_s(tmp, ener);
    L_tmp = (Word32)tmp << 16;             /* result is normalized */
    exp = sub(exp, exp_ener);
    L_tmp = Isqrt_n(L_tmp, &exp);
    L_tmp = L_shl(L_tmp, sub(1, exp));     /* L_tmp x 2, L_tmp in Q31 */
    tmp = (Word16)(L_tmp >> 16);           /* tmp = 2xsqrt(ener_exc/ener_hf) */

	arrMpy(HF, tmp, L_SUBFR16k);
//    for (i = 0; i < L_SUBFR16k; i++)
//    {
//        HF[i] = mult(HF[i], tmp);
//    }

    /* find tilt of synthesis speech (tilt: 1=voiced, -1=unvoiced) */

    HP400_12k8(synth, L_SUBFR, st->mem_hp400);

    L_tmp = Dot_product12(synth, synth, L_SUBFR, &exp);
    ener = (Word16)(L_tmp >> 16);   /* ener = r[0] */

    L_tmp = Dot_product(synth, &synth[1], L_SUBFR - 1);
    L_tmp++;                        // useless, but keep compatible result
    L_tmp = L_shl(L_tmp, (-exp));
    tmp = (Word16)(L_tmp >> 16);    /* tmp = r[1] */

    if (tmp > 0)
    {
        fac = div_s(tmp, ener);
    } else
    {
        fac = 0;
    }

    /* modify energy of white noise according to synthesis tilt */
    gain1 = 32767 - fac;
    gain2 = mult(gain1, 20480);
    gain2 = shl(gain2, 1);

    if (st->vad_hist > 0)
    {
        HP_est_gain = gain2;
    } else
    {
        HP_est_gain = gain1;
    }

    if (HP_est_gain < 3277)
    {
        HP_est_gain = 3277;                /* 0.1 in Q15 */
    }
    /* synthesis of noise: 4.8kHz..5.6kHz --> 6kHz..7kHz */
    Weight_a(Aq, Ap, 19661, M);            /* fac=0.6 */
    Syn_filt(Ap, M, HF, HF, L_SUBFR16k, st->mem_syn_hf, 1);

    /* noise High Pass filtering (1ms of delay) */
    Filt_6k_7k(HF, L_SUBFR16k, st->mem_hf);

    /* filtering of the original signal */
    Filt_6k_7k(HF_SP, L_SUBFR16k, st->mem_hf2);

    /* check the gain difference */
    Scale_sig(HF_SP, L_SUBFR16k, -1);

    L_tmp = Dot_product12(HF_SP, HF_SP, L_SUBFR16k, &exp_ener);
    ener = (Word16)(L_tmp >> 16);

    /* set energy of white noise to energy of excitation */
    L_tmp = Dot_product12(HF, HF, L_SUBFR16k, &exp);
    tmp = (Word16)(L_tmp >> 16);

    if (tmp > ener)
    {
        tmp >>= 1;                 /* Be sure tmp < ener */
        exp++;
    }

    tmp = div_s(tmp, ener);                /* result is normalized */
    L_tmp = (Word32)tmp << 16;
    exp = sub(exp, exp_ener);
    L_tmp = Isqrt_n(L_tmp, &exp);
    L_tmp = L_shl(L_tmp, (-exp));          /* L_tmp, L_tmp in Q31 */
    HP_calc_gain = (Word16)(L_tmp >> 16);  /* tmp = sqrt(ener_input/ener_hf) */

    /* st->gain_alpha *= st->dtx_encSt->dtxHangoverCount/7 */
//    L_tmp = L_shl(L_mult(st->dtx_encSt->dtxHangoverCount, 4681), 15);
//    st->gain_alpha = mult(st->gain_alpha, (Word16)(L_tmp >> 16));

    if (st->dtx_encSt->dtxHangoverCount > 6)
    {
        st->gain_alpha = MAX_16;
    }
    else
    {
        tmp = st->dtx_encSt->dtxHangoverCount * 4681;
        st->gain_alpha = mult(st->gain_alpha, tmp);
    }

    HP_est_gain = HP_est_gain >> 1;     /* From Q15 to Q14 */
    HP_corr_gain = add(mult(HP_calc_gain, st->gain_alpha), mult(sub(32767, st->gain_alpha), HP_est_gain));

    /* Quantise the correction gain */
    dist_min = MAX_16;
    for (i = 0; i < 16; i++)
    {
        tmp = sub(HP_corr_gain, HP_gain[i]);
        dist = mult(tmp, tmp);
        if (dist_min > dist)
        {
            dist_min = dist;
            HP_gain_ind = i;
        }
    }

    HP_corr_gain = HP_gain[HP_gain_ind];

    /* return the quantised gain index when using the highest mode, otherwise zero */
    return (HP_gain_ind);
}


void Reset_encoder(Coder_State *cod_state, Word16 reset_all)
{
    Word16 i;

    Set_zero(cod_state->old_exc, PIT_MAX + L_INTERPOL);
    Set_zero(cod_state->mem_syn, M);
    Set_zero(cod_state->past_isfq, M);

    cod_state->mem_w0 = 0;
    cod_state->tilt_code = 0;
    cod_state->first_frame = 1;

    Init_gp_clip(cod_state->gp_clip);

    cod_state->L_gc_thres = 0;

    if (reset_all != 0)
    {
        /* Static vectors to zero */

        Set_zero(cod_state->old_speech, L_TOTAL - L_FRAME);
        Set_zero(cod_state->old_wsp, (PIT_MAX / OPL_DECIM));
        Set_zero(cod_state->mem_decim2, 3);

        /* routines initialization */

        Set_zero(cod_state->mem_decim, 30);	//Init_Decim_12k8
        Set_zero(cod_state->mem_sig_in, 6);	//Init_HP50_12k8
        Set_zero(cod_state->mem_levinson, M + 2);	//Init_Levinson
        Init_Q_gain2(cod_state->qua_gain);
        Set_zero(cod_state->hp_wsp_mem, 9);	//Init_Hp_wsp

        /* isp initialization */

        Copy(isp_init, cod_state->ispold, M);
        Copy(isp_init, cod_state->ispold_q, M);

        /* variable initialization */

        cod_state->mem_preemph = 0;
        cod_state->mem_wsp = 0;
        cod_state->Q_old = 15;
        cod_state->Q_max[0] = 15;
        cod_state->Q_max[1] = 15;
        cod_state->old_wsp_max = 0;
        cod_state->old_wsp_shift = 0;

        /* pitch ol initialization */

        cod_state->old_T0_med = 40;
        cod_state->ol_gain = 0;
        cod_state->ada_w = 0;
        cod_state->ol_wght_flg = 0;
        for (i = 0; i < 5; i++)
        {
            cod_state->old_ol_lag[i] = 40;
        }
        Set_zero(cod_state->old_hp_wsp, (L_FRAME / 2) / OPL_DECIM + (PIT_MAX / OPL_DECIM));

        Set_zero(cod_state->mem_syn_hf, M);
        Set_zero(cod_state->mem_syn_hi, M);
        Set_zero(cod_state->mem_syn_lo, M);

        Set_zero(cod_state->mem_sig_out, 6);	//Init_HP50_12k8
        Set_zero(cod_state->mem_hf, 30);		//Init_Filt_6k_7k
        Set_zero(cod_state->mem_hp400, 6);	//Init_HP400_12k8

        Copy(isf_init, cod_state->isfold, M);

        cod_state->mem_deemph = 0;

        cod_state->seed2 = 21845;

        Set_zero(cod_state->mem_hf2, 30);	//Init_Filt_6k_7k
        cod_state->gain_alpha = 32767;

        cod_state->vad_hist = 0;

        dtx_enc_reset(cod_state->dtx_encSt, isf_init);
    }
    return;
}

/*-----------------------------------------------------------------*
 *   Funtion  init_coder                                           *
 *            ~~~~~~~~~~                                           *
 *   ->Initialization of variables for the coder section.          *
 *-----------------------------------------------------------------*/

void Init_coder(Coder_State *spe_state)
{
    wb_vad_init(&(spe_state->vadSt));
    dtx_enc_init(&(spe_state->dtx_encSt), isf_init);

    Reset_encoder(spe_state, 1);

    return;
}

/*-----------------------------------------------------------------*
 *   Funtion  coder                                                *
 *            ~~~~~                                                *
 *   ->Main coder routine.                                         *
 *                                                                 *
 *-----------------------------------------------------------------*/

void coder(
     Word16 * mode,          /* input :  used mode                          */
     Word16 speech16k[],     /* input :  320 new speech samples (at 16 kHz) */
     Word16 prms[],          /* output:  output parameters                  */
     Word16 * ser_size,      /* output:  bit rate of the used mode          */
     Coder_State *spe_state, /* i/o   :  State structure                    */
     Word16 allow_dtx        /* input :  DTX ON/OFF                         */
)
{
    /* Speech vector */
    Word16 old_speech[L_TOTAL];
    Word16 *new_speech, *speech, *p_window;

    /* Weighted speech vector */
    Word16 old_wsp[L_FRAME + (PIT_MAX / OPL_DECIM)];
    Word16 *wsp;

    /* Excitation vector */
    Word16 old_exc[(L_FRAME + 1) + PIT_MAX + L_INTERPOL];
    Word16 *exc;

    /* LPC coefficients */

    Word32 r_h[M + 1];               /* Autocorrelations of windowed speech  */
    Word16 rc[M];                    /* Reflection coefficients.             */
    Word16 Ap[M + 1];                /* A(z) with spectral expansion         */
    Word16 ispnew[M];                /* immittance spectral pairs at 4nd sfr */
    Word16 ispnew_q[M];              /* quantized ISPs at 4nd subframe       */
    Word16 isf[M];                   /* ISF (frequency domain) at 4nd sfr    */
    Word16 *p_A, *p_Aq;              /* ptr to A(z) for the 4 subframes      */
    Word16 A[NB_SUBFR * (M + 1)];    /* A(z) unquantized for the 4 subframes */
    Word16 Aq[NB_SUBFR * (M + 1)];   /* A(z)   quantized for the 4 subframes */

    /* Other vectors */

    Word16 xn[L_SUBFR];              /* Target vector for pitch search     */
    Word16 xn2[L_SUBFR];             /* Target vector for codebook search  */
    Word16 dn[L_SUBFR];              /* Correlation between xn2 and h1     */
    Word16 cn[L_SUBFR];              /* Target vector in residual domain   */

    Word16 h1[L_SUBFR];              /* Impulse response vector            */
    Word16 h2[L_SUBFR];              /* Impulse response vector            */
    Word16 code[L_SUBFR];            /* Fixed codebook excitation          */
    Word16 y1[L_SUBFR];              /* Filtered adaptive excitation       */
    Word16 y2[L_SUBFR];              /* Filtered adaptive excitation       */
    Word16 error[M + L_SUBFR];       /* error of quantization              */
    Word16 synth[L_SUBFR];           /* 12.8kHz synthesis vector           */
    Word16 exc2[L_FRAME];            /* excitation vector                  */
    Word16 buf[L_FRAME];             /* VAD buffer                         */

    /* Scalars */

    Word16 i, j, i_subfr, select, pit_flag, clip_gain, vad_flag;
    Word16 codec_mode;
    Word16 T_op, T_op2, T0, T0_min, T0_max, T0_frac, index;
    Word16 gain_pit, gain_code, g_coeff[4], g_coeff2[4];
    Word16 tmp, gain1, gain2, exp, Q_new, mu, shift, max;
    Word16 voice_fac;
    Word16 indice[8];

    Word32 L_tmp, L_gain_code, L_max;

    Word16 code2[L_SUBFR];                 /* Fixed codebook excitation  */
    Word16 stab_fac, fac;

    Word16 corr_gain;

    Word16 reset_flag;

    /* check for homing frame */
    reset_flag = encoder_homing_frame_test(speech16k);

    *ser_size = nb_of_bits[*mode];
    codec_mode = *mode;

    /*---------------------------------------------------------------------*
     *          Initialize pointers to speech vector.                      *
     *                                                                     *
     *                                                                     *
     *               |-------|-------|-------|-------|-------|-------|     *
     *                past sp   sf1     sf2     sf3     sf4    L_NEXT      *
     *               <-------  Total speech buffer (L_TOTAL)   ------>     *
     *         old_speech                                                  *
     *               <-------  LPC analysis window (L_WINDOW)  ------>     *
     *               |       <-- present frame (L_FRAME) ---->             *
     *              p_window |       <----- new speech (L_FRAME) ---->     *
     *                       |       |                                     *
     *                     speech    |                                     *
     *                            new_speech                               *
     *---------------------------------------------------------------------*/

    new_speech = old_speech + L_TOTAL - L_FRAME - L_FILT;      /* New speech     */
    speech = old_speech + L_TOTAL - L_FRAME - L_NEXT;     /* Present frame  */
    p_window = old_speech + L_TOTAL - L_WINDOW;

    exc = old_exc + PIT_MAX + L_INTERPOL;
    wsp = old_wsp + (PIT_MAX / OPL_DECIM);

    /* copy coder memory state into working space (internal memory for DSP) */

    Copy(spe_state->old_speech, old_speech, L_TOTAL - L_FRAME);
    Copy(spe_state->old_wsp, old_wsp, PIT_MAX / OPL_DECIM);
    Copy(spe_state->old_exc, old_exc, PIT_MAX + L_INTERPOL);

    /*---------------------------------------------------------------*
     * Down sampling signal from 16kHz to 12.8kHz                    *
     * -> The signal is extended by L_FILT samples (padded to zero)  *
     * to avoid additional delay (L_FILT samples) in the coder.      *
     * The last L_FILT samples are approximated after decimation and *
     * are used (and windowed) only in autocorrelations.             *
     *---------------------------------------------------------------*/

    Decim_12k8(speech16k, L_FRAME16k, new_speech, spe_state->mem_decim);

    /* last L_FILT samples for autocorrelation window */
    Copy(spe_state->mem_decim, code, 2 * L_FILT16k);
    Set_zero(error, L_FILT16k);            /* set next sample to zero */
    Decim_12k8(error, L_FILT16k, new_speech + L_FRAME, code);

    /*---------------------------------------------------------------*
     * Perform 50Hz HP filtering of input signal.                    *
     *---------------------------------------------------------------*/

    HP50_12k8(new_speech, L_FRAME, spe_state->mem_sig_in);

    /* last L_FILT samples for autocorrelation window */
    Copy(spe_state->mem_sig_in, code, 6);
    HP50_12k8(new_speech + L_FRAME, L_FILT, code);

    /*---------------------------------------------------------------*
     * Perform fixed preemphasis through 1 - g z^-1                  *
     * Scale signal to get maximum of precision in filtering         *
     *---------------------------------------------------------------*/

    mu = PREEMPH_FAC >> 1;              /* Q15 --> Q14 */

    /* get max of new preemphased samples (L_FRAME+L_FILT) */

    L_tmp = L_mult(new_speech[0], 0x4000);
    L_tmp = L_msu(L_tmp, spe_state->mem_preemph, mu);
    L_max = L_abs(L_tmp);

    for (i = 1; i < L_FRAME + L_FILT; i++)
    {
        L_tmp = L_mult(new_speech[i], 0x4000);
        L_tmp = L_msu(L_tmp, new_speech[i - 1], mu);
        L_tmp = L_abs(L_tmp);
        if (L_tmp > L_max)
        {
            L_max = L_tmp;
        }
    }

    /* get scaling factor for new and previous samples */
    /* limit scaling to Q_MAX to keep dynamic for ringing in low signal */
    /* limit scaling to Q_MAX also to avoid a[0]<1 in syn_filt_32 */
    tmp = (Word16)(L_max >> 16);
    if (tmp == 0)
    {
        shift = Q_MAX;
    } else
    {
        shift = norm_s0(tmp);
        shift = shift - 1;

        if (shift < 0)
        {
            shift = 0;
        }

        if (shift > Q_MAX)
        {
            shift = Q_MAX;
        }
    }
    Q_new = shift;
    if (Q_new > spe_state->Q_max[0])
    {
        Q_new = spe_state->Q_max[0];
    }
    if (Q_new > spe_state->Q_max[1])
    {
        Q_new = spe_state->Q_max[1];
    }
    exp = sub(Q_new, spe_state->Q_old);
    spe_state->Q_old = Q_new;
    spe_state->Q_max[1] = spe_state->Q_max[0];
    spe_state->Q_max[0] = shift;

    /* preemphasis with scaling (L_FRAME+L_FILT) */

    tmp = new_speech[L_FRAME - 1];

    for (i = L_FRAME + L_FILT - 1; i > 0; i--)
    {
        L_tmp = L_mult(new_speech[i], 16384);
        L_tmp = L_msu(L_tmp, new_speech[i - 1], mu);
        L_tmp = L_shl(L_tmp, Q_new);
        new_speech[i] = round16(L_tmp);
    }

    L_tmp = L_mult(new_speech[0], 16384);
    L_tmp = L_msu(L_tmp, spe_state->mem_preemph, mu);
    L_tmp = L_shl(L_tmp, Q_new);
    new_speech[0] = round16(L_tmp);

    spe_state->mem_preemph = tmp;

    /* scale previous samples and memory */

    Scale_sig(old_speech, L_TOTAL - L_FRAME - L_FILT, exp);
    Scale_sig(old_exc, PIT_MAX + L_INTERPOL, exp);

    Scale_sig(spe_state->mem_syn, M, exp);
    Scale_sig(spe_state->mem_decim2, 3, exp);
    Scale_sig(&(spe_state->mem_wsp), 1, exp);
    Scale_sig(&(spe_state->mem_w0), 1, exp);

    /*------------------------------------------------------------------------*
     *  Call VAD                                                              *
     *  Preemphesis scale down signal in low frequency and keep dynamic in HF.*
     *  Vad work slightly in futur (new_speech = speech + L_NEXT - L_FILT).   *
     *------------------------------------------------------------------------*/

    Copy(new_speech, buf, L_FRAME);

    Scale_sig(buf, L_FRAME, sub(1, Q_new));

    vad_flag = wb_vad(spe_state->vadSt, buf);
    if (vad_flag == 0)
    {
        spe_state->vad_hist = add(spe_state->vad_hist, 1);
    } else
    {
        spe_state->vad_hist = 0;
    }

    /* DTX processing */
    if (allow_dtx != 0)
    {
        /* Note that mode may change here */
        tx_dtx_handler(spe_state->dtx_encSt, vad_flag, mode);
        *ser_size = nb_of_bits[*mode];
    }

    if (*mode != MRDTX)
    {
        Parm_serial(vad_flag, 1, &prms);
    }
  /*------------------------------------------------------------------------*
   *  Perform LPC analysis                                                  *
   *  ~~~~~~~~~~~~~~~~~~~~                                                  *
   *   - autocorrelation + lag windowing                                    *
   *   - Levinson-durbin algorithm to find a[]                              *
   *   - convert a[] to isp[]                                               *
   *   - convert isp[] to isf[] for quantization                            *
   *   - quantize and code the isf[]                                        *
   *   - convert isf[] to isp[] for interpolation                           *
   *   - find the interpolated ISPs and convert to a[] for the 4 subframes  *
   *------------------------------------------------------------------------*/

    /* LP analysis centered at 4nd subframe */
    Autocorr(p_window, M, r_h);            /* Autocorrelations */
    Lag_window(r_h);                       /* Lag windowing    */
    Levinson(r_h, A, rc, spe_state->mem_levinson);     /* Levinson Durbin  */
    Az_isp(A, ispnew, spe_state->ispold);         /* From A(z) to ISP */

    /* Find the interpolated ISPs and convert to a[] for all subframes */
    Int_isp(spe_state->ispold, ispnew, A);

    /* update ispold[] for the next frame */
    Copy(ispnew, spe_state->ispold, M);

    /* Convert ISPs to frequency domain 0..6400 */
    Isp_isf(ispnew, isf, M);

    /* check resonance for pitch clipping algorithm */
    Gp_clip_test_isf(isf, spe_state->gp_clip);

    /*----------------------------------------------------------------------*
     *  Perform PITCH_OL analysis                                           *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~                                           *
     * - Find the residual res[] for the whole speech frame                 *
     * - Find the weighted input speech wsp[] for the whole speech frame    *
     * - scale wsp[] to avoid overflow in pitch estimation                  *
     * - Find open loop pitch lag for whole speech frame                    *
     *----------------------------------------------------------------------*/

    p_A = A;
    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {
        Weight_a(p_A, Ap, GAMMA1, M);
        Residu(Ap, M, &speech[i_subfr], &wsp[i_subfr], L_SUBFR);
        p_A += (M + 1);
    }
    Deemph2(wsp, TILT_FAC, L_FRAME, &(spe_state->mem_wsp));

    /* find maximum value on wsp[] for 12 bits scaling */
/*    max = 0;
    for (i = 0; i < L_FRAME; i++)
    {
        tmp = abs_s(wsp[i]);
		if(tmp > max)
			max = tmp;
    }
*/
	max = arrMax(wsp, L_FRAME);

    tmp = spe_state->old_wsp_max;
	if (tmp < max)
		tmp = max;                 /* tmp = max(wsp_max, old_wsp_max) */

    spe_state->old_wsp_max = max;

    shift = norm_s0(tmp);
    shift = shift - 3;

	if (shift > 0)
		shift = 0;                  /* shift = 0..-3 */

    /* decimation of wsp[] to search pitch in LF and to reduce complexity */
    LP_Decim2(wsp, L_FRAME, spe_state->mem_decim2);

    /* scale wsp[] in 12 bits to avoid overflow */
    Scale_sig(wsp, L_FRAME / OPL_DECIM, shift);

    /* scale old_wsp (warning: exp must be Q_new-Q_old) */
    exp = add(exp, sub(shift, spe_state->old_wsp_shift));
    spe_state->old_wsp_shift = shift;
    Scale_sig(old_wsp, PIT_MAX / OPL_DECIM, exp);
    Scale_sig(spe_state->old_hp_wsp, PIT_MAX / OPL_DECIM, exp);
    scale_mem_Hp_wsp(spe_state->hp_wsp_mem, exp);

    /* Find open loop pitch lag for whole speech frame */

    if (*ser_size == NBBITS_7k)
    {
        /* Find open loop pitch lag for whole speech frame */
        T_op = Pitch_med_ol(wsp, PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
                            L_FRAME / OPL_DECIM, spe_state->old_T0_med,
                            &(spe_state->ol_gain), spe_state->hp_wsp_mem,
                            spe_state->old_hp_wsp, spe_state->ol_wght_flg);
    } else
    {
        /* Find open loop pitch lag for first 1/2 frame */
        T_op = Pitch_med_ol(wsp, PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
                            (L_FRAME / 2) / OPL_DECIM, spe_state->old_T0_med,
                            &(spe_state->ol_gain), spe_state->hp_wsp_mem,
                            spe_state->old_hp_wsp, spe_state->ol_wght_flg);
    }

    spe_state->ol_wght_flg = 1;
    if (spe_state->ol_gain > 19661)              /* 0.6 in Q15 */
    {
        spe_state->old_T0_med = Med_olag(T_op, spe_state->old_ol_lag);
        spe_state->ada_w = 32767;
    } else
    {
        spe_state->ada_w = mult(spe_state->ada_w, 29491);
        if (spe_state->ada_w < 26214)
            spe_state->ol_wght_flg = 0;
    }

    wb_vad_tone_detection(spe_state->vadSt, spe_state->ol_gain);

    T_op *= OPL_DECIM;

    if (*ser_size != NBBITS_7k)
    {
        /* Find open loop pitch lag for second 1/2 frame */
        T_op2 = Pitch_med_ol(wsp + ((L_FRAME / 2) / OPL_DECIM),
                             PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
                             (L_FRAME / 2) / OPL_DECIM, spe_state->old_T0_med,
                             &(spe_state->ol_gain), spe_state->hp_wsp_mem,
                             spe_state->old_hp_wsp, spe_state->ol_wght_flg);

        spe_state->ol_wght_flg = 1;
        if (spe_state->ol_gain > 19661)            /* 0.6 in Q15 */
        {
            spe_state->old_T0_med = Med_olag(T_op2, spe_state->old_ol_lag);
            spe_state->ada_w = 32767;
        } else
        {
            spe_state->ada_w = mult(spe_state->ada_w, 29491);
            if (spe_state->ada_w < 26214)
                spe_state->ol_wght_flg = 0;
        }

        wb_vad_tone_detection(spe_state->vadSt, spe_state->ol_gain);

        T_op2 *= OPL_DECIM;

    } else
    {
        T_op2 = T_op;
    }


    /*----------------------------------------------------------------------*
     *                              DTX-CNG                                 *
     *----------------------------------------------------------------------*/

    if (*mode == MRDTX)            /* CNG mode */
    {
        /* Buffer isf's and energy */
        Residu(&A[3 * (M + 1)], M, speech, exc, L_FRAME);

        for (i = 0; i < L_FRAME; i++)
        {
            exc2[i] = shr(exc[i], Q_new);
        }

        L_tmp = Dot_product(exc2, exc2, L_FRAME);
		L_tmp = L_tmp >> 1;

        dtx_buffer(spe_state->dtx_encSt, isf, L_tmp, codec_mode);

        /* Quantize and code the ISFs */
        dtx_enc(spe_state->dtx_encSt, isf, exc2, &prms);

        /* Convert ISFs to the cosine domain */
        Isf_isp(isf, ispnew_q, M);
        Isp_Az(ispnew_q, Aq, M, 0);

        for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
        {
            synthesis_enc(Aq, &exc2[i_subfr], 0, &speech16k[i_subfr * 5 / 4],
                          spe_state);
        }
        Copy(isf, spe_state->isfold, M);


        /* reset speech coder memories */
        Reset_encoder(spe_state, 0);

        /*--------------------------------------------------*
         * Update signal for next frame.                    *
         * -> save past of speech[] and wsp[].              *
         *--------------------------------------------------*/

        Copy(&old_speech[L_FRAME], spe_state->old_speech, L_TOTAL - L_FRAME);
        Copy(&old_wsp[L_FRAME / OPL_DECIM], spe_state->old_wsp, PIT_MAX / OPL_DECIM);

        return;
    }
    /*----------------------------------------------------------------------*
     *                               ACELP                                  *
     *----------------------------------------------------------------------*/

    /* Quantize and code the ISFs */

    if (*ser_size <= NBBITS_7k)
    {
        Qpisf_2s_36b(isf, isf, spe_state->past_isfq, indice, 4);

        Parm_serial(indice[0], 8, &prms);
        Parm_serial(indice[1], 8, &prms);
        Parm_serial(indice[2], 7, &prms);
        Parm_serial(indice[3], 7, &prms);
        Parm_serial(indice[4], 6, &prms);
    } else
    {
        Qpisf_2s_46b(isf, isf, spe_state->past_isfq, indice, 4);

        Parm_serial(indice[0], 8, &prms);
        Parm_serial(indice[1], 8, &prms);
        Parm_serial(indice[2], 6, &prms);
        Parm_serial(indice[3], 7, &prms);
        Parm_serial(indice[4], 7, &prms);
        Parm_serial(indice[5], 5, &prms);
        Parm_serial(indice[6], 5, &prms);
    }

    /* Check stability on isf : distance between old isf and current isf */

    L_tmp = 0;
    for (i = 0; i < M - 1; i++)
    {
        tmp = sub(isf[i], spe_state->isfold[i]);
        L_tmp = L_mac(L_tmp, tmp, tmp);
    }

    L_tmp = L_shl(L_tmp, 8);              /* saturation can occur here */
    tmp = (Word16)(L_tmp >> 16);

    tmp = mult(tmp, 26214);                /* tmp = L_tmp*0.8/256 */
    tmp = sub(20480, tmp);                 /* 1.25 - tmp (in Q14) */

    stab_fac = shl(tmp, 1);                /* saturation can occur here */

    if (stab_fac < 0)
    {
        stab_fac = 0;
    }
    Copy(isf, spe_state->isfold, M);

    /* Convert ISFs to the cosine domain */
    Isf_isp(isf, ispnew_q, M);

    if (spe_state->first_frame != 0)
    {
        spe_state->first_frame = 0;
        Copy(ispnew_q, spe_state->ispold_q, M);
    }
    /* Find the interpolated ISPs and convert to a[] for all subframes */

    Int_isp(spe_state->ispold_q, ispnew_q, Aq);

    /* update ispold[] for the next frame */
    Copy(ispnew_q, spe_state->ispold_q, M);

    p_Aq = Aq;
    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {
        Residu(p_Aq, M, &speech[i_subfr], &exc[i_subfr], L_SUBFR);
        p_Aq += (M + 1);
    }

    /* Buffer isf's and energy for dtx on non-speech frame */

    if (vad_flag == 0)
    {
        for (i = 0; i < L_FRAME; i++)
        {
            exc2[i] = shr(exc[i], Q_new);
        }
        L_tmp = Dot_product(exc2, exc2, L_FRAME);
		L_tmp = L_tmp >> 1;

        dtx_buffer(spe_state->dtx_encSt, isf, L_tmp, codec_mode);
    }
    /* range for closed loop pitch search in 1st subframe */

    T0_min = T_op - 8;
	if (T0_min < PIT_MIN)
		T0_min = PIT_MIN;

    T0_max = T0_min + 15;
    if (T0_max > PIT_MAX)
    {
        T0_max = PIT_MAX;
        T0_min = PIT_MAX - 15;
    }
    /*------------------------------------------------------------------------*
     *          Loop for every subframe in the analysis frame                 *
     *------------------------------------------------------------------------*
     *  To find the pitch and innovation parameters. The subframe size is     *
     *  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.               *
     *     - compute the target signal for pitch search                       *
     *     - compute impulse response of weighted synthesis filter (h1[])     *
     *     - find the closed-loop pitch parameters                            *
     *     - encode the pitch dealy                                           *
     *     - find 2 lt prediction (with / without LP filter for lt pred)      *
     *     - find 2 pitch gains and choose the best lt prediction.            *
     *     - find target vector for codebook search                           *
     *     - update the impulse response h1[] for codebook search             *
     *     - correlation between target vector and impulse response           *
     *     - codebook search and encoding                                     *
     *     - VQ of pitch and codebook gains                                   *
     *     - find voicing factor and tilt of code for next subframe.          *
     *     - update states of weighting filter                                *
     *     - find excitation and synthesis speech                             *
     *------------------------------------------------------------------------*/

    p_A = A;
    p_Aq = Aq;

    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {
        pit_flag = i_subfr;
        if ((i_subfr == 2 * L_SUBFR) && (*ser_size > NBBITS_7k))
        {
            pit_flag = 0;

            /* range for closed loop pitch search in 3rd subframe */

            T0_min = T_op2 - 8;
			if (T0_min < PIT_MIN)
				T0_min = PIT_MIN;
            T0_max = T0_min + 15;
            if (T0_max > PIT_MAX)
            {
                T0_max = PIT_MAX;
                T0_min = PIT_MAX - 15;
            }
        }
   /*-----------------------------------------------------------------------*
    *                                                                       *
    *        Find the target vector for pitch search:                       *
    *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
    *                                                                       *
    *             |------|  res[n]                                          *
    * speech[n]---| A(z) |--------                                          *
    *             |------|       |   |--------| error[n]  |------|          *
    *                   zero -- (-)--| 1/A(z) |-----------| W(z) |-- target *
    *                   exc          |--------|           |------|          *
    *                                                                       *
    * Instead of subtracting the zero-input response of filters from        *
    * the weighted input speech, the above configuration is used to         *
    * compute the target vector.                                            *
    *                                                                       *
    *-----------------------------------------------------------------------*/

        for (i = 0; i < M; i++)
        {
            error[i] = sub(speech[i + i_subfr - M], spe_state->mem_syn[i]);
        }
        Residu(p_Aq, M, &speech[i_subfr], &exc[i_subfr], L_SUBFR);

        Syn_filt(p_Aq, M, &exc[i_subfr], error + M, L_SUBFR, error, 0);

        Weight_a(p_A, Ap, GAMMA1, M);
        Residu(Ap, M, error + M, xn, L_SUBFR);

        Deemph2(xn, TILT_FAC, L_SUBFR, &(spe_state->mem_w0));

   /*----------------------------------------------------------------------*
    * Find approx. target in residual domain "cn[]" for inovation search.  *
    *----------------------------------------------------------------------*/

        /* first half: xn[] --> cn[] */
        Set_zero(code, M);
        Copy(xn, code + M, L_SUBFR / 2);
        Preemph2(code + M, TILT_FAC, L_SUBFR / 2);
        Weight_a(p_A, Ap, GAMMA1, M);
        Syn_filt(Ap, M, code + M, code + M, L_SUBFR / 2, code, 0);
        Residu(p_Aq, M, code + M, cn, L_SUBFR / 2);

        /* second half: res[] --> cn[] (approximated and faster) */
        Copy(&exc[i_subfr + (L_SUBFR / 2)], cn + (L_SUBFR / 2), L_SUBFR / 2);

        /*---------------------------------------------------------------*
         * Compute impulse response, h1[], of weighted synthesis filter  *
         *---------------------------------------------------------------*/

        Set_zero(error, M + L_SUBFR);
        Weight_a(p_A, error + M, GAMMA1, M);

        for (i = 0; i < L_SUBFR; i++)
        {
            L_tmp = L_mult(error[i + M], 16384);        /* x4 (Q12 to Q14) */
            for (j = 1; j <= M; j++)
                L_tmp = L_msu(L_tmp, p_Aq[j], error[i + M - j]);

            h1[i] = error[i + M] = round16(L_shl(L_tmp, 3));
        }
        /* deemph without division by 2 -> Q14 to Q15 */
        tmp = 0;
        Deemph2(h1, TILT_FAC, L_SUBFR, &tmp);   /* h1 in Q14 */

        /* h2 in Q12 for codebook search */
        Copy(h1, h2, L_SUBFR);
        Scale_sig(h2, L_SUBFR, -2);

        /*---------------------------------------------------------------*
         * scale xn[] and h1[] to avoid overflow in dot_product12()      *
         *---------------------------------------------------------------*/

        Scale_sig(xn, L_SUBFR, shift);        /* scaling of xn[] to limit dynamic at 12 bits */
        Scale_sig(h1, L_SUBFR, (shift + 1));  /* set h1[] in Q15 with scaling for convolution */

   /*----------------------------------------------------------------------*
    *                 Closed-loop fractional pitch search                  *
    *----------------------------------------------------------------------*/

        /* find closed loop fractional pitch  lag */

        if (*ser_size <= NBBITS_9k)
        {
            T0 = Pitch_fr4(&exc[i_subfr], xn, h1, T0_min, T0_max, &T0_frac,
                pit_flag, PIT_MIN, PIT_FR1_8b, L_SUBFR);

            /* encode pitch lag */

            if (pit_flag == 0)             /* if 1st/3rd subframe */
            {
           /*--------------------------------------------------------------*
            * The pitch range for the 1st/3rd subframe is encoded with     *
            * 8 bits and is divided as follows:                            *
            *   PIT_MIN to PIT_FR1-1  resolution 1/2 (frac = 0 or 2)       *
            *   PIT_FR1 to PIT_MAX    resolution 1   (frac = 0)            *
            *--------------------------------------------------------------*/

                if (T0 < PIT_FR1_8b)
                {
                    index = (T0 - PIT_MIN) * 2 + (T0_frac >> 1);
                } else
                {
                    index = (T0 + PIT_FR1_8b) - PIT_MIN * 2;
                }

                Parm_serial(index, 8, &prms);

                /* find T0_min and T0_max for subframe 2 and 4 */

                T0_min = T0 - 8;
				if (T0_min < PIT_MIN)
					T0_min = PIT_MIN;
                T0_max = T0_min + 15;
                if (T0_max > PIT_MAX)
                {
                    T0_max = PIT_MAX;
                    T0_min = PIT_MAX - 15;
                }
            } else
            {                              /* if subframe 2 or 4 */
           /*--------------------------------------------------------------*
            * The pitch range for subframe 2 or 4 is encoded with 5 bits:  *
            *   T0_min  to T0_max     resolution 1/2 (frac = 0 or 2)       *
            *--------------------------------------------------------------*/

                index = (T0 - T0_min) * 2 + (T0_frac >> 1);

                Parm_serial(index, 5, &prms);
            }
        } else
        {
            T0 = Pitch_fr4(&exc[i_subfr], xn, h1, T0_min, T0_max, &T0_frac,
                pit_flag, PIT_FR2, PIT_FR1_9b, L_SUBFR);

            /* encode pitch lag */

            if (pit_flag == 0)             /* if 1st/3rd subframe */
            {
           /*--------------------------------------------------------------*
            * The pitch range for the 1st/3rd subframe is encoded with     *
            * 9 bits and is divided as follows:                            *
            *   PIT_MIN to PIT_FR2-1  resolution 1/4 (frac = 0,1,2 or 3)   *
            *   PIT_FR2 to PIT_FR1-1  resolution 1/2 (frac = 0 or 1)       *
            *   PIT_FR1 to PIT_MAX    resolution 1   (frac = 0)            *
            *--------------------------------------------------------------*/

                if (T0 < PIT_FR2)
                {
                    index = ((T0 - PIT_MIN) << 2) + T0_frac;
                } else if (T0 < PIT_FR1_9b)
                {
                    index = (T0 + PIT_FR2) * 2 + (T0_frac >> 1) - PIT_MIN * 4;
                } else
                {
                    index = (T0 + PIT_FR1_9b) + PIT_FR2 * 2 - PIT_MIN * 4;
                }

                Parm_serial(index, 9, &prms);
                /* find T0_min and T0_max for subframe 2 and 4 */

                T0_min = T0 - 8;
				if (T0_min < PIT_MIN)
					T0_min = PIT_MIN;
                T0_max = T0_min + 15;
                if (T0_max > PIT_MAX)
                {
                    T0_max = PIT_MAX;
                    T0_min = PIT_MAX - 15;
                }
            } else
            {                              /* if subframe 2 or 4 */
           /*--------------------------------------------------------------*
            * The pitch range for subframe 2 or 4 is encoded with 6 bits:  *
            *   T0_min  to T0_max     resolution 1/4 (frac = 0,1,2 or 3)   *
            *--------------------------------------------------------------*/

                index = ((T0 - T0_min) << 2) + T0_frac;

                Parm_serial(index, 6, &prms);
            }
        }

        /*-----------------------------------------------------------------*
         * Gain clipping test to avoid unstable synthesis on frame erasure *
         *-----------------------------------------------------------------*/

        clip_gain = Gp_clip(spe_state->gp_clip);

        /*-----------------------------------------------------------------*
         * - find unity gain pitch excitation (adaptive codebook entry)    *
         *   with fractional interpolation.                                *
         * - find filtered pitch exc. y1[]=exc[] convolved with h1[])      *
         * - compute pitch gain1                                           *
         *-----------------------------------------------------------------*/

        /* find pitch exitation */

        Pred_lt4(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
        if (*ser_size > NBBITS_9k)
        {
            Convolve(&exc[i_subfr], h1, y1, L_SUBFR);
            gain1 = G_pitch(xn, y1, g_coeff, L_SUBFR);

            /* clip gain if necessary to avoid problem at decoder */
            if ((clip_gain != 0) && (gain1 > GP_CLIP))
            {
                gain1 = GP_CLIP;
            }
            /* find energy of new target xn2[] */
            Updt_tar(xn, dn, y1, gain1, L_SUBFR);       /* dn used temporary */
        } else
        {
            gain1 = 0;
        }

        /*-----------------------------------------------------------------*
         * - find pitch excitation filtered by 1st order LP filter.        *
         * - find filtered pitch exc. y2[]=exc[] convolved with h1[])      *
         * - compute pitch gain2                                           *
         *-----------------------------------------------------------------*/

        /* find pitch excitation with lp filter */
        for (i = 0; i < L_SUBFR; i++)
        {
            L_tmp = L_mult(20972, exc[i + i_subfr]);
            L_tmp = L_mac(L_tmp, 5898, exc[i - 1 + i_subfr]);
            L_tmp = L_mac(L_tmp, 5898, exc[i + 1 + i_subfr]);
            code[i] = round16(L_tmp);
        }

        Convolve(code, h1, y2, L_SUBFR);
        gain2 = G_pitch(xn, y2, g_coeff2, L_SUBFR);

        /* clip gain if necessary to avoid problem at decoder */
        if ((clip_gain != 0) && (gain2 > GP_CLIP))
        {
            gain2 = GP_CLIP;
        }
        /* find energy of new target xn2[] */
        Updt_tar(xn, xn2, y2, gain2, L_SUBFR);

        /*-----------------------------------------------------------------*
         * use the best prediction (minimise quadratic error).             *
         *-----------------------------------------------------------------*/

        select = 0;
        if (*ser_size > NBBITS_9k)
        {
            L_tmp = 0L;
            for (i = 0; i < L_SUBFR; i++)
            {
                L_tmp = L_mac(L_tmp, dn[i], dn[i]);
                L_tmp = L_msu(L_tmp, xn2[i], xn2[i]);
            }
            if (L_tmp <= 0)
            {
                select = 1;
            }
            Parm_serial(select, 1, &prms);
        }
        if (select == 0)
        {
            /* use the lp filter for pitch excitation prediction */
            gain_pit = gain2;
            Copy(code, &exc[i_subfr], L_SUBFR);
            Copy(y2, y1, L_SUBFR);
            Copy(g_coeff2, g_coeff, 4);
        } else
        {
            /* no filter used for pitch excitation prediction */
            gain_pit = gain1;
            Copy(dn, xn2, L_SUBFR); /* target vector for codebook search */
        }

        /*-----------------------------------------------------------------*
         * - update cn[] for codebook search                               *
         *-----------------------------------------------------------------*/

        Updt_tar(cn, cn, &exc[i_subfr], gain_pit, L_SUBFR);

        Scale_sig(cn, L_SUBFR, shift);     /* scaling of cn[] to limit dynamic at 12 bits */

        /*-----------------------------------------------------------------*
         * - include fixed-gain pitch contribution into impulse resp. h1[] *
         *-----------------------------------------------------------------*/

        Preemph(h2, spe_state->tilt_code, L_SUBFR);

        if (T0_frac > 2)
            T0++;
        Pit_shrp(h2, T0, PIT_SHARP, L_SUBFR);

        /*-----------------------------------------------------------------*
         * - Correlation between target xn2[] and impulse response h1[]    *
         * - Innovative codebook search                                    *
         *-----------------------------------------------------------------*/

        cor_h_x(h2, xn2, dn);

        if (*ser_size <= NBBITS_7k)
        {
            ACELP_2t64_fx(dn, cn, h2, code, y2, indice);

            Parm_serial(indice[0], 12, &prms);
        } else if (*ser_size <= NBBITS_9k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 20, *ser_size, indice);

            Parm_serial(indice[0], 5, &prms);
            Parm_serial(indice[1], 5, &prms);
            Parm_serial(indice[2], 5, &prms);
            Parm_serial(indice[3], 5, &prms);
        } else if (*ser_size <= NBBITS_12k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 36, *ser_size, indice);

            Parm_serial(indice[0], 9, &prms);
            Parm_serial(indice[1], 9, &prms);
            Parm_serial(indice[2], 9, &prms);
            Parm_serial(indice[3], 9, &prms);
        } else if (*ser_size <= NBBITS_14k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 44, *ser_size, indice);

            Parm_serial(indice[0], 13, &prms);
            Parm_serial(indice[1], 13, &prms);
            Parm_serial(indice[2], 9, &prms);
            Parm_serial(indice[3], 9, &prms);
        } else if (*ser_size <= NBBITS_16k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 52, *ser_size, indice);

            Parm_serial(indice[0], 13, &prms);
            Parm_serial(indice[1], 13, &prms);
            Parm_serial(indice[2], 13, &prms);
            Parm_serial(indice[3], 13, &prms);
        } else if (*ser_size <= NBBITS_18k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 64, *ser_size, indice);

            Parm_serial(indice[0], 2, &prms);
            Parm_serial(indice[1], 2, &prms);
            Parm_serial(indice[2], 2, &prms);
            Parm_serial(indice[3], 2, &prms);
            Parm_serial(indice[4], 14, &prms);
            Parm_serial(indice[5], 14, &prms);
            Parm_serial(indice[6], 14, &prms);
            Parm_serial(indice[7], 14, &prms);
        } else if (*ser_size <= NBBITS_20k)
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 72, *ser_size, indice);

            Parm_serial(indice[0], 10, &prms);
            Parm_serial(indice[1], 10, &prms);
            Parm_serial(indice[2], 2, &prms);
            Parm_serial(indice[3], 2, &prms);
            Parm_serial(indice[4], 10, &prms);
            Parm_serial(indice[5], 10, &prms);
            Parm_serial(indice[6], 14, &prms);
            Parm_serial(indice[7], 14, &prms);
        } else
        {
            ACELP_4t64_fx(dn, cn, h2, code, y2, 88, *ser_size, indice);

            Parm_serial(indice[0], 11, &prms);
            Parm_serial(indice[1], 11, &prms);
            Parm_serial(indice[2], 11, &prms);
            Parm_serial(indice[3], 11, &prms);
            Parm_serial(indice[4], 11, &prms);
            Parm_serial(indice[5], 11, &prms);
            Parm_serial(indice[6], 11, &prms);
            Parm_serial(indice[7], 11, &prms);
        }

        /*-------------------------------------------------------*
         * - Add the fixed-gain pitch contribution to code[].    *
         *-------------------------------------------------------*/
        Preemph(code, spe_state->tilt_code, L_SUBFR);

        Pit_shrp(code, T0, PIT_SHARP, L_SUBFR);

        /*----------------------------------------------------------*
         *  - Compute the fixed codebook gain                       *
         *  - quantize fixed codebook gain                          *
         *----------------------------------------------------------*/

        if (*ser_size <= NBBITS_9k)
        {
            tmp = 6;
        } else
        {
            tmp = 7;
        }
        index = Q_gain2(xn, y1, add(Q_new, shift), y2, code, g_coeff, L_SUBFR,
                tmp, &gain_pit, &L_gain_code, clip_gain, spe_state->qua_gain);
        Parm_serial(index, tmp, &prms);

        /* test quantized gain of pitch for pitch clipping algorithm */
        Gp_clip_test_gain_pit(gain_pit, spe_state->gp_clip);

        L_tmp = L_shl(L_gain_code, Q_new); /* saturation can occur here */
        gain_code = round16(L_tmp);          /* scaled gain_code with Qnew */

        /*----------------------------------------------------------*
         * Update parameters for the next subframe.                 *
         * - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
         *----------------------------------------------------------*/

        /* find voice factor in Q15 (1=voiced, -1=unvoiced) */

        Copy(&exc[i_subfr], exc2, L_SUBFR);
        Scale_sig(exc2, L_SUBFR, shift);

        voice_fac = voice_factor(exc2, shift, gain_pit, code, gain_code, L_SUBFR);

        /* tilt of code for next subframe: 0.5=voiced, 0=unvoiced */
        spe_state->tilt_code = add((voice_fac >> 2), 8192);

        /*------------------------------------------------------*
         * - Update filter's memory "mem_w0" for finding the    *
         *   target vector in the next subframe.                *
         * - Find the total excitation                          *
         * - Find synthesis speech to update mem_syn[].         *
         *------------------------------------------------------*/

        /* y2 in Q9, gain_pit in Q14 */
        L_tmp = L_mult(gain_code, y2[L_SUBFR - 1]);
        L_tmp = L_shl(L_tmp, add(5, shift));
        L_tmp = L_negate(L_tmp);
        L_tmp = L_mac(L_tmp, xn[L_SUBFR - 1], 16384);
        L_tmp = L_msu(L_tmp, y1[L_SUBFR - 1], gain_pit);
        L_tmp = L_shl(L_tmp, sub(1, shift));
        spe_state->mem_w0 = round16(L_tmp);

        if (*ser_size >= NBBITS_24k)
            Copy(&exc[i_subfr], exc2, L_SUBFR);

        for (i = 0; i < L_SUBFR; i++)
        {
            /* code in Q9, gain_pit in Q14 */
            L_tmp = L_mult(gain_code, code[i]);
            L_tmp = L_shl(L_tmp, 5);
            L_tmp = L_mac(L_tmp, exc[i + i_subfr], gain_pit);
            L_tmp = L_shl(L_tmp, 1);       /* saturation can occur here */
            exc[i + i_subfr] = round16(L_tmp);
        }

        Syn_filt(p_Aq, M, &exc[i_subfr], synth, L_SUBFR, spe_state->mem_syn, 1);

        if (*ser_size >= NBBITS_24k)
        {
            /*------------------------------------------------------------*
             * phase dispersion to enhance noise in low bit rate          *
             *------------------------------------------------------------*/

            /*------------------------------------------------------------*
             * noise enhancer                                             *
             * ~~~~~~~~~~~~~~                                             *
             * - Enhance excitation on noise. (modify gain of code)       *
             *   If signal is noisy and LPC filter is stable, move gain   *
             *   of code 1.5 dB toward gain of code threshold.            *
             *   This decrease by 3 dB noise energy variation.            *
             *------------------------------------------------------------*/

            tmp = sub(16384, (voice_fac >> 1));    /* 1=unvoiced, 0=voiced */
            fac = mult(stab_fac, tmp);

            if (L_gain_code < spe_state->L_gc_thres)
            {
                L_tmp = Mpy_32x16(L_gain_code, 6226);
                L_tmp = L_add(L_tmp, L_gain_code);

				if (L_tmp < spe_state->L_gc_thres)
					spe_state->L_gc_thres = L_tmp;
            } else
            {
                L_tmp = Mpy_32x16(L_gain_code, 27536);

				if (L_tmp > spe_state->L_gc_thres)
					spe_state->L_gc_thres = L_tmp;
            }

            L_gain_code = Mpy_32x16(L_gain_code, (32767 - fac));
            L_tmp = Mpy_32x16(spe_state->L_gc_thres, fac);
            L_gain_code = L_add(L_gain_code, L_tmp);

            /*------------------------------------------------------------*
             * pitch enhancer                                             *
             * ~~~~~~~~~~~~~~                                             *
             * - Enhance excitation on voice. (HP filtering of code)      *
             *   On voiced signal, filtering of code by a smooth fir HP   *
             *   filter to decrease energy of code in low frequency.      *
             *------------------------------------------------------------*/

            tmp = add((voice_fac >> 3), 4096); /* 0.25=voiced, 0=unvoiced */

            L_tmp = (Word32)code[0] << 16;
            for (i = 0; i < L_SUBFR - 1;)
            {
                L_tmp = L_msu(L_tmp, code[i + 1], tmp);
                code2[i] = round16(L_tmp);
                i++;
                L_tmp = (Word32)code[i] << 16;
                L_tmp = L_msu(L_tmp, code[i - 1], tmp);
            }
            code2[i] = round16(L_tmp);

            /* build excitation */

            gain_code = round16(L_shl(L_gain_code, Q_new));

            for (i = 0; i < L_SUBFR; i++)
            {
                L_tmp = L_mult(code2[i], gain_code);
                L_tmp = L_shl(L_tmp, 5);
                L_tmp = L_mac(L_tmp, exc2[i], gain_pit);
                L_tmp = L_shl(L_tmp, 1);   /* saturation can occur here */
                exc2[i] = round16(L_tmp);
            }

            corr_gain = synthesis_enc(p_Aq, exc2, Q_new, &speech16k[i_subfr * 5 / 4], spe_state);
            Parm_serial(corr_gain, 4, &prms);
        }
        p_A += (M + 1);
        p_Aq += (M + 1);

    }                                      /* end of subframe loop */

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> save past of speech[], wsp[] and exc[].       *
     *--------------------------------------------------*/

    Copy(&old_speech[L_FRAME], spe_state->old_speech, L_TOTAL - L_FRAME);
    Copy(&old_wsp[L_FRAME / OPL_DECIM], spe_state->old_wsp, PIT_MAX / OPL_DECIM);
    Copy(&old_exc[L_FRAME], spe_state->old_exc, PIT_MAX + L_INTERPOL);

    /* perform homing if homing frame was detected at encoder input */
    if (reset_flag != 0)
        Reset_encoder(spe_state, 1);

    return;
}
