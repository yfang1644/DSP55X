#include "tistdtypes.h"

#define MAX_BUFFER_SIZE 1280

/***************************************************************************/
/* Function definitions                                                    */
/***************************************************************************/

void DSP_zero(Int16 *buf, Int16 cnt);
Int16 DSP_scale(Int16 *dst, const Int32 *src, Uint16 cnt);
/*****************************************************
 * prototype in main
 ****************************************************/

void TIMER_init(void);
void INTR_init(void);
void PLL_init(Uint32 clock);

int DMA_audio_init(Uint16 framesize);
int File_init(char *filename);


