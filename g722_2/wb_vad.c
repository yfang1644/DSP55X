/*-------------------------------------------------------------------*
 *                         WB_VAD.C									 *
 *-------------------------------------------------------------------*
 * Voice Activity Detection.										 *
 *-------------------------------------------------------------------*/

/******************************************************************************
*                         INCLUDE FILES
******************************************************************************/

#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "math_op.h"
#include "wb_vad_c.h"
#include "wb_vad.h"

VadVars vad_state;

/******************************************************************************
*                         PRIVATE PROGRAM CODE
******************************************************************************/

/******************************************************************************
* log2
*
*  Calculate Log2 and scale the signal:
*
*    ilog2(Word32 in) = -1024*log10(in * 2^-31)/log10(2), where in = [1, 2^31-1]
*
*  input   output
*  32768   16384
*  1       31744
*
* When input is in the range of [1,2^16], max error is 0.0380%.
*
*
*/
Word16 ilog2(Word16);

Word16 ilog2_(                        /* return: output value of the log2 */
     Word16 mant                     /* i: value to be converted */
)
{
    Word16 ex, ex2, res;
    Word32 L_temp;

    if (mant <= 0)
    {
        mant = 1;
    }
    ex = norm_s0(mant);
    mant = shl(mant, ex);

    mant = mult(mant, mant);
    mant = mult(mant, mant);
    mant = mult(mant, mant);
    L_temp = L_mult(mant, mant);

    ex2 = -exp_mant0(&L_temp);
    mant = (Word16)(L_temp >> 24);

    res = (ex + 16) << 10;
    res = res + (ex2 << 6);
    res = res + 127 - mant;
    return (res);
}

/******************************************************************************
*
*     Function     : filter5
*     Purpose      : Fifth-order half-band lowpass/highpass filter pair with
*                    decimation.
*
*/
void filter5(
     Word16 * in0,          /* i/o : input values; output low-pass part  */
     Word16 * in1,          /* i/o : input values; output high-pass part */
     Word16 data[]          /* i/o : filter memory                       */
)
{
    Word16 temp0, temp1, temp2;
    Word32 L_tmp;

    temp0 = sub(*in0, mult(COEFF5_1, data[0]));
    temp1 = add(data[0], mult(COEFF5_1, temp0));
    data[0] = temp0;

    temp0 = sub(*in1, mult(COEFF5_2, data[1]));
    temp2 = add(data[1], mult(COEFF5_2, temp0));
    data[1] = temp0;

    L_tmp = (Word32)temp1 + (Word32)temp2;
    *in0 = L_tmp >> 1;
    L_tmp = (Word32)temp1 - (Word32)temp2;
    *in1 = L_tmp >> 1;
}

/******************************************************************************
*
*     Function     : filter3
*     Purpose      : Third-order half-band lowpass/highpass filter pair with
*                    decimation.
*
*/
void filter3(
     Word16 * in0,          /* i/o : input values; output low-pass part  */
     Word16 * in1,          /* i/o : input values; output high-pass part */
     Word16 * data          /* i/o : filter memory                       */
)
{
    Word16 temp1, temp2;
    Word32 L_tmp;

    temp1 = sub(*in1, mult(COEFF3, *data));
    temp2 = add(*data, mult(COEFF3, temp1));
    *data = temp1;

    L_tmp = ((Word32)*in0) - (Word32)temp2;
    *in1 = L_tmp >> 1;
    L_tmp = ((Word32)*in0) + (Word32)temp2;
    *in0 = L_tmp >> 1;
}

/******************************************************************************
*
*     Function   : level_calculation
*     Purpose    : Calculate signal level in a sub-band. Level is calculated
*                  by summing absolute values of the input data.
*
*                  Signal level calculated from of the end of the frame
*                  (data[count1 - count2]) is stored to (*sub_level)
*                  and added to the level of the next frame.
*
*/
Word16 level_calculation(      /* return: signal level */
     Word16 data[],        /* i : signal buffer                           */
     Word16 * sub_level,   /* i : level calculated at the end of the previous frame*/
						   /* o : level of signal calculated from the last*/
						   /*     (count2 - count1) samples               */
     Word16 count1,        /* i : number of samples to be counted         */
     Word16 count2,        /* i : number of samples to be counted         */
     Word16 ind_m,         /* i : step size for the index of the data buffer*/
     Word16 ind_a,         /* i : starting index of the data buffer       */
     Word16 scale          /* i : scaling for the level calculation       */
)
{
    Word32 L_temp1, L_temp2;
    Word16 level, i;
    Word16 exp;

    L_temp1 = 0L;
    for (i = count1; i < count2; i++)
    {
        L_temp1 = L_mac(L_temp1, 1, abs_s(data[ind_m * i + ind_a]));
    }

    exp = sub(16, scale);
    L_temp2 = L_add(L_temp1, L_shl((Word32)*sub_level, exp));
    *sub_level = (Word16)(L_shl(L_temp1, scale) >> 16);

    for (i = 0; i < count1; i++)
    {
        L_temp2 = L_mac(L_temp2, 1, abs_s(data[ind_m * i + ind_a]));
    }

    level = (Word16)(L_shl(L_temp2, scale) >> 16);
    return level;
}

/******************************************************************************
*
*     Function     : filter_bank
*     Purpose      : Divide input signal into bands and calculate level of
*                    the signal in each band
*
*/
void filter_bank(
     VadVars * st,              /* i/o : State struct               */
     Word16 in[],               /* i   : input frame                */
     Word16 level[]             /* 0   : signal levels at each band */
)
{
    Word16 i;
    Word16 tmp_buf[FRAME_LEN];

    /* shift input 1 bit down for safe scaling */
    for (i = 0; i < FRAME_LEN; i++)
    {
        tmp_buf[i] = shr(in[i], 1);
    }

    /* run the filter bank */
    for (i = 0; i < FRAME_LEN / 2; i++)
    {
        filter5(&tmp_buf[2 * i], &tmp_buf[2 * i + 1], st->a_data5[0]);
    }
    for (i = 0; i < FRAME_LEN / 4; i++)
    {
        filter5(&tmp_buf[4 * i], &tmp_buf[4 * i + 2], st->a_data5[1]);
        filter5(&tmp_buf[4 * i + 1], &tmp_buf[4 * i + 3], st->a_data5[2]);
    }
    for (i = 0; i < FRAME_LEN / 8; i++)
    {
        filter5(&tmp_buf[8 * i], &tmp_buf[8 * i + 4], st->a_data5[3]);
        filter5(&tmp_buf[8 * i + 2], &tmp_buf[8 * i + 6], st->a_data5[4]);
        filter3(&tmp_buf[8 * i + 3], &tmp_buf[8 * i + 7], &st->a_data3[0]);
    }
    for (i = 0; i < FRAME_LEN / 16; i++)
    {
        filter3(&tmp_buf[16 * i + 0], &tmp_buf[16 * i + 8], &st->a_data3[1]);
        filter3(&tmp_buf[16 * i + 4], &tmp_buf[16 * i + 12], &st->a_data3[2]);
        filter3(&tmp_buf[16 * i + 6], &tmp_buf[16 * i + 14], &st->a_data3[3]);
    }

    for (i = 0; i < FRAME_LEN / 32; i++)
    {
        filter3(&tmp_buf[32 * i + 0], &tmp_buf[32 * i + 16], &st->a_data3[4]);
        filter3(&tmp_buf[32 * i + 8], &tmp_buf[32 * i + 24], &st->a_data3[5]);
    }

    /* calculate levels in each frequency band */

    /* 4800 - 6400 Hz */
    level[11] = level_calculation(tmp_buf, &st->sub_level[11],
        FRAME_LEN / 4 - 48, FRAME_LEN / 4, 4, 1, 14);
    /* 4000 - 4800 Hz */
    level[10] = level_calculation(tmp_buf, &st->sub_level[10],
        FRAME_LEN / 8 - 24, FRAME_LEN / 8, 8, 7, 15);
    /* 3200 - 4000 Hz */
    level[9] = level_calculation(tmp_buf, &st->sub_level[9],
        FRAME_LEN / 8 - 24, FRAME_LEN / 8, 8, 3, 15);
    /* 2400 - 3200 Hz */
    level[8] = level_calculation(tmp_buf, &st->sub_level[8],
        FRAME_LEN / 8 - 24, FRAME_LEN / 8, 8, 2, 15);
    /* 2000 - 2400 Hz */
    level[7] = level_calculation(tmp_buf, &st->sub_level[7],
        FRAME_LEN / 16 - 12, FRAME_LEN / 16, 16, 14, 16);
    /* 1600 - 2000 Hz */
    level[6] = level_calculation(tmp_buf, &st->sub_level[6],
        FRAME_LEN / 16 - 12, FRAME_LEN / 16, 16, 6, 16);
    /* 1200 - 1600 Hz */
    level[5] = level_calculation(tmp_buf, &st->sub_level[5],
        FRAME_LEN / 16 - 12, FRAME_LEN / 16, 16, 4, 16);
    /* 800 - 1200 Hz */
    level[4] = level_calculation(tmp_buf, &st->sub_level[4],
        FRAME_LEN / 16 - 12, FRAME_LEN / 16, 16, 12, 16);
    /* 600 - 800 Hz */
    level[3] = level_calculation(tmp_buf, &st->sub_level[3],
        FRAME_LEN / 32 - 6, FRAME_LEN / 32, 32, 8, 17);
    /* 400 - 600 Hz */
    level[2] = level_calculation(tmp_buf, &st->sub_level[2],
        FRAME_LEN / 32 - 6, FRAME_LEN / 32, 32, 24, 17);
    /* 200 - 400 Hz */
    level[1] = level_calculation(tmp_buf, &st->sub_level[1],
        FRAME_LEN / 32 - 6, FRAME_LEN / 32, 32, 16, 17);
    /* 0 - 200 Hz */
    level[0] = level_calculation(tmp_buf, &st->sub_level[0],
        FRAME_LEN / 32 - 6, FRAME_LEN / 32, 32, 0, 17);
}

/******************************************************************************
*
*     Function   : update_cntrl
*     Purpose    : Control update of the background noise estimate.
*
*/
void update_cntrl(
     VadVars * st,         /* i/o : State structure                    */
     Word16 level[]        /* i   : sub-band levels of the input frame */
)
{
    Word16 i, temp, stat_rat, exp;
    Word16 num, denom;
    Word16 alpha;

    /* if a tone has been detected for a while, initialize stat_count */
    if ((st->tone_flag & 0x7c00) == 0x7c00)
    {
        st->stat_count = STAT_COUNT;
    } else
    {
        /* if 8 last vad-decisions have been "0", reinitialize stat_count */
        if ((st->vadreg & 0x7f80) == 0)
        {
            st->stat_count = STAT_COUNT;
        } else
        {
            stat_rat = 0;
            for (i = 0; i < COMPLEN; i++)
            {
                if (level[i] > st->ave_level[i])
                {
                    num = level[i];
                    denom = st->ave_level[i];
                } else
                {
                    num = st->ave_level[i];
                    denom = level[i];
                }
                /* Limit nimimum value of num and denom to STAT_THR_LEVEL */
                if (num < STAT_THR_LEVEL)
                {
                    num = STAT_THR_LEVEL;
                }
                if (denom < STAT_THR_LEVEL)
                {
                    denom = STAT_THR_LEVEL;
                }
                exp = norm_s0(denom);
                denom = shl(denom, exp);

                /* stat_rat = num/denom * 64 */
                temp = div_s((num >> 1), denom);
                stat_rat = add(stat_rat, shr(temp, sub(8, exp)));
            }

            /* compare stat_rat with a threshold and update stat_count */
            if (stat_rat > STAT_THR)
            {
                st->stat_count = STAT_COUNT;
            } else
            {
                if ((st->vadreg & 0x4000) != 0)
                {
                    if (st->stat_count != 0)
                    {
                        st->stat_count = sub(st->stat_count, 1);
                    }
                }
            }
        }
    }

    /* Update average amplitude estimate for stationarity estimation */
    alpha = ALPHA4;
    if (st->stat_count == STAT_COUNT)
    {
        alpha = 32767;
    } else if ((st->vadreg & 0x4000) == 0)
    {
        alpha = ALPHA5;
    }
    for (i = 0; i < COMPLEN; i++)
    {
        st->ave_level[i] = add(st->ave_level[i],
            mult_r(alpha, sub(level[i], st->ave_level[i])));
    }
}

/******************************************************************************
*
*     Function     : hangover_addition
*     Purpose      : Add hangover after speech bursts
*
*/

Word16 hangover_addition(     /* return: VAD_flag indicating final VAD decision */
     VadVars * st,            /* i/o : State structure                     */
     Word16 low_power,        /* i   : flag power of the input frame    */
     Word16 hang_len,         /* i   : hangover length */
     Word16 burst_len         /* i   : minimum burst length for hangover addition */
)
{
    /* if the input power (pow_sum) is lower than a threshold, clear counters and set VAD_flag to "0"         */
    if (low_power != 0)
    {
        st->burst_count = 0;
        st->hang_count = 0;
        return 0;
    }
    /* update the counters (hang_count, burst_count) */
    if ((st->vadreg & 0x4000) != 0)
    {
        st->burst_count = add(st->burst_count, 1);
        if (st->burst_count >= burst_len)
        {
            st->hang_count = hang_len;
        }
        return 1;
    } else
    {
        st->burst_count = 0;
        if (st->hang_count > 0)
        {
            st->hang_count = sub(st->hang_count, 1);
            return 1;
        }
    }
    return 0;
}

/******************************************************************************
*
*     Function   : noise_estimate_update
*     Purpose    : Update of background noise estimate
*
*/

void noise_estimate_update(
     VadVars * st,            /* i/o : State structure                    */
     Word16 level[]           /* i   : sub-band levels of the input frame */
)
{
    Word16 i, alpha_up, alpha_down, bckr_add;
    Word16 temp;

    /* Control update of bckr_est[] */
    update_cntrl(st, level);

    /* Reason for using bckr_add is to avoid problems caused by fixed-point dynamics when noise level and
     * required change is very small. */
    bckr_add = 2;

    /* Choose update speed */
    if ((0x7800 & st->vadreg) == 0)
    {
        alpha_up = ALPHA_UP1;
        alpha_down = ALPHA_DOWN1;
    } else
    {
        if ((st->stat_count == 0))
        {
            alpha_up = ALPHA_UP2;
            alpha_down = ALPHA_DOWN2;
        } else
        {
            alpha_up = 0;
            alpha_down = ALPHA3;
            bckr_add = 0;
        }
    }

    /* Update noise estimate (bckr_est) */
    for (i = 0; i < COMPLEN; i++)
    {
        temp = sub(st->old_level[i], st->bckr_est[i]);

        if (temp < 0)
        {                                  /* update downwards */
            st->bckr_est[i] += mult_r(alpha_down, temp) - 2;

            /* limit minimum value of the noise estimate to NOISE_MIN */
            if (st->bckr_est[i] < NOISE_MIN)
            {
                st->bckr_est[i] = NOISE_MIN;
            }
        } else
        {                                  /* update upwards */
            st->bckr_est[i] += mult_r(alpha_up, temp) + bckr_add;

            /* limit maximum value of the noise estimate to NOISE_MAX */
            if (st->bckr_est[i] > NOISE_MAX)
            {
                st->bckr_est[i] = NOISE_MAX;
            }
        }
        /* Update signal levels of the previous frame (old_level) */
        st->old_level[i] = level[i];
    }

    return;
}

/******************************************************************************
*
*     Function     : vad_decision
*     Purpose      : Calculates VAD_flag
*
*/

Word16 vad_decision(       /* return value : VAD_flag */
     VadVars * st,                /* i/o : State structure                 */
     Word16 level[COMPLEN],       /* i   : sub-band levels of the input frame */
     Word32 pow_sum               /* i   : power of the input frame           */
)
{
    Word16 i;
    Word32 L_snr_sum;
    Word32 L_tmp2;
    Word16 vad_thr, temp, noise_level;
    Word16 low_power_flag;
    Word16 hang_len, burst_len;
    Word16 ilog2_speech_level, ilog2_noise_level;
    Word16 temp2, exp;

    /* Calculate squared sum of the input levels (level) divided by the background noise components
     * (bckr_est). */
    L_snr_sum = 0;
    L_tmp2 = (Word32)(-st->bckr_est[0]);		/* ignore lowest band */
    for (i = 0; i < COMPLEN; i++)
    {
        exp = norm_s0(st->bckr_est[i]);
        temp = shl(st->bckr_est[i], exp);
        temp = div_s((level[i] >> 1), temp);
        temp = shl(temp, sub(exp, UNIRSHFT - 1));
        L_snr_sum = L_mac(L_snr_sum, temp, temp);

        /* Calculate average level of estimated background noise */
        L_tmp2 = L_tmp2 + (Word32)st->bckr_est[i];
    }

    noise_level = (Word16)(L_tmp2 >> 4);
    /* if SNR is lower than a threshold (MIN_SPEECH_SNR), and increase speech_level */
    temp = shl(mult(noise_level, MIN_SPEECH_SNR), 3);

    if (st->speech_level < temp)
    {
        st->speech_level = temp;
    }
    ilog2_noise_level = ilog2(noise_level);

    /* If SNR is very poor, speech_level is probably corrupted by noise level. This is correctred by
     * subtracting MIN_SPEECH_SNR*noise_level from speech level */
    ilog2_speech_level = ilog2(sub(st->speech_level, temp));

    temp = add(mult(NO_SLOPE, sub(ilog2_noise_level, NO_P1)), THR_HIGH);

    temp2 = add(SP_CH_MIN, mult(SP_SLOPE, sub(ilog2_speech_level, SP_P1)));
    if (temp2 < SP_CH_MIN)
    {
        temp2 = SP_CH_MIN;
    }
    if (temp2 > SP_CH_MAX)
    {
        temp2 = SP_CH_MAX;
    }
    vad_thr = add(temp, temp2);

    if (vad_thr < THR_MIN)
    {
        vad_thr = THR_MIN;
    }
    /* Shift VAD decision register */
    st->vadreg = shr(st->vadreg, 1);

    /* Make intermediate VAD decision */
    if (L_snr_sum > L_mult(vad_thr, 512 * COMPLEN))
    {
        st->vadreg = (st->vadreg | 0x4000);
    }
    /* check if the input power (pow_sum) is lower than a threshold" */
    if (pow_sum < VAD_POW_LOW)
    {
        low_power_flag = 1;
    } else
    {
        low_power_flag = 0;
    }
    /* Update background noise estimates */
    noise_estimate_update(st, level);

    /* Calculate values for hang_len and burst_len based on vad_thr */
    hang_len = add(mult(HANG_SLOPE, sub(vad_thr, HANG_P1)), HANG_HIGH);
    if (hang_len < HANG_LOW)
    {
        hang_len = HANG_LOW;
    }

    burst_len = add(mult(BURST_SLOPE, sub(vad_thr, BURST_P1)), BURST_HIGH);

    return (hangover_addition(st, low_power_flag, hang_len, burst_len));
}

/******************************************************************************
*
*     Estimate_Speech()
*     Purpose      : Estimate speech level
*
* Maximum signal level is searched and stored to the variable sp_max.
* The speech frames must locate within SP_EST_COUNT number of frames.
* Thus, noisy frames having occasional VAD = "1" decisions will not
* affect to the estimated speech_level.
*
*/
void Estimate_Speech(
     VadVars * st,                         /* i/o : State structure    */
     Word16 in_level                       /* level of the input frame */
)
{
    Word16 alpha;
    Word16 tmp;

    /* if the required activity count cannot be achieved, reset counters */
    /* if (SP_ACTIVITY_COUNT  > SP_EST_COUNT - st->sp_est_cnt + st->sp_max_cnt) */
    if (sub(st->sp_est_cnt, st->sp_max_cnt) > SP_EST_COUNT - SP_ACTIVITY_COUNT)
    {
        st->sp_est_cnt = 0;
        st->sp_max = 0;
        st->sp_max_cnt = 0;
    }
    st->sp_est_cnt = add(st->sp_est_cnt, 1);

    if (((st->vadreg & 0x4000) || (in_level > st->speech_level))
        && (in_level > MIN_SPEECH_LEVEL1))
    {
        /* update sp_max */
        if (in_level > st->sp_max)
        {
            st->sp_max = in_level;
        }
        st->sp_max_cnt = add(st->sp_max_cnt, 1);
        if (st->sp_max_cnt >= SP_ACTIVITY_COUNT)
        {
            /* update speech estimate */
            tmp = shr(st->sp_max, 1);      /* scale to get "average" speech level */

            /* select update speed */
            if (tmp > st->speech_level)
            {
                alpha = ALPHA_SP_UP;
            } else
            {
                alpha = ALPHA_SP_DOWN;
            }
            if (tmp > MIN_SPEECH_LEVEL2)
            {
                st->speech_level = add(st->speech_level,
                    mult_r(alpha, sub(tmp, st->speech_level)));
            }
            /* clear all counters used for speech estimation */
            st->sp_max = 0;
            st->sp_max_cnt = 0;
            st->sp_est_cnt = 0;
        }
    }
}

/******************************************************************************
*                         PUBLIC PROGRAM CODE
******************************************************************************/

/******************************************************************************
*
*  Function:   wb_vad_init
*  Purpose:    Initializes state memory
*
*/

Word16 wb_vad_init(             /* return: non-zero with error, zero for ok. */
     VadVars ** state            /* i/o : State structure    */
)
{
    Word16 i, j;
	VadVars *s;

	s = &vad_state;

    s->tone_flag = 0;
    s->vadreg = 0;
    s->hang_count = 0;
    s->burst_count = 0;
    s->hang_count = 0;

    /* initialize memory used by the filter bank */
    for (i = 0; i < F_5TH_CNT; i++)
    {
        for (j = 0; j < 2; j++)
        {
            s->a_data5[i][j] = 0;
        }
    }

    for (i = 0; i < F_3TH_CNT; i++)
    {
        s->a_data3[i] = 0;
    }

    /* initialize the rest of the memory */
    for (i = 0; i < COMPLEN; i++)
    {
        s->bckr_est[i] = NOISE_INIT;
        s->old_level[i] = NOISE_INIT;
        s->ave_level[i] = NOISE_INIT;
        s->sub_level[i] = 0;
    }

    s->sp_est_cnt = 0;
    s->sp_max = 0;
    s->sp_max_cnt = 0;
    s->speech_level = SPEECH_LEVEL_INIT;
    s->prev_pow_sum = 0;

	*state = s;
    return 0;
}


/******************************************************************************
*
*     Function     : wb_vad_tone_detection
*     Purpose      : Search maximum pitch gain from a frame. Set tone flag if
*                    pitch gain is high. This is used to detect
*                    signaling tones and other signals with high pitch gain.
*
*/
void wb_vad_tone_detection(
     VadVars * st,                         /* i/o : State struct            */
     Word16 p_gain                         /* pitch gain      */
)
{
    /* update tone flag */
    st->tone_flag = st->tone_flag >> 1;

    /* if (pitch_gain > TONE_THR) set tone flag */
    if (p_gain > TONE_THR)
    {
        st->tone_flag = (st->tone_flag | 0x4000);
    }
}

/******************************************************************************
*
*     Function     : wb_vad
*     Purpose      : Main program for Voice Activity Detection (VAD) for AMR
*
*/
Word16 wb_vad(                /* Return value : VAD Decision, 1 = speech, 0 = noise */
     VadVars * st,            /* i/o : State structure                 */
     Word16 in_buf[]          /* i   : samples of the input frame   */
)
{
    Word16 level[COMPLEN];
    Word16 i;
    Word16 VAD_flag, temp;
    Word32 L_temp, pow_sum;

    /* Calculate power of the input frame. */
    L_temp = Dot_product(in_buf, in_buf, FRAME_LEN);

    /* pow_sum = power of current frame and previous frame */
    pow_sum = L_add(L_temp, st->prev_pow_sum);

    /* save power of current frame for next call */
    st->prev_pow_sum = L_temp;

    /* If input power is very low, clear tone flag */
    if (pow_sum < POW_TONE_THR)
    {
        st->tone_flag = (st->tone_flag & 0x1fff);
    }
    /* Run the filter bank and calculate signal levels at each band */
    filter_bank(st, in_buf, level);

    /* compute VAD decision */
    VAD_flag = vad_decision(st, level, pow_sum);

    /* Calculate input level */
    L_temp = 0;
    for (i = 1; i < COMPLEN; i++)          /* ignore lowest band */
    {
        L_temp += (Word32)level[i];
    }

    temp = (Word16)(L_temp >> 4);

    Estimate_Speech(st, temp);             /* Estimate speech level */
    return (VAD_flag);
}
