/*
  ===========================================================================
   File: TYPEDEF.H                                       v.1.0 - 26.Jan.2000
  ===========================================================================

		      ITU-T STL  BASIC OPERATORS

 		      TYPE DEFINITION PROTOTYPES

   History:
   26.Jan.00	v1.0	Incorporated to the STL from updated G.723.1/G.729 
                        basic operator library (based on basic_op.h)
  ============================================================================
*/

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "tistdtypes.h"

/*
typedef int Int16;
typedef unsigned int UInt16;

typedef long Int32;
typedef unsigned long UInt32;

typedef int Flag;
*/
/***************************************************************************/
/* Type definitions                                                        */
/***************************************************************************/
typedef struct
{
    Int16 code_bit_count;      /* bit count of the current word */
    Int16 current_word;        /* current word in the bitstream being processed */
    Int16 *code_word_ptr;      /* pointer to the bitstream */
    Int16 number_of_bits_left; /* number of bits left in the current word */
    Int16 next_bit;            /* next bit in the current word */
}Bit_Obj;

typedef struct
{
    Int16 seed0;
    Int16 seed1;
    Int16 seed2;
    Int16 seed3;
}Rand_Obj;

typedef struct 
{
    Int16 cosine;
    Int16 minus_sine;
} cos_msin_t;

#endif /* TYPEDEF_H */

