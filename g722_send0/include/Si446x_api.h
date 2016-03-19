#ifndef SI446X_API_H_
#define SI446X_API_H_

#include "Si446x_B1_defs.h"

//++++++++++++++++++++++++++++++
// Define capacitor bank value
//++++++++++++++++++++++++++++++
#define CAP_BANK_VALUE    0x4A    // Capacitor bank value for adjusting the XTAL frequency
                                  // Note that it may vary from test card to test card

//+++++++++++++++++++++
//  TestCard selection
//+++++++++++++++++++++

#define EZRP_NEXT_TestCard TestCard_4463_TCE20B915


//++++++++++++++++++++++++
//  TestCard definitions
//++++++++++++++++++++++++

#define TestCard_4463_TSQ20B169  0
#define TestCard_4463_TSQ20D169  1
#define TestCard_4463_TSQ27F169  2
#define TestCard_4461_TCE14D434  3
#define TestCard_4460_TCE10D434  4
#define TestCard_4463_TCE20B460  5
#define TestCard_4463_TCE20C460  6
#define TestCard_4460_TSC10D868  7
#define TestCard_4460_TCE10D868  8
#define TestCard_4461_TSC14D868  9
#define TestCard_4461_TCE16D868 10
#define TestCard_4463_TCE20B868 11
#define TestCard_4463_TCE20C868 12
#define TestCard_4463_TCE27F868 13
#define TestCard_4463_TCE20B915 14
#define TestCard_4463_TCE20C915 15
#define TestCard_4463_TCE30E915 16
#define TestCard_4464_TCE20B420 17

//+++++++++++++++++++++
//  TestCard settings
//+++++++++++++++++++++

#if EZRP_NEXT_TestCard == TestCard_4463_TCE20C460
  #define ONE_SMA_WITH_RF_SWITCH
#elif EZRP_NEXT_TestCard == TestCard_4463_TCE20C868
  #define ONE_SMA_WITH_RF_SWITCH
#elif EZRP_NEXT_TestCard == TestCard_4463_TCE20C915
  #define ONE_SMA_WITH_RF_SWITCH
#endif

//++++++++++++++++++++++++++++++++++++
//  Modulations for Si446x devices
//++++++++++++++++++++++++++++++++++++

#define MOD_CW    0
#define MOD_OOK   1
#define MOD_2FSK  2
#define MOD_2GFSK 3
#define MOD_4FSK  4
#define MOD_4GFSK 5

//++++++++++++++++++++++++++
//  Communication settings
//++++++++++++++++++++++++++

#define MAX_CTS_RETRY					2500
#define NIRQ_TIMEOUT					10000

void Si446x_Init(void);
void SendPacket(Uint8 length, Uint8 *buff);
void ReceivePacket(Uint8 *buff);
void bApi_Set_Receive(void);
void bApi_Set_Send(Uint16 length);

#endif /* SI446X_API_H_ */

