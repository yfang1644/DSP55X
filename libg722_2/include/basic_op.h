/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |                                                                           |
 | $Id $
 |___________________________________________________________________________|
*/

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)+32767	/* 0x7fff */
#define MIN_16 (Word16)-32768	/* 0x8000 */

/*___________________________________________________________________________
 |                                                                           |
 |   Macros                                                                  |
 |___________________________________________________________________________|
*/

/*___________________________________________________________________________
 |                                                                           |
 |   Prototypes for basic arithmetic operators                               |
 |___________________________________________________________________________|
*/

#define	add(a,b)		(_sadd((a),(b)))
#define	sub(a,b)		(_ssub((a),(b)))

#define	L_mult(a,b)		(_lsmpy((a),(b)))
#define	L_mac(a,b,c)	(_smac((a),(b),(c)))
#define	L_msu(a,b,c)	(_smas((a),(b),(c)))
#define	L_add(a,b)		(_lsadd((a),(b)))
#define	L_sub(a,b)		(_lssub((a),(b)))

#define abs_s(a)		(_abss((a)))
#define	negate(a)		(_sneg(a))
#define	shl(a, b)		(_sshl((a), (b)))
#define	shr(a, b)		(_shrs((a), (b)))

#define L_shl(a,b)		(_lsshl((a),(b)))
#define L_shr(a,b)		(_lshrs((a),(b)))

#define mult(a,b)		(_smpy((a),(b)))
#define	round16(a)		(short)(_sround(a)>>16)

#define L_abs(a)		(_labss((a)))
#define L_negate(a)		(_lsneg(a))      
#define mult_r(a,b)		((short)(_smpyr((a),(b))>>16))

#define	norm_l0(a)		(_lnorm(a))
#define	norm_s0(a)		(_norm(a))

Word16 exp_mant0(Word32 *L_var);

Word16 arrMax(Word16 *x, Word16 lg);
Word16 div_s (Word16 var1, Word16 var2); // Short division,       18

//Word16 exp_mant0(Word32 L_var, Word32 *b);

/*
Word16 add (Word16 var1, Word16 var2);    // Short add,           1
Word16 sub (Word16 var1, Word16 var2);    // Short sub,           1
Word16 abs_s (Word16 var1);               // Short abs,           1
Word16 shl (Word16 var1, Word16 var2);    // Short shift left,    1
Word16 shr (Word16 var1, Word16 var2);    // Short shift right,   1
Word16 mult (Word16 var1, Word16 var2);   // Short mult,          1
Word32 L_mult (Word16 var1, Word16 var2); // Long mult,           1
Word16 negate (Word16 var1);              // Short negate,        1
Word16 round16 (Word32 L_var1);           // Round,               1
Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);   // Mac,  1
Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);   // Msu,  1
Word32 L_add (Word32 L_var1, Word32 L_var2);    // Long add,        2
Word32 L_sub (Word32 L_var1, Word32 L_var2);    // Long sub,        2
Word32 L_negate (Word32 L_var1);                // Long negate,     2
Word16 mult_r (Word16 var1, Word16 var2);       // Mult with round, 2
Word32 L_shl (Word32 L_var1, Word16 var2);      // Long shift left, 2
Word32 L_shr (Word32 L_var1, Word16 var2);      // Long shift right, 2
Word32 L_abs (Word32 L_var1);            // Long abs,              3
Word16 div_s (Word16 var1, Word16 var2); // Short division,       18

*/

#include <c55x.h>

