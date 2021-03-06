/*                                                      v.1.0 - 26.Jan.2000
  =============================================================================

                          U    U   GGG    SSSS  TTTTT
                          U    U  G       S       T
                          U    U  G  GG   SSSS    T
                          U    U  G   G       S   T
                           UUU     GG     SSS     T

                   ========================================
                    ITU-T - USER'S GROUP ON SOFTWARE TOOLS
                   ========================================

       =============================================================
       COPYRIGHT NOTE: This source code, and all of its derivations,
       is subject to the "ITU-T General Public License". Please have
       it  read  in    the  distribution  disk,   or  in  the  ITU-T
       Recommendation G.191 on "SOFTWARE TOOLS FOR SPEECH AND  AUDIO
       CODING STANDARDS".
       =============================================================

MODULE:         BASOP, BASIC OPERATORS

ORIGINAL BY:
                Incorporated from anonymous contributions for 
                ETSI Standards as well as G.723.1, G.729, and G.722.1

DESCRIPTION:
        This file contains the definition of 16- and 32-bit basic
        operators to be used in the implementation of signal
        processing algorithms. The basic operators try to resemble
        assembly language instructions that are commonly found in
        digital signal processor (DSP) CPUs, thus allowing algorithm
        C-code implementations more directly mapeable to DSP assembly
        code.

        *********************************************************
         NOTE: so far, this module does not have a demo program!
        *********************************************************

FUNCTIONS:
  Defined in basop.h. Self-documentation within each function.

HISTORY:
  26.Jan.00	v1.0	Incorporated to the STL from updated G.723.1/G.729 
                        basic operator library (based on basicop2.c) and 
                        G.723.1's basop.c [L_mls(), div_l(), i_mult()]
  05.Jul.00     v1.1    Added 32-bit shiftless accumulation basic 
                        operators (L_msu0, L_mac0, L_mult0). Improved
                        documentation for i_mult().
  =============================================================================
*/

/*___________________________________________________________________________
 |                                                                           |
 | Basic arithmetic operators.                                               |
 |                                                                           |
 | $Id $
 |___________________________________________________________________________|
*/

/*___________________________________________________________________________
 |                                                                           |
 |   Include-Files                                                           |
 |___________________________________________________________________________|
*/

#include "tistdtypes.h"
#include "basop32.h"

/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/
Int16 Overflow = 0;

/*___________________________________________________________________________
 |                                                                           |
 |   Functions                                                               |
 |___________________________________________________________________________|
*/


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : saturate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Limit the 32 bit input to the range of a 16 bit word.                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
Int16 saturate (Int32 L_var1)
{
    Int16 var_out;

    if (L_var1 > 0x00007fffL)
    {
        Overflow = 1;
        var_out = MAX_16;
    }
    else if (L_var1 < (Int32) 0xffff8000L)
    {
        Overflow = 1;
        var_out = MIN_16;
    }
    else
    {
        var_out = (Int16) L_var1;
    }

    return (var_out);
}
/* ------------------------- End of saturate() ------------------------- */


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult                                                    |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the multiplication of var1 by var2 and gives a 16 bit result  |
 |    which is scaled i.e.:                                                  |
 |             mult(var1,var2) = extract_l(L_shr((var1 times var2),15)) and  |
 |             mult(-32768,-32768) = 32767.                                  |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
Int16 mult (Int16 var1, Int16 var2)
{
    Int16 var_out;
    Int32 L_product;

    L_product = (Int32) var1 *(Int32) var2;

    L_product >>= 15;

    if (L_product & (Int32) 0x00010000L)
        L_product = L_product | (Int32) 0xffff0000L;

    var_out = saturate (L_product);

    return (var_out);
}
/* ------------------------- End of mult() ------------------------- */


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult_r                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as mult with rounding, i.e.:                                       |
 |     mult_r(var1,var2) = extract_l(L_shr(((var1 * var2) + 16384),15)) and  |
 |     mult_r(-32768,-32768) = 32767.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
Int16 mult_r (Int16 var1, Int16 var2)
{
    Int16 var_out;
    Int32 L_product_arr;

    L_product_arr = (Int32) var1 *(Int32) var2;       /* product */
    L_product_arr += (Int32) 0x00004000L;      /* round */
    L_product_arr &= (Int32) 0xffff8000L;
    L_product_arr >>= 15;       /* shift */

    if (L_product_arr & (Int32) 0x00010000L)   /* sign extend when necessary */
    {
        L_product_arr |= (Int32) 0xffff0000L;
    }
    var_out = saturate (L_product_arr);

    return (var_out);
}
/* ------------------------- End of mult_r() ------------------------- */

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as shr(var1,var2) but with rounding. Saturate the result in case of|
 |   underflows or overflows :                                               |
 |    - If var2 is greater than zero :                                       |
 |          if (sub(shl(shr(var1,var2),1),shr(var1,sub(var2,1))))            |
 |          is equal to zero                                                 |
 |                     then                                                  |
 |                     shr_r(var1,var2) = shr(var1,var2)                     |
 |                     else                                                  |
 |                     shr_r(var1,var2) = add(shr(var1,var2),1)              |
 |    - If var2 is less than or equal to zero :                              |
 |                     shr_r(var1,var2) = shr(var1,var2).                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
Int16 shr_r (Int16 var1, Int16 var2)
{
    Int16 var_out;

    if (var2 > 15)
    {
        var_out = 0;
    }
    else
    {
        var_out = shr (var1, var2);

        if (var2 > 0)
        {
            if ((var1 & ((Int16) 1 << (var2 - 1))) != 0)
            {
                var_out++;
            }
        }
    }

    return (var_out);
}
/* ------------------------- End of shr_r() ------------------------- */


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mac_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation. Round the LS 16 bits of the result    |
 |   into the MS 16 bits with saturation and shift the result right by 16.   |
 |   Return a 16 bit result.                                                 |
 |            mac_r(L_var3,var1,var2) = round(L_mac(L_var3,var1,var2))       |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/
Int16 mac_r (Int32 L_var3, Int16 var1, Int16 var2)
{
    Int16 var_out;

    L_var3 = L_mac (L_var3, var1, var2);

    L_var3 = L_add (L_var3, (Int32) 0x00008000L);

    var_out = (Int16) (L_var3 >> 16);

    return (var_out);
}
/* ------------------------- End of mac_r() ------------------------- */



/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_s                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shift needed to normalize the 16 bit varia- |
 |   ble var1 for positive values on the interval with minimum of 16384 and  |
 |   maximum of 32767, and for negative values on the interval with minimum  |
 |   of -32768 and maximum of -16384; in order to normalize the result, the  |
 |   following operation must be done :                                      |
 |                    norm_var1 = shl(var1,norm_s(var1)).                    |
 |                                                                           |
 |   Complexity weight : 15                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 000f.                |
 |___________________________________________________________________________|
*/
Int16 norm_s (Int16 var1)
{
    Int16 var_out;

    if (var1 == 0)
    {
        var_out = 0;
    }
    else if (var1 == (Int16) 0xffff)
    {
        var_out = 15;
    }
    else
    {
        if (var1 < 0)
        {
            var1 = ~var1;
        }
        for (var_out = 0; var1 < 0x4000; var_out++)
        {
            var1 <<= 1;
        }
    }

    return (var_out);
}
/* ------------------------- End of norm_s() ------------------------- */




/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : LU_shl                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  |
 |   fill the var2 LSB of the result. If var2 is negative, arithmetically    |
 |   shift L_var1 right by -var2 with sign extension. Saturate the result in |
 |   case of underflows or overflows.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/
UInt32 LU_shl (UInt32 L_var1, Int16 var2)
{
    Int16 neg_var2;
    UInt32 L_var_out;

    if (var2 <= 0)
    {
        if (var2 < -32)
            var2 = -32;
        neg_var2 = negate(var2);
        L_var_out = LU_shr (L_var1, neg_var2);
    }
    else
    {
        for (; var2 > 0; var2--)
        {
            if (L_var1 > (UInt32) 0x7fffffffL)
            {
                Overflow = 1;
                L_var_out = (UInt32)MAX_32;
                break;
            }
            else
            {
                if (L_var1 < (UInt32) 0x00000001L)
                {
                    Overflow = 1;
                    L_var_out = (UInt32)MIN_32;
                    break;
                }
            }
            L_var1 <<= 1;
            L_var_out = L_var1;
        }
    }

    return (L_var_out);
}
/* ------------------------- End of LU_shl() ------------------------- */


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : LU_shr                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
 |   sign extension. If var2 is negative, arithmetically shift L_var1 left   |
 |   by -var2 and zero fill the -var2 LSB of the result. Saturate the result |
 |   in case of underflows or overflows.                                     |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Int16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Int32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/
UInt32 LU_shr (UInt32 L_var1, Int16 var2)
{
    Int16  neg_var2;
    UInt32 L_var_out;

    if (var2 < 0)
    {
        if (var2 < -32)
            var2 = -32;
        neg_var2 = negate(var2);
        L_var_out = LU_shl (L_var1, neg_var2);
    }
    else
    {
        if (var2 >= 32)
        {
            L_var_out = 0L;
        }
        else
        {
            L_var_out = L_var1 >> var2;
        }
    }

    return (L_var_out);
}
/* ------------------------- End of LU_shr() ------------------------- */

/* ************************** END OF BASOP32.C ************************** */

