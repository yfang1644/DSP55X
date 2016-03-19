/*
  ===========================================================================
   File: BASOP.H                                         v.1.1 - 05.Jul.2000
  ===========================================================================

		      ITU-T STL  BASIC OPERATORS

 		      GLOBAL FUNCTION PROTOTYPES

   History:
   26.Jan.00	v1.0	Incorporated to the STL from updated G.723.1/G.729 
                        basic operator library (based on basic_op.h) and 
                        G.723.1's basop.h.
   05.Jul.00    v1.1    Added 32-bit shiftless mult/mac/msub operators
  ============================================================================
*/

#ifndef BASOP_H_DEFINED
#define BASOP_H_DEFINED

/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |                                                                           |
 | $Id $
 |___________________________________________________________________________|
*/
#include "tistdtypes.h"

#define MAX_32 (Int32)0x7fffffffL
#define MIN_32 (Int32)0x80000000L

#define MAX_16 (Int16)0x7fff
#define MIN_16 (Int16)0x8000

#define UMAX_32 (Uint32)0xffffffffL
#define UMIN_32 (Uint32)0x00000000L

/*___________________________________________________________________________
 |                                                                           |
 |   Prototypes for basic arithmetic operators                               |
 |___________________________________________________________________________|
*/

#define add(a,b)        (_sadd((a),(b)))
#define sub(a,b)        (_ssub((a),(b)))

#define L_mult(a,b)     (_lsmpy((a),(b)))
#define L_mac(a,b,c)    (_smac((a),(b),(c)))
#define L_msu(a,b,c)    (_smas((a),(b),(c)))
#define L_add(a,b)      (_lsadd((a),(b)))
#define L_sub(a,b)      (_lssub((a),(b)))

#define abs_s(a)        (_abss((a)))
#define negate(a)       (_sneg(a))
#define shl(a, b)       (_sshl((a), (b)))
#define shr(a, b)       (_shrs((a), (b)))

#define L_shl(a,b)      (_lsshl((a),(b)))
#define L_shr(a,b)      (_lshrs((a),(b)))

#define mult(a,b)       (_smpy((a),(b)))
#define round16(a)      (short)(_sround(a)>>16)

#define L_abs(a)        (_labss((a)))
#define L_negate(a)     (_lsneg(a))      
#define mult_r(a,b)     ((short)(_smpyr((a),(b))>>16))

#define div_s(a,b)      (divs(a,b))
#define norm_l(a)       (_lnorm(a))
#define norm_s(a)       (_norm(a))

/*
Int16 extract_h (Int32 L_var1);                       // Extract high,     1
Int16 saturate (Int32 L_var1);
Int16 add (Int16 var1, Int16 var2);                  // Short add,        1
Int16 sub (Int16 var1, Int16 var2);                  // Short sub,        1
Int16 abs_s (Int16 var1);                             // Short abs,        1
Int16 shl (Int16 var1, Int16 var2);                  // Short shift left, 1
Int16 shr (Int16 var1, Int16 var2);                  // Short shift right,1
Int16 mult (Int16 var1, Int16 var2);                 // Short mult,       1
Int32 L_mult (Int16 var1, Int16 var2);               // Long mult,        1
Int16 negate (Int16 var1);                            // Short negate,     1
Int16 round16 (Int32 L_var1);                         // Round,            1
Int32 L_mac (Int32 L_var3, Int16 var1, Int16 var2); // Mac,              1
Int32 L_msu (Int32 L_var3, Int16 var1, Int16 var2); // Msu,              1
Int32 L_add (Int32 L_var1, Int32 L_var2);            // Long add,         2
Int32 L_sub (Int32 L_var1, Int32 L_var2);            // Long sub,         2
Int16 mult_r (Int16 var1, Int16 var2);               // Mult with round,  2
Int32 L_shl (Int32 L_var1, Int16 var2);              // Long shift left,  2
Int32 L_shr (Int32 L_var1, Int16 var2);              // Long shift right, 2
Int16 shr_r (Int16 var1, Int16 var2);                // Shift right with
                                                            round, 2
Int16 mac_r (Int32 L_var3, Int16 var1, Int16 var2); // Mac with
                                                           rounding,2
Int32 L_deposit_l (Int16 var1);                  // 16 bit var1 -> LSB,    2
Int16 norm_s (Int16 var1);                       // Short norm,           15

//   New shiftless operators, not used in G.729/G.723.1
Int32 L_mult0(Int16 v1, Int16 v2);             // Long Multiply w/o shift 1
Int32 L_mac0(Int32 L_v3, Int16 v1, Int16 v2); // Long Mac w/o shift      1

//   Additional G.722.1 operators
UInt32 LU_shl (UInt32 L_var1, Int16 var2);
UInt32 LU_shr (UInt32 L_var1, Int16 var2);
*/

#include <c55x.h>

#endif /* BASOP_H_DEFINED */

/* ************************* END OF BASIC_OP.H ************************* */

