/*-----------------------------------------------------------------------*
 *                         HP400.C										 *
 *-----------------------------------------------------------------------*
 * Interpolation of the LP parameters in 4 subframes.					 *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "acelp.h"

/* LPC interpolation coef {0.45, 0.8, 0.96, 1.0}; in Q15 */
Word16 interpol_frac[NB_SUBFR] = {14746, 26214, 31457, 32767};
Word16 interpol_back[NB_SUBFR] = {18021,  6553,  1310,     0};

void Int_isp(
     Word16 isp_old[],        /* input : isps from past frame              */
     Word16 isp_new[],        /* input : isps from present frame           */
     Word16 Az[]              /* output: LP coefficients in 4 subframes    */
)
{
    Word16 i, k, fac_old, fac_new;
    Word16 isp[M];
    Word32 L_tmp;

    for (k = 0; k < 3; k++)
    {
        fac_new = interpol_frac[k];
        fac_old = interpol_back[k]+1;  /* 1.0 - fac_new */

        for (i = 0; i < M; i++)
        {
            L_tmp = L_mult(isp_old[i], fac_old);
            L_tmp = L_mac(L_tmp, isp_new[i], fac_new);
            isp[i] = round16(L_tmp);
        }
        Isp_Az(isp, Az, M, 0);
        Az += M + 1;
    }

    /* 4th subframe: isp_new (frac=1.0) */

    Isp_Az(isp_new, Az, M, 0);

    return;
}
