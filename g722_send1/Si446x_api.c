/* Includes ------------------------------------------------------------------*/
#include "ezdsp5535_spi.h"
#include "Si446x_api.h"
#include "modem_params.h"

Uint8 abApi_Write[16];		// Write buffer for API communication
Uint8 abApi_Read[16];		// Read buffer for API communication

#define	MAX_TRX_LENGTH			62

Uint8 PaSettings[][5] = {
    /* PA_MODE,  PA_PWR_LVL,  PA_BIAS_CLKDUTY,  PA_TC_FSK,  PA_TC_OOK */
    {/*4463_TSQ20B169*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TSQ20D169*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TSQ27F169*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4461_TCE14D434*/0x20,0x21,0x00,0x5D,0x14},
    {/*4460_TCE10D434*/0x18,0x1A,0xC0,0x5D,0x14},
    {/*4463_TCE20B460*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TCE20C460*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4460_TSC10D868*/0x19,0x40,0xE8,0x5D,0x14},
    {/*4460_TCE10D868*/0x18,0x18,0xC0,0x5D,0x14},
    {/*4461_TSC14D868*/0x21,0x64,0x2C,0x5D,0x14},
    {/*4461_TCE16D868*/0x20,0x41,0x00,0x5D,0x14},
    {/*4463_TCE20B868*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TCE20C868*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TCE27F868*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TCE20B915*/0x08,0x20,0x00,0x5D,0x14},  //0x08,0x7F,0x00,0x5D,0x14
    {/*4463_TCE20C915*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4463_TCE30E915*/0x08,0x7F,0x00,0x5D,0x14},
    {/*4464_TCE20B420*/0x08,0x7F,0x00,0x5D,0x14}
};

Uint8 ModemTrx1[] = {7, MODEM_MOD_TYPE_7};
Uint8 ModemTrx2[] = {5, MODEM_CLKGEN_BAND_5};
Uint8 ModemTrx3[] = {11, SYNTH_PFDCP_CPFF_11};
Uint8 ModemTrx4[] = {12, FREQ_CONTROL_INTE_12};

Uint8 ModemRx1[] = {11, MODEM_MDM_CTRL_11};
Uint8 ModemRx2[] = {14, MODEM_BCR_OSR_1_14};
Uint8 ModemRx3[] = {12, MODEM_AFC_GEAR_12};
Uint8 ModemRx4[] = {5, MODEM_AGC_CONTRL_5};
Uint8 ModemRx4_1[] = {7, MODEM_AGC_WINDOW_SIZE_7};
Uint8 ModemRx5[] = {9, MODEM_FSK4_GAIN1_9};
Uint8 ModemRx6[] = {8, MODEM_OOK_PDTC_8};
Uint8 ModemRx7[] = {8, MODEM_RAW_SEARCH_8};
Uint8 ModemRx8[] = {6, MODEM_ANT_DIV_MODE_6};
Uint8 ModemRx9[] = {13, MODEM_CHFLT_RX1_CHFLT_COE13_7_0_13};
Uint8 ModemRx10[] = {13, MODEM_CHFLT_RX1_CHFLT_COE4_7_0_13};
Uint8 ModemRx11[] = {13, MODEM_CHFLT_RX2_CHFLT_COE13_7_0_13};
Uint8 ModemRx12[] = {13, MODEM_CHFLT_RX2_CHFLT_COE4_7_0_13};

Uint8 ModemTx1[] = {14, MODEM_DATA_RATE_2_14};
Uint8 ModemTx2[] = {5, MODEM_TX_RAMP_DELAY_5};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + FUNCTION NAME:  void vSetPAMode(PA_OPTIONS PaMode)
  + DESCRIPTION:    Set the PA operating mode
  + INPUT:   PaMode:
  +          TestCard_4463_TSQ20B169
  +          TestCard_4463_TSQ20D169
  +          TestCard_4463_TSQ27F169
  +          TestCard_4461_TCE14D434
  +          TestCard_4460_TCE10D434
  +          TestCard_4463_TCE20B460
  +          TestCard_4463_TCE20C460
  +          TestCard_4460_TSC10D868
  +          TestCard_4460_TCE10D868
  +          TestCard_4461_TSC14D868
  +          TestCard_4461_TCE16D868
  +          TestCard_4463_TCE20B868
  +          TestCard_4463_TCE20C868
  +          TestCard_4463_TCE27F868
  +          TestCard_4463_TCE20B915
  +          TestCard_4463_TCE20C915
  +          TestCard_4463_TCE30E915
  +          TestCard_4464_TCE20B420
  +
  + RETURN:         None
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void SetPAMode(Uint8 bPaMode, Uint8 bModType)
{
	abApi_Write[0] = CMD_SET_PROPERTY;
	abApi_Write[1] = PROP_PA_GROUP;
	abApi_Write[2] = 4;
	abApi_Write[3] = PROP_PA_MODE;
	abApi_Write[4] = PaSettings[bPaMode][0];
	abApi_Write[5] = PaSettings[bPaMode][1];
	abApi_Write[6] = PaSettings[bPaMode][2];
	if (bModType == MOD_OOK)
		abApi_Write[7] = PaSettings[bPaMode][4];
	else
		abApi_Write[7] = PaSettings[bPaMode][3];

	SpiSendCommand(8, abApi_Write);
}

void bApi_Set_Receive(void)     //将系统设置到接收状态
{
	// Start Rx         数据包接收完成后将产生 NIRQ 中断
	abApi_Write[0] = CMD_START_RX;			// Use Rx Start command
	abApi_Write[1] = 0x00;					// Set channel number
	abApi_Write[2] = 0x00;					// Ready state after Rx, start Rx immediately
	abApi_Write[3] = 0x00;					// Packet length [15:8]
	abApi_Write[4] = 0x00;					// Packet length [7:0]
	abApi_Write[5] = 0x00;					// No change if Rx timeout
	abApi_Write[6] = 0x03;					// Ready state after Rx valid state
	// 05 TX tune State ; 03 ready state
	// 接收完后用于减少电流用03,需要较高的响应速度用05
	abApi_Write[7] = 0x03;					// Ready state after Rx invalid state
	SpiSendCommand(8, abApi_Write);
}

void bApi_Set_Send(Uint16 length)     //将系统设置到发射状态
{
	// Set TX packet length
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_PKT_GROUP;			// Select property group
	abApi_Write[2] = 1;							// Number of properties to be written
	abApi_Write[3] = PROP_PKT_FIELD_1_LENGTH_7_0;	// Specify first property
	abApi_Write[4] = length;					// Field length (variable packet length info)
	SpiSendCommand(5, abApi_Write);

	// Start Tx
	abApi_Write[0] = CMD_START_TX;				// Use Tx Start command
	abApi_Write[1] = 0;							// Set channel number
	abApi_Write[2] = 0x30;						// Ready state after Tx, start Tx immediately  发送完后进入：接收状态80,RX TUNE 60,READY 30
	abApi_Write[3] = 0x00;						// Packet length [15:8]
	abApi_Write[4] = 0x00;						// Packet length [7:0]
	SpiSendCommand(5, abApi_Write);

//	SpiGetResponse(CMD_FRR_C_READ, 1, abApi_Read);
	// clear pending interrupts
	SpiWriteByte(CMD_GET_INT_STATUS, 0);
//	SpiSendCommand(4, abApi_Write);
//	SpiGetResponse(CMD_READ_CMD_BUFF, 9, abApi_Read);	 // Make sure that CTS is ready then get the response
												//发送完成后会响应中断
}

void ReceivePacket(Uint8 *buff)
{
	// Get length of the packet
//	abApi_Write[0] = CMD_FIFO_INFO;			// get the packet length information
//	SpiSendCommand(1, abApi_Write);
	SpiWriteByte(CMD_FIFO_INFO, 0);
	SpiGetResponse(CMD_READ_CMD_BUFF, 3, abApi_Read);
	// Read out the payload after the length
	SpiReadRxDataBuffer(abApi_Read[1], buff);

  // Provides access to transmit and receive fifo counts and reset, clear RXFIFO  
	SpiWriteByte(CMD_FIFO_INFO, 3);			// Use CMD_FIFO_INFO command, clear FIFO

	// clear pending interrupts
	SpiWriteByte(CMD_GET_INT_STATUS, 0);
//	SpiSendCommand(4, abApi_Write);
//	SpiGetResponse(CMD_READ_CMD_BUFF, 9, abApi_Read);	// Make sure that CTS is ready then get the response

	bApi_Set_Receive();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + FUNCTION NAME:  Uint8 SendPacket(Uint8 bLength, Uint8 *abPayload)
  + DESCRIPTION:    Send a packet
  + INPUT:          Payload to be transmitted and its length
  + RETURN:         Zero
  + NOTES:          None
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void SendPacket(Uint8 length, Uint8 *buff)
{
	SpiWriteByte(CMD_CHANGE_STATE, 5);			// Use change state command, go to ready mode

	SpiWriteTxDataBuffer(length, buff);			// Write data to Tx FIFO

	// Set TX packet length
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_PKT_GROUP;			// Select property group
	abApi_Write[2] = 1;							// Number of properties to be written
	abApi_Write[3] = PROP_PKT_FIELD_1_LENGTH_7_0;	// Specify first property
	abApi_Write[4] = length;					// Field length (variable packet length info)
	SpiSendCommand(5, abApi_Write);

	// Start Tx
	abApi_Write[0] = CMD_START_TX;				// Use Tx Start command
	abApi_Write[1] = 0;							// Set channel number
	abApi_Write[2] = 0x30;						// Ready state after Tx, start Tx immediately  发送完后进入：接收状态80,RX TUNE 60,READY 30
	abApi_Write[3] = 0x00;						// Packet length [15:8]
	abApi_Write[4] = 0x00;						// Packet length [7:0]
	SpiSendCommand(5, abApi_Write);
/*
	// Read ITs, clear pending ones
	abApi_Write[0] = CMD_GET_INT_STATUS;		// Use interrupt status command
	abApi_Write[1] = 0;							// Clear PH_CLR_PEND
	abApi_Write[2] = 0;							// Clear MODEM_CLR_PEND
	abApi_Write[3] = 0;							// Clear CHIP_CLR_PEND
	SpiSendCommand(4, abApi_Write);
	SpiGetResponse(CMD_READ_CMD_BUFF, 9, abApi_Read);	 // Make sure that CTS is ready then get the response
												//发送完成后会响应中断
*/
}

void Si446x_Init(void)
{
	abApi_Write[0] = CMD_POWER_UP;
	abApi_Write[1] = 0x01;
	abApi_Write[2] = 0x00;
	SpiSendCommand(3, abApi_Write);

	abApi_Write[0] = CMD_PART_INFO;
	SpiSendCommand(1, abApi_Write);
	SpiGetResponse(CMD_READ_CMD_BUFF, 9, abApi_Read);

	abApi_Write[0] = CMD_FUNC_INFO;
	SpiSendCommand(1, abApi_Write);
	SpiGetResponse(CMD_READ_CMD_BUFF, 7, abApi_Read);

	// Read ITs, clear pending ones
	abApi_Write[0] = CMD_GET_INT_STATUS;		// Use interrupt status command
	abApi_Write[1] = 0;							// Clear PH_CLR_PEND
	abApi_Write[2] = 0;							// Clear MODEM_CLR_PEND
	abApi_Write[3] = 0;							// Clear CHIP_CLR_PEND
	SpiSendCommand(4, abApi_Write);
	SpiGetResponse(CMD_READ_CMD_BUFF, 9, abApi_Read);

	// Set TRX parameters of the radio IC;
	//data retrieved from the WDS-generated modem_params.h
	SpiSendCommand(ModemTrx1[0], (Uint8 *)&ModemTrx1[1]);
	SpiSendCommand(ModemTrx2[0], (Uint8 *)&ModemTrx2[1]);
	SpiSendCommand(ModemTrx3[0], (Uint8 *)&ModemTrx3[1]);
	SpiSendCommand(ModemTrx4[0], (Uint8 *)&ModemTrx4[1]);

	// Set Rx parameters of the radio IC
	SpiSendCommand(ModemRx1[0], (Uint8 *)&ModemRx1[1]);
	SpiSendCommand(ModemRx2[0], (Uint8 *)&ModemRx2[1]);
	SpiSendCommand(ModemRx3[0], (Uint8 *)&ModemRx3[1]);
	SpiSendCommand(ModemRx4[0], (Uint8 *)&ModemRx4[1]);
	SpiSendCommand(ModemRx4_1[0], (Uint8 *)&ModemRx4_1[1]);
	SpiSendCommand(ModemRx5[0], (Uint8 *)&ModemRx5[1]);
	SpiSendCommand(ModemRx6[0], (Uint8 *)&ModemRx6[1]);
	SpiSendCommand(ModemRx7[0], (Uint8 *)&ModemRx7[1]);
	SpiSendCommand(ModemRx8[0], (Uint8 *)&ModemRx8[1]);
	SpiSendCommand(ModemRx9[0], (Uint8 *)&ModemRx9[1]);
	SpiSendCommand(ModemRx10[0], (Uint8 *)&ModemRx10[1]);
	SpiSendCommand(ModemRx11[0], (Uint8 *)&ModemRx11[1]);
	SpiSendCommand(ModemRx12[0], (Uint8 *)&ModemRx12[1]);

	// Set TX parameters of the radio IC
	SpiSendCommand(ModemTx1[0], (Uint8 *)&ModemTx1[1]);
	SpiSendCommand(ModemTx2[0], (Uint8 *)&ModemTx2[1]);

	// Configuring PA mode, select proper test card and modulation
	SetPAMode(EZRP_NEXT_TestCard, ModemTrx1[5]);

	// Enable the proper interrupts
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_INT_CTL_GROUP;		// Select property group
	abApi_Write[2] = 4;							// Number of properties to be written
	abApi_Write[3] = PROP_INT_CTL_ENABLE;		// Specify property
	abApi_Write[4] = 0x01;						// INT_CTL: PH interrupt enable
	abApi_Write[5] = 0x30;						// INT_CTL_PH: PH PACKET_SENT, PACKET_RX, CRC2_ERR enabled
	abApi_Write[6] = 0x00;						// INT_CTL_MODEM: -
	abApi_Write[7] = 0x00;						// INT_CTL_CHIP_EN: -
	SpiSendCommand(8, abApi_Write);

	// Configure Fast response registers
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_FRR_CTL_GROUP;		// Select property group
	abApi_Write[2] = 4;							// Number of properties to be written
	abApi_Write[3] = PROP_FRR_CTL_A_MODE;		// Specify fast response registers
	abApi_Write[4] = 0x04;						// FRR A: PH IT pending
	abApi_Write[5] = 0x06;						// FRR B: Modem IT pending
	abApi_Write[6] = 0x0A;						// FRR C: Latched RSSI
	abApi_Write[7] = 0x00;						// FRR D: disabled
	SpiSendCommand(8, abApi_Write);

	// Set packet content
	// Set preamble length - Rx
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_PREAMBLE_GROUP;		// Select property group
	abApi_Write[2] = 1;							// Number of properties to be written
	abApi_Write[3] = PROP_PREAMBLE_CONFIG_STD_1;	// Specify property
	abApi_Write[4] = 20;						// 20 bits preamble detection threshold
	SpiSendCommand(5, abApi_Write);

	// Set preamble length - Tx
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_PREAMBLE_GROUP;		// Select property group
	abApi_Write[2] = 1;							// Number of properties to be written
	abApi_Write[3] = PROP_PREAMBLE_TX_LENGTH;	// Specify property
	abApi_Write[4] = 0x05;						// 5 bytes Tx preamble
	SpiSendCommand(5, abApi_Write);

	// Set preamble pattern
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PREAMBLE_GROUP;     // Select property group
	abApi_Write[2] = 1;               // Number of properties to be written
	abApi_Write[3] = PROP_PREAMBLE_CONFIG;      // Specify property
	abApi_Write[4] = 0x31;              // Use `1010` pattern, length defined in bytes
	SpiSendCommand(5, abApi_Write);

	// Set sync word
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_SYNC_GROUP;       // Select property group

	abApi_Write[2] = 3;               // Number of properties to be written
	abApi_Write[3] = PROP_SYNC_CONFIG;        // Specify property
	abApi_Write[4] = 0x01;              // SYNC_CONFIG: 2 bytes sync word
	abApi_Write[5] = 0xB4;              // SYNC_BITS_31_24: 1st sync byte: 0x2D; NOTE: LSB transmitted first!
	abApi_Write[6] = 0x2B;              // SYNC_BITS_23_16: 2nd sync byte: 0xD4; NOTE: LSB transmitted first!
	SpiSendCommand(7, abApi_Write);

	// General packet config (set bit order)
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PKT_GROUP;        // Select property group
	abApi_Write[2] = 1;               // Number of properties to be written
	abApi_Write[3] = PROP_PKT_CONFIG1;        // Specify property
	abApi_Write[4] = 0x80;              // Separate RX and TX FIELD properties are used, payload data goes MSB first
	SpiSendCommand(5, abApi_Write);

	// Set variable packet length
	abApi_Write[0] = CMD_SET_PROPERTY;			// Use property command
	abApi_Write[1] = PROP_PKT_GROUP;			// Select property group
	abApi_Write[2] = 3;							// Number of properties to be written
	abApi_Write[3] = PROP_PKT_LEN;				// Specify property
	abApi_Write[4] = 0x0A;						// PKT_LEN: length is put in the Rx FIFO, FIELD 2 is used for the payload (with variable length)
	abApi_Write[5] = 0x01;						// PKT_LEN_FIELD_SOURCE: FIELD 1 is used for the length information
	abApi_Write[6] = 0x00;						// PKT_LEN_ADJUST: no adjustment (FIELD 1 determines the actual payload length)
	SpiSendCommand(7, abApi_Write);

	// Set packet fields for Tx (one field is used)
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PKT_GROUP;        // Select property group
	abApi_Write[2] = 4;               // Number of properties to be written
	abApi_Write[3] = PROP_PKT_FIELD_1_LENGTH_12_8;  // Specify first property
	abApi_Write[4] = 0x00;              // PKT_FIELD_1_LENGTH_12_8: defined later (under bSendPacket() )
	abApi_Write[5] = 0x00;              // PKT_FIELD_1_LENGTH_7_0: defined later (under bSendPacket() )
	abApi_Write[6] = 0x00;              // PKT_FIELD_1_CONFIG : No 4(G)FSK/Whitening/Manchester coding for this field
	abApi_Write[7] = 0xA2;              // PKT_FIELD_1_CRC_CONFIG: Start CRC calc. from this field, send CRC at the end of the field
	SpiSendCommand(8, abApi_Write);

	// Set packet fields for Rx (two fields are used)
	// FIELD1 is fixed, 1byte length, used for PKT_LEN
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PKT_GROUP;        // Select property group
	abApi_Write[2] = 4;               // Number of properties to be written
	abApi_Write[3] = PROP_PKT_RX_FIELD_1_LENGTH_12_8; // Specify first property
	abApi_Write[4] = 0x00;              // PKT_RX_FIELD_1_LENGTH_12_8: 1 byte (containing variable packet length info)
	abApi_Write[5] = 0x01;              // PKT_RX_FIELD_1_LENGTH_7_0: 1 byte (containing variable packet length info)
	abApi_Write[6] = 0x00;              // PKT_RX_FIELD_1_CONFIG: No 4(G)FSK/Whitening/Manchester coding for this field
	abApi_Write[7] = 0x82;              // PKT_RX_FIELD_1_CRC_CONFIG: Start CRC calc. from this field, enable CRC calc.
	SpiSendCommand(8, abApi_Write);

	// FIELD2 is variable length, contains the actual payload
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PKT_GROUP;        // Select property group
	abApi_Write[2] = 4;               // Number of properties to be written
	abApi_Write[3] = PROP_PKT_RX_FIELD_2_LENGTH_12_8; // Specify first property
	abApi_Write[4] = 0x00;              // PKT_RX_FIELD_2_LENGTH_12_8: max. field length (variable packet length)
	abApi_Write[5] = MAX_TRX_LENGTH;     // PKT_RX_FIELD_2_LENGTH_7_0: max. field length (variable packet length)
	abApi_Write[6] = 0x00;              // PKT_RX_FIELD_2_CONFIG: No 4(G)FSK/Whitening/Manchester coding for this field
	abApi_Write[7] = 0x0A;              // PKT_RX_FIELD_2_CRC_CONFIG: check CRC at the end, enable CRC calc.
	SpiSendCommand(8, abApi_Write);

	// Configure CRC polynomial and seed
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_PKT_GROUP;        // Select property group
	abApi_Write[2] = 1;               // Number of properties to be written
	abApi_Write[3] = PROP_PKT_CRC_CONFIG;     // Specify property
	abApi_Write[4] = 0x83;              // CRC seed: all `1`s, poly: No. 3, 16bit, Baicheva-16
	SpiSendCommand(5, abApi_Write);

	// Set RSSI latch to sync word
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_MODEM_GROUP;        // Select property group
	abApi_Write[2] = 1;               // Number of properties to be written
	abApi_Write[3] = PROP_MODEM_RSSI_CONTROL;   // Specify property
	abApi_Write[4] = 0x12;              // RSSI average over 4 bits, latches at sync word detect
	SpiSendCommand(5, abApi_Write);

/*	// Configure the GPIOs
	abApi_Write[0] = CMD_GPIO_PIN_CFG;        // Use GPIO pin configuration command
#ifdef ONE_SMA_WITH_RF_SWITCH
	// If RF switch is used
	// Select Tx state to GPIO2, Rx state to GPIO0
	abApi_Write[1] = 0x21;              // Configure GPIO0 as Rx state
	abApi_Write[2] = 0x13;              // Configure GPIO1 as Tx data
	abApi_Write[3] = 0x20;              // Configure GPIO2 as Tx state 
	abApi_Write[4] = 0x10;              // Configure GPIO3 as Tx data CLK
#else
	abApi_Write[1] = 0x10;              // Configure GPIO0 as Tx data CLK
	abApi_Write[2] = 0x13;              // Configure GPIO1 as Tx data
	abApi_Write[3] = 0x20;              // Configure GPIO2 as Tx state
	abApi_Write[4] = 0x21;              // Configure GPIO3 as Rx state
#endif
	SpiSendCommand(5,abApi_Write);
*/
	// Adjust XTAL clock frequency
	abApi_Write[0] = CMD_SET_PROPERTY;        // Use property command
	abApi_Write[1] = PROP_GLOBAL_GROUP;       // Select property group
	abApi_Write[2] = 1;               // Number of properties to be written
	abApi_Write[3] = PROP_GLOBAL_XO_TUNE;     // Specify property
	abApi_Write[4] = CAP_BANK_VALUE;        // Set cap bank value to adjust XTAL clock frequency
	SpiSendCommand(5, abApi_Write);
}

