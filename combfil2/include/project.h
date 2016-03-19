//
// FILENAME : project.h
// This file defines global constants
//

#ifndef	_PROJECT_H
#define	_PROJECT_H

#include "tistdtypes.h"

/***************************************************************************/
/* Function definitions                                                    */
/***************************************************************************/

void aic3204_init(Uint8 rate, Uint8 micGain);
void TIMER_init(void);
void INTR_init(void);
void DMA_audio_init(Uint16 size);
void PLL_init(Uint32 clock);

void audioProcessing(void);

#endif	// _PROJECT_H
// End of project.h

