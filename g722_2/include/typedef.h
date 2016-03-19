/*
*
*      File             : typedef.h
*      Description      : Definition of platform independent data
*                         types and constants
*
*
*      The following platform independent data types and corresponding
*      preprocessor (#define) constants are defined:
*
*        defined type  meaning           corresponding constants
*        ----------------------------------------------------------
*        Char          character         (none)
*        Bool          boolean           true, false
*        Word8         8-bit signed      minWord8,   maxWord8
*        UWord8        8-bit unsigned    minUWord8,  maxUWord8
*        Word16        16-bit signed     minWord16,  maxWord16
*        UWord16       16-bit unsigned   minUWord16, maxUWord16
*        Word32        32-bit signed     minWord32,  maxWord32
*        UWord32       32-bit unsigned   minUWord32, maxUWord32
*        Float         floating point    minFloat,   maxFloat
*
*
*      The following compile switches are #defined:
*
*        PLATFORM      string indicating platform progam is compiled on
*                      possible values: "OSF", "PC", "SUN"
*
*        OSF           only defined if the current platform is an Alpha
*        PC            only defined if the current platform is a PC
*        SUN           only defined if the current platform is a Sun
*        
*        LSBFIRST      is defined if the byte order on this platform is
*                      "least significant byte first" -> defined on DEC Alpha
*                      and PC, undefined on Sun
*
********************************************************************************
*/
#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "tistdtypes.h"

/*
********************************************************************************
*                         DEFINITION OF CONSTANTS 
********************************************************************************
*/
/*
 ********* define char type
 */


/*
 ********* define 8 bit signed/unsigned types & constants
 */

typedef signed char Word8;
#define minWord8  -128
#define maxWord8  127

typedef unsigned char UWord8;
#define minUWord8 0
#define maxUWord8 255


/*
 ********* define 16 bit signed/unsigned types & constants
 */

typedef short Word16;
#define minWord16     -32767
#define maxWord16     32768
typedef unsigned short UWord16;
#define minUWord16    0
#define maxUWord16    65535


/*
 ********* define 32 bit signed/unsigned types & constants
 */


typedef long Word32;
#define minWord32     0x80000000L
#define maxWord32     0x7fffffffL
typedef unsigned long UWord32;
#define minUWord32    0
#define maxUWord32    0xffffffffL


/*
 ********* define floating point type & constants
 */
/* use "#if 0" below if Float should be double;
   use "#if 1" below if Float should be float
 */
#if 0
typedef float Float;
#define maxFloat      FLT_MAX
#define minFloat      FLT_MIN
#else
typedef double Float;
#define maxFloat      DBL_MAX
#define minFloat      DBL_MIN
#endif

/*
 ********* define complex type
 */
typedef struct {
  Float r;  /* real      part */
  Float i;  /* imaginary part */
} CPX;

/*
 ********* define DPF(double precision format) type
 */
typedef struct {
  Word16  hi;
  UWord16 lo;
} DPF;

/*
 ********* define boolean type
 */

#define false 0
#define true 1

#ifndef	NULL
#define	NULL	((void *)0)
#endif


#endif /* TYPEDEF_H */

