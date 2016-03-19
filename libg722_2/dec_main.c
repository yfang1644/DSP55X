/*------------------------------------------------------------------------*
 *                         DEC_MAIN.C                                     *
 *------------------------------------------------------------------------*
 * Performs the main decoder routine                                      *
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
#include "cnst.h"
#include "acelp.h"
#include "bits.h"
#include "math_op.h"
#include "dec_main.h"

/*-----------------------------------------------------*
 * Function synthesis()                                *
 *                                                     *
 * Synthesis of signal at 16kHz with HF extension.     *
 *                                                     *
 *-----------------------------------------------------*/

void synthesis_dec(
     Word16 Aq[],              /* A(z)  : quantized Az               */
     Word16 exc[],             /* (i)   : excitation at 12kHz        */
     Word16 Q_new,             /* (i)   : scaling performed on exc   */
     Word16 synth16k[],        /* (o)   : 16kHz synthesis signal     */
     Word16 prms,              /* (i)   : parameter                  */
     Word16 HfIsf[],
     Word16 nb_bits,
     Word16 newDTXState,
     Decoder_State * st,       /* (i/o) : State structure            */
     Word16 bfi                /* (i)   : bad frame indicator        */
)
{
    Word16 i, fac, tmp, exp;
    Word16 ener, exp_ener;
    Word32 L_tmp;

    Word16 synth_hi[M + L_SUBFR], synth_lo[M + L_SUBFR];
    Word16 synth[L_SUBFR];
    Word16 HF[L_SUBFR16k];                 /* High Frequency vector      */
    Word16 Ap[M16k + 1];
    Word16 HfA[M16k + 1];
    Word16 HF_corr_gain;
    Word16 HF_gain_ind;
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

    Oversamp_16k(synth, L_SUBFR, synth16k, st->mem_oversamp);

    /*------------------------------------------------------*
    * HF noise synthesis                                   *
    * ~~~~~~~~~~~~~~~~~~                                   *
    * - Generate HF noise between 5.5 and 7.5 kHz.         *
    * - Set energy of noise according to synthesis tilt.   *
    *     tilt > 0.8 ==> - 14 dB (voiced)                  *
    *     tilt   0.5 ==> - 6 dB  (voiced or noise)         *
    *     tilt < 0.0 ==>   0 dB  (noise)                   *
    *------------------------------------------------------*/

    /* generate white noise vector */
    for (i = 0; i < L_SUBFR16k; i++)
    {
        HF[i] = Random(&(st->seed2)) >> 3;
    }
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
        tmp >>= 1;                       /* Be sure tmp < ener */
        exp++;
    }
    tmp = div_s(tmp, ener);
    L_tmp = (Word32)tmp << 16;           /* result is normalized */
    exp = sub(exp, exp_ener);
    L_tmp = Isqrt_n(L_tmp, &exp);
    L_tmp = L_shl(L_tmp, sub(1, exp));   /* L_tmp x 2, L_tmp in Q31 */
    tmp = (Word16)(L_tmp >> 16);         /* tmp = 2 x sqrt(ener_exc/ener_hf) */
//    for (i = 0; i < L_SUBFR16k; i++)
//    {
//        HF[i] = mult(HF[i], tmp);
//    }
	arrMpy(HF, tmp, L_SUBFR16k);
    /* find tilt of synthesis speech (tilt: 1=voiced, -1=unvoiced) */

    HP400_12k8(synth, L_SUBFR, st->mem_hp400);

    L_tmp = Dot_product12(synth, synth, L_SUBFR, &exp);
    ener = (Word16)(L_tmp >> 16);   /* ener = r[0] */

    L_tmp = Dot_product(synth, &synth[1], L_SUBFR-1);
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
        tmp = gain2;
    } else
    {
        tmp = gain1;
    }

    if (tmp < 3277)
    {
        tmp = 3277;                        /* 0.1 in Q15 */
    }
    if ((nb_bits >= NBBITS_24k) && (bfi == 0))
    {
        /* HF correction gain */
        HF_gain_ind = prms;
        HF_corr_gain = HP_gain[HF_gain_ind];

        /* HF gain */
        for (i = 0; i < L_SUBFR16k; i++)
        {
            HF[i] = shl(mult(HF[i], HF_corr_gain), 1);
        }
    } else
    {
		arrMpy(HF, tmp, L_SUBFR16k);
//        for (i = 0; i < L_SUBFR16k; i++)
//        {
//            HF[i] = mult(HF[i], tmp);
//        }
    }

    if ((nb_bits <= NBBITS_7k) && (newDTXState == SPEECH))
    {
        Isf_Extrapolation(HfIsf);
        Isp_Az(HfIsf, HfA, M16k, 0);

        Weight_a(HfA, Ap, 29491, M16k);    /* fac=0.9 */
        Syn_filt(Ap, M16k, HF, HF, L_SUBFR16k, st->mem_syn_hf, 1);
    } else
    {
        /* synthesis of noise: 4.8kHz..5.6kHz --> 6kHz..7kHz */
        Weight_a(Aq, Ap, 19661, M);        /* fac=0.6 */
        Syn_filt(Ap, M, HF, HF, L_SUBFR16k, st->mem_syn_hf + (M16k - M), 1);
    }

    /* noise High Pass filtering (1ms of delay) */
    Filt_6k_7k(HF, L_SUBFR16k, st->mem_hf);

    if (nb_bits >= NBBITS_24k)
    {
        /* Low Pass filtering (7 kHz) */
        Filt_7k(HF, L_SUBFR16k, st->mem_hf3);
    }
    /* add filtered HF noise to speech synthesis */
    for (i = 0; i < L_SUBFR16k; i++)
    {
        synth16k[i] = add(synth16k[i], HF[i]);
    }

    return;
}

void Reset_decoder(Decoder_State *dec_state, Word16 reset_all)
{
    Word16 i;

    Set_zero(dec_state->old_exc, PIT_MAX + L_INTERPOL);
    Set_zero(dec_state->past_isfq, M);

    dec_state->old_T0_frac = 0;              /* old pitch value = 64.0 */
    dec_state->old_T0 = 64;
    dec_state->first_frame = 1;
    dec_state->L_gc_thres = 0;
    dec_state->tilt_code = 0;

    Set_zero(dec_state->disp_mem, 8); //Init_Phase_dispersion

    /* scaling memories for excitation */
    dec_state->Q_old = Q_MAX;
    dec_state->Qsubfr[3] = Q_MAX;
    dec_state->Qsubfr[2] = Q_MAX;
    dec_state->Qsubfr[1] = Q_MAX;
    dec_state->Qsubfr[0] = Q_MAX;

    if (reset_all != 0)
    {
        /* routines initialization */

        Init_D_gain2(dec_state->dec_gain);
        Set_zero(dec_state->mem_oversamp, 24);	//Init_Oversamp_16k
        Set_zero(dec_state->mem_sig_out, 6);	//Init_HP50_12k8
        Set_zero(dec_state->mem_hf, 30);		//Init_Filt_6k_7k
        Set_zero(dec_state->mem_hf3, 30);		//Init_Filt_7k
        Set_zero(dec_state->mem_hp400, 6);		//Init_HP400_12k8
        Init_Lagconc(dec_state->lag_hist);

        /* isp initialization */

        Copy(isp_init, dec_state->ispold, M);
        Copy(isf_init, dec_state->isfold, M);
        for (i = 0; i < L_MEANBUF; i++)
            Copy(isf_init, &dec_state->isf_buf[i * M], M);
        /* variable initialization */

        dec_state->mem_deemph = 0;

        dec_state->seed = 21845;             /* init random with 21845 */
        dec_state->seed2 = 21845;
        dec_state->seed3 = 21845;

        dec_state->state = 0;
        dec_state->prev_bfi = 0;

        /* Static vectors to zero */

        Set_zero(dec_state->mem_syn_hf, M16k);
        Set_zero(dec_state->mem_syn_hi, M);
        Set_zero(dec_state->mem_syn_lo, M);

        dtx_dec_reset(dec_state->dtx_decSt, isf_init);
        dec_state->vad_hist = 0;

    }
    return;
}

/*-----------------------------------------------------------------*
 *   Funtion  init_decoder	                                       *
 *            ~~~~~~~~~~~~                                         *
 *   ->Initialization of variables for the decoder section.        *
 *-----------------------------------------------------------------*/

void Init_decoder(Decoder_State *spd_state)
{
    dtx_dec_init(&(spd_state->dtx_decSt), isf_init);

    Reset_decoder(spd_state, 1);

    return;
}

/*-----------------------------------------------------------------*
 *   Funtion decoder		                                       *
 *           ~~~~~~~	                                           *
 *   ->Main decoder routine.                                       *
 *                                                                 *
 *-----------------------------------------------------------------*/

void decoder(
     Word16 mode,              /* input : used mode                     */
     Word16 prms[],            /* input : parameter vector              */
     Word16 synth16k[],        /* output: synthesis speech              */
     Decoder_State *spd_state, /* i/o   : State structure               */
     Word16 frame_type         /* input : received frame type           */
)
{
    /* Excitation vector */
    Word16 old_exc[(L_FRAME + 1) + PIT_MAX + L_INTERPOL];
    Word16 *exc;

    /* LPC coefficients */

    Word16 *p_Aq;                  /* ptr to A(z) for the 4 subframes      */
    Word16 Aq[NB_SUBFR * (M + 1)]; /* A(z)   quantized for the 4 subframes */
    Word16 ispnew[M];              /* immittance spectral pairs at 4nd sfr */
    Word16 isf[M];                 /* ISF (frequency domain) at 4nd sfr    */
    Word16 code[L_SUBFR];          /* algebraic codevector                 */
    Word16 code2[L_SUBFR];         /* algebraic codevector                 */
    Word16 exc2[L_FRAME];          /* excitation vector                    */

    Word16 fac, stab_fac, voice_fac, Q_new = 0;
    Word32 L_tmp, L_gain_code;

    /* Scalars */

    Word16 i, j, i_subfr, index, ind[8], max, tmp;
    Word16 T0, T0_frac, T0_max, select, T0_min = 0;
    Word16 gain_pit, gain_code;
    Word16 newDTXState, bfi, unusable_frame, nb_bits;
    Word16 vad_flag;
    Word16 pit_sharp;
    Word16 excp[L_SUBFR];
    Word16 isf_tmp[M];
    Word16 HfIsf[M16k];

    static Word16 mode0 = 0;
    static Word16 reset_flag = 0;
    static Word16 reset_flag_old = 1;

    if ((frame_type == RX_NO_DATA) || (frame_type == RX_SPEECH_LOST))
    {
        bfi = 1;
        unusable_frame = 1;
        reset_flag = 0;
    }
    else
    {
        unusable_frame = 0;
        if ((frame_type == RX_SPEECH_BAD) ||
              (frame_type == RX_SPEECH_PROBABLY_DEGRADED))
        {
            bfi = 1;
        }
        else
        {
            bfi = 0;
        }
        mode0 = mode;

        /* if homed: check if this frame is another homing frame */
        if (reset_flag_old == 1)
        { /* only check until end of first subframe */
          //    reset_flag = decoder_homing_frame_test_first(prms, mode);
            reset_flag = dhf_test(prms, mode, prmnofsf[mode]);
        }
    }

    /* produce encoder homing frame if homed & input=decoder homing frame */
    if ((reset_flag != 0) && (reset_flag_old != 0))
    {
        for (i = 0; i < L_FRAME16k; i++)
        {
            synth16k[i] = EHF_MASK;
        }
        if(reset_flag != 0)
	        Reset_decoder(spd_state, 1);

		reset_flag_old = reset_flag;

        return;
    }

    /* mode verification */
    nb_bits = nb_of_bits[mode];

    /* find the new  DTX state  SPEECH OR DTX */
    newDTXState = rx_dtx_handler(spd_state->dtx_decSt, frame_type);

    if (newDTXState != SPEECH)
    {
        dtx_dec(spd_state->dtx_decSt, exc2, newDTXState, isf, &prms);
    }

    if (bfi != 0)
    {
        spd_state->state++;
        if (spd_state->state > 6)
        {
            spd_state->state = 6;
        }
    } else
    {
        spd_state->state = spd_state->state >> 1;
    }

    /* If this frame is the first speech frame after CNI period,     */
    /* set the BFH state machine to an appropriate state depending   */
    /* on whether there was DTX muting before start of speech or not */
    /* If there was DTX muting, the first speech frame is muted.     */
    /* If there was no DTX muting, the first speech frame is not     */
    /* muted. The BFH state machine starts from state 5, however, to */
    /* keep the audible noise resulting from a SID frame which is    */
    /* erroneously interpreted as a good speech frame as small as    */
    /* possible (the decoder output in this case is quickly muted)   */
    if (spd_state->dtx_decSt->dtxGlobalState == DTX)
    {
        spd_state->state = 5;
        spd_state->prev_bfi = 0;
    } else if (spd_state->dtx_decSt->dtxGlobalState == DTX_MUTE)
    {
        spd_state->state = 5;
        spd_state->prev_bfi = 1;
    }

    if (newDTXState == SPEECH)
    {
        vad_flag = Serial_parm(1, &prms);
        if (bfi == 0)
        {
            if (vad_flag == 0)
            {
                spd_state->vad_hist = add(spd_state->vad_hist, 1);
            } else
            {
                spd_state->vad_hist = 0;
            }
        }
    }
    /*----------------------------------------------------------------------*
     *                              DTX-CNG                                 *
     *----------------------------------------------------------------------*/
    else                                                        /* CNG mode */
    {
        /* increase slightly energy of noise below 200 Hz */

        /* Convert ISFs to the cosine domain */
        Isf_isp(isf, ispnew, M);

        Isp_Az(ispnew, Aq, M, 1);

        Copy(spd_state->isfold, isf_tmp, M);

        for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
        {
            j = i_subfr >> 6;
            for (i = 0; i < M; i++)
            {
                L_tmp = L_mult(isf_tmp[i], interpol_back[j]);
                L_tmp = L_mac(L_tmp, isf[i], interpol_frac[j]);
                HfIsf[i] = round16(L_tmp);
            }
            synthesis_dec(Aq, &exc2[i_subfr], 0, &synth16k[i_subfr * 5 / 4], (short) 1, HfIsf, nb_bits, newDTXState, spd_state, bfi);
        }

        /* reset speech coder memories */
        Reset_decoder(spd_state, 0);

        Copy(isf, spd_state->isfold, M);

        spd_state->prev_bfi = bfi;
        spd_state->dtx_decSt->dtxGlobalState = newDTXState;

        return;
    }
    /*----------------------------------------------------------------------*
     *                               ACELP                                  *
     *----------------------------------------------------------------------*/

    /* copy coder memory state into working space (internal memory for DSP) */

    Copy(spd_state->old_exc, old_exc, PIT_MAX + L_INTERPOL);
    exc = old_exc + PIT_MAX + L_INTERPOL;

    /* Decode the ISFs */
    ind[0] = Serial_parm(8, &prms);
    ind[1] = Serial_parm(8, &prms);
    if (nb_bits <= NBBITS_7k)
    {
        ind[2] = Serial_parm(7, &prms);
        ind[3] = Serial_parm(7, &prms);
        ind[4] = Serial_parm(6, &prms);

        Dpisf_2s_36b(ind, isf, spd_state->past_isfq, spd_state->isfold, spd_state->isf_buf, bfi, 1);
    } else
    {
        ind[2] = Serial_parm(6, &prms);
        ind[3] = Serial_parm(7, &prms);
        ind[4] = Serial_parm(7, &prms);
        ind[5] = Serial_parm(5, &prms);
        ind[6] = Serial_parm(5, &prms);

        Dpisf_2s_46b(ind, isf, spd_state->past_isfq, spd_state->isfold, spd_state->isf_buf, bfi, 1);
    }

    /* Convert ISFs to the cosine domain */

    Isf_isp(isf, ispnew, M);
    if (spd_state->first_frame != 0)
    {
        spd_state->first_frame = 0;
        Copy(ispnew, spd_state->ispold, M);
    }
    /* Find the interpolated ISPs and convert to a[] for all subframes */
    Int_isp(spd_state->ispold, ispnew, Aq);

    /* update ispold[] for the next frame */
    Copy(ispnew, spd_state->ispold, M);

    /* Check stability on isf : distance between old isf and current isf */

    L_tmp = 0;
    for (i = 0; i < M - 1; i++)
    {
        tmp = sub(isf[i], spd_state->isfold[i]);
        L_tmp = L_mac(L_tmp, tmp, tmp);
    }
    L_tmp = L_shl(L_tmp, 8);				// OVERFLOWed!!!
    tmp = (Word16)(L_tmp >> 16);
    tmp = mult(tmp, 26214);                /* tmp = L_tmp*0.8/256 */

    tmp = sub(20480, tmp);                 /* 1.25 - tmp */
    stab_fac = shl(tmp, 1);                /* Q14 -> Q15 with saturation */
    if (stab_fac < 0)
    {
        stab_fac = 0;
    }
    Copy(spd_state->isfold, isf_tmp, M);
    Copy(isf, spd_state->isfold, M);

    /*------------------------------------------------------------------------*
     *          Loop for every subframe in the analysis frame                 *
     *------------------------------------------------------------------------*
     * The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
     *  times                                                                 *
     *     - decode the pitch delay and filter mode                           *
     *     - decode algebraic code                                            *
     *     - decode pitch and codebook gains                                  *
     *     - find voicing factor and tilt of code for next subframe.          *
     *     - find the excitation and compute synthesis speech                 *
     *------------------------------------------------------------------------*/

    p_Aq = Aq;                      /* pointer to interpolated LPC parameters */

    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {
        if (((i_subfr - 2 * L_SUBFR == 0) && (nb_bits > NBBITS_7k)) ||
              (i_subfr == 0))
        {
        /*-------------------------------------------------*
         * - Decode pitch lag                              *
         * Lag indeces received also in case of BFI,       *
         * so that the parameter pointer stays in sync.    *
         *-------------------------------------------------*/
            if (nb_bits <= NBBITS_9k)
            {
                index = Serial_parm(8, &prms);
                if (index < (PIT_FR1_8b - PIT_MIN) * 2)
                {
                    T0 = add(PIT_MIN, shr(index, 1));
                    T0_frac = sub(index, shl(sub(T0, PIT_MIN), 1));
                    T0_frac = shl(T0_frac, 1);
                } else
                {
                    T0 = add(index, PIT_FR1_8b - ((PIT_FR1_8b - PIT_MIN) * 2));
                    T0_frac = 0;
                }
            } else
            {
                index = Serial_parm(9, &prms);
                if (index < (PIT_FR2 - PIT_MIN) * 4)
                {
                    T0 = add(PIT_MIN, shr(index, 2));
                    T0_frac = sub(index, shl(sub(T0, PIT_MIN), 2));
                } else if (index < (((PIT_FR2 - PIT_MIN) * 4) + ((PIT_FR1_9b - PIT_FR2) * 2)))
                {
                    index = sub(index, (PIT_FR2 - PIT_MIN) * 4);
                    T0 = add(PIT_FR2, shr(index, 1));
                    T0_frac = sub(index, shl(sub(T0, PIT_FR2), 1));
                    T0_frac = shl(T0_frac, 1);
                } else
                {
                    T0 = add(index, (PIT_FR1_9b - ((PIT_FR2 - PIT_MIN) * 4) - ((PIT_FR1_9b - PIT_FR2) * 2)));
                    T0_frac = 0;
                }
            }

            /* find T0_min and T0_max for subframe 2 and 4 */

            T0_min = sub(T0, 8);
            if (T0_min < PIT_MIN)
            {
                T0_min = PIT_MIN;
            }
            T0_max = add(T0_min, 15);
            if (T0_max > PIT_MAX)
            {
                T0_max = PIT_MAX;
                T0_min = PIT_MAX - 15;
            }
        } else
        {                                  /* if subframe 2 or 4 */
            if (nb_bits <= NBBITS_9k)
            {
                index = Serial_parm(5, &prms);

                T0 = add(T0_min, shr(index, 1));
                T0_frac = sub(index, shl(sub(T0, T0_min), 1));
                T0_frac = shl(T0_frac, 1);
            } else
            {
                index = Serial_parm(6, &prms);

                T0 = add(T0_min, shr(index, 2));
                T0_frac = sub(index, shl(sub(T0, T0_min), 2));
            }
        }

        /* check BFI after pitch lag decoding */
        if (bfi != 0)                      /* if frame erasure */
        {
            lagconc(&(spd_state->dec_gain[17]), spd_state->lag_hist, &T0, &(spd_state->old_T0), &(spd_state->seed3), unusable_frame);
            T0_frac = 0;
        }
        /*-------------------------------------------------*
         * - Find the pitch gain, the interpolation filter *
         *   and the adaptive codebook vector.             *
         *-------------------------------------------------*/

        Pred_lt4(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);

        if (unusable_frame != 0)
        {
            /* the innovative code doesn't need to be scaled (see Q_gain2) */
            for (i = 0; i < L_SUBFR; i++)
            {
                code[i] = Random(&spd_state->seed) >> 3;
            }
        } else
        {
            if (nb_bits <= NBBITS_9k)
            {
                select = 0;
            } else
            {
                select = Serial_parm(1, &prms);
            }

            if (select == 0)
            {
                /* find pitch excitation with lp filter */
                for (i = 0; i < L_SUBFR; i++)
                {
                    L_tmp = L_mult(5898, exc[i - 1 + i_subfr]);
                    L_tmp = L_mac(L_tmp, 20972, exc[i + i_subfr]);
                    L_tmp = L_mac(L_tmp, 5898, exc[i + 1 + i_subfr]);
                    code[i] = round16(L_tmp);
                }
                Copy(code, &exc[i_subfr], L_SUBFR);
            }
        /*-------------------------------------------------------*
         * - Decode innovative codebook.                         *
         * - Add the fixed-gain pitch contribution to code[].    *
         *-------------------------------------------------------*/
            if (nb_bits <= NBBITS_7k)
            {
                ind[0] = Serial_parm(12, &prms);
                DEC_ACELP_2t64_fx(ind[0], code);
            } else if (nb_bits <= NBBITS_9k)
            {
                for (i = 0; i < 4; i++)
                {
                    ind[i] = Serial_parm(5, &prms);
                }
                DEC_ACELP_4t64_fx(ind, 20, code);
            } else if (nb_bits <= NBBITS_12k)
            {
                for (i = 0; i < 4; i++)
                {
                    ind[i] = Serial_parm(9, &prms);
                }
                DEC_ACELP_4t64_fx(ind, 36, code);
            } else if (nb_bits <= NBBITS_14k)
            {
                ind[0] = Serial_parm(13, &prms);
                ind[1] = Serial_parm(13, &prms);
                ind[2] = Serial_parm(9, &prms);
                ind[3] = Serial_parm(9, &prms);
                DEC_ACELP_4t64_fx(ind, 44, code);
            } else if (nb_bits <= NBBITS_16k)
            {
                for (i = 0; i < 4; i++)
                {
                    ind[i] = Serial_parm(13, &prms);
                }
                DEC_ACELP_4t64_fx(ind, 52, code);
            } else if (nb_bits <= NBBITS_18k)
                {
                for (i = 0; i < 4; i++)
                {
                    ind[i] = Serial_parm(2, &prms);
                }
                for (i = 4; i < 8; i++)
                {
                    ind[i] = Serial_parm(14, &prms);
                }
                DEC_ACELP_4t64_fx(ind, 64, code);
            } else if (nb_bits <= NBBITS_20k)
            {
                ind[0] = Serial_parm(10, &prms);
                ind[1] = Serial_parm(10, &prms);
                ind[2] = Serial_parm(2, &prms);
                ind[3] = Serial_parm(2, &prms);
                ind[4] = Serial_parm(10, &prms);
                ind[5] = Serial_parm(10, &prms);
                ind[6] = Serial_parm(14, &prms);
                ind[7] = Serial_parm(14, &prms);
                DEC_ACELP_4t64_fx(ind, 72, code);
            } else
            {
                for (i = 0; i < 4; i++)
                {
                    ind[i] = Serial_parm(11, &prms);
                }
                for (i = 4; i < 8; i++)
                {
                    ind[i] = Serial_parm(11, &prms);
                }
                DEC_ACELP_4t64_fx(ind, 88, code);
            }
        }

        Preemph(code, spd_state->tilt_code, L_SUBFR);

        tmp = T0;
        if (T0_frac > 2)
        {
            tmp++;
        }
        Pit_shrp(code, tmp, PIT_SHARP, L_SUBFR);

        /*-------------------------------------------------*
         * - Decode codebooks gains.                       *
         *-------------------------------------------------*/
        if (nb_bits <= NBBITS_9k)
        {
            tmp = 6;
        } else
        {
            tmp = 7;
        }
        index = Serial_parm(tmp, &prms); /* codebook gain index */

        D_gain2(index, tmp, code, L_SUBFR, &gain_pit, &L_gain_code, bfi,
                spd_state->prev_bfi, spd_state->state, unusable_frame,
                spd_state->vad_hist, spd_state->dec_gain);

        /* find best scaling to perform on excitation (Q_new) */
        /* limit scaling (Q_new) to Q_MAX: see cnst.h and syn_filt_32() */
        tmp = Q_MAX;
        for (i = 0; i < 4; i++)
        {
            if (tmp > spd_state->Qsubfr[i])
            {
                tmp = spd_state->Qsubfr[i];
            }
        }

        Q_new = 0;
        L_tmp = L_gain_code;                /* L_gain_code in Q16 */

        while ((L_tmp < 0x08000000L) && (Q_new < tmp))
        {
            L_tmp <<= 1;
            Q_new++;
        }
        gain_code = round16(L_tmp);          /* scaled gain_code with Qnew */

        Scale_sig(exc + i_subfr - (PIT_MAX + L_INTERPOL),
            PIT_MAX + L_INTERPOL + L_SUBFR, sub(Q_new, spd_state->Q_old));
        spd_state->Q_old = Q_new;


        /*----------------------------------------------------------*
         * Update parameters for the next subframe.                 *
         * - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
         *----------------------------------------------------------*/

        if (bfi == 0)
        {
            /* LTP-Lag history update */
            for (i = 4; i > 0; i--)
            {
                spd_state->lag_hist[i] = spd_state->lag_hist[i - 1];
            }
            spd_state->lag_hist[0] = T0;

            spd_state->old_T0 = T0;
            spd_state->old_T0_frac = 0;             /* Remove fraction in case of BFI */
        }
        /* find voice factor in Q15 (1=voiced, -1=unvoiced) */

        Copy(&exc[i_subfr], exc2, L_SUBFR);
        Scale_sig(exc2, L_SUBFR, -3);

        /* post processing of excitation elements */
        if (nb_bits <= NBBITS_9k)
        {
            pit_sharp = shl(gain_pit, 1);
            if (gain_pit > 8192)
            {
                for (i = 0; i < L_SUBFR; i++)
                {
                    tmp = mult(exc2[i], pit_sharp);
                    L_tmp = L_mult(tmp, gain_pit);
                    L_tmp >>= 1;
                    excp[i] = round16(L_tmp);
                }
            }
        }

        voice_fac = voice_factor(exc2, -3, gain_pit, code, gain_code, L_SUBFR);

        /* tilt of code for next subframe: 0.5=voiced, 0=unvoiced */

        spd_state->tilt_code = add((voice_fac >> 2), 8192);

        /*-------------------------------------------------------*
         * - Find the total excitation.                          *
         * - Find synthesis speech corresponding to exc[].       *
         *-------------------------------------------------------*/

        Copy(&exc[i_subfr], exc2, L_SUBFR);

        for (i = 0; i < L_SUBFR; i++)
        {
            L_tmp = L_mult(code[i], gain_code);
            L_tmp = L_shl(L_tmp, 5);
            L_tmp = L_mac(L_tmp, exc[i + i_subfr], gain_pit);
            L_tmp = L_shl(L_tmp, 1);
            exc[i + i_subfr] = round16(L_tmp);
        }

        /* find maximum value of excitation for next scaling */
/*        max = 1;
        for (i = 0; i < L_SUBFR; i++)
        {
            tmp = abs_s(exc[i + i_subfr]);
            if (tmp > max)
            {
                max = tmp;
            }
        }
*/
		max = arrMax(&exc[i_subfr], L_SUBFR);

        /* tmp = scaling possible according to max value of excitation */
        tmp = norm_s0(max);
        tmp = tmp + Q_new - 1;

        spd_state->Qsubfr[3] = spd_state->Qsubfr[2];
        spd_state->Qsubfr[2] = spd_state->Qsubfr[1];
        spd_state->Qsubfr[1] = spd_state->Qsubfr[0];
        spd_state->Qsubfr[0] = tmp;

        /*------------------------------------------------------------*
         * phase dispersion to enhance noise in low bit rate          *
         *------------------------------------------------------------*/

        if (nb_bits <= NBBITS_7k)
            j = 0;              /* high dispersion for rate <= 7.5 kbit/s */
        else if (nb_bits <= NBBITS_9k)
            j = 1;              /* low dispersion for rate <= 9.6 kbit/s */
        else
            j = 2;              /* no dispersion for rate > 9.6 kbit/s */

        /* L_gain_code in Q16 */
        gain_code = (Word16)(L_gain_code >> 16);

        Phase_dispersion(gain_code, gain_pit, code, j, spd_state->disp_mem);

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

        if (L_gain_code < spd_state->L_gc_thres)
        {
            L_tmp = Mpy_32x16(L_gain_code, 6226);
            L_tmp = L_add(L_tmp, L_gain_code);

            if (L_tmp < spd_state->L_gc_thres)
                spd_state->L_gc_thres = L_tmp;
        } else
        {
            L_tmp = Mpy_32x16(L_gain_code, 27536);

            if (L_tmp > spd_state->L_gc_thres)
                spd_state->L_gc_thres = L_tmp;
        }

        L_gain_code = Mpy_32x16(L_gain_code, (32767 - fac));
        L_tmp = Mpy_32x16(spd_state->L_gc_thres, fac);
        L_gain_code = L_add(L_gain_code, L_tmp);

        /*------------------------------------------------------------*
         * pitch enhancer                                             *
         * ~~~~~~~~~~~~~~                                             *
         * - Enhance excitation on voice. (HP filtering of code)      *
         *   On voiced signal, filtering of code by a smooth fir HP   *
         *   filter to decrease energy of code in low frequency.      *
         *------------------------------------------------------------*/

        tmp = add((voice_fac >> 3), 4096);/* 0.25=voiced, 0=unvoiced */

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
            L_tmp = L_shl(L_tmp, 1);       /* saturation can occur here */
            exc2[i] = round16(L_tmp);
        }

        if (nb_bits <= NBBITS_9k)
        {
            if (gain_pit > 8192)
            {
                for (i = 0; i < L_SUBFR; i++)
                {
                    excp[i] = add(excp[i], exc2[i]);
                }
                agc2(exc2, excp, L_SUBFR);
                Copy(excp, exc2, L_SUBFR);
            }
        }

        if (nb_bits <= NBBITS_7k)
        {
            j = i_subfr >> 6;
            for (i = 0; i < M; i++)
            {
                L_tmp = L_mult(isf_tmp[i], interpol_back[j]);
                L_tmp = L_mac(L_tmp, isf[i], interpol_frac[j]);
                HfIsf[i] = round16(L_tmp);
            }
        } else
        {
            Set_zero(spd_state->mem_syn_hf, M16k - M);
        }

        if (nb_bits >= NBBITS_24k)
        {
            tmp = Serial_parm(4, &prms);
        } else
        {
            tmp = 0;
        }

        synthesis_dec(p_Aq, exc2, Q_new, &synth16k[i_subfr * 5 / 4], tmp,
                      HfIsf, nb_bits, newDTXState, spd_state, bfi);

        p_Aq += (M + 1);     /* interpolated LPC parameters for next subframe */
    }

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> save past of exc[].                           *
     * -> save pitch parameters.                        *
     *--------------------------------------------------*/

    Copy(&old_exc[L_FRAME], spd_state->old_exc, PIT_MAX + L_INTERPOL);

    Scale_sig(exc, L_FRAME, sub(0, Q_new));
    dtx_dec_activity_update(spd_state->dtx_decSt, isf, exc);

    spd_state->dtx_decSt->dtxGlobalState = newDTXState;

    spd_state->prev_bfi = bfi;

    /* if not homed: check whether current frame is a homing frame */
    if (reset_flag_old == 0)
    { /* check whole frame */
      //   reset_flag = decoder_homing_frame_test(prms, mode0);
        reset_flag = dhf_test(prms, mode0, nb_of_bits[mode0]);
    }
    /* reset decoder if current frame is a homing frame */
    if (reset_flag != 0)
    {
        Reset_decoder(spd_state, 1);
    }
    reset_flag_old = reset_flag;

    return;
}
