/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file usb.c
 *
 *  @brief USB functional layer dma mode example source file
 *
 *
 * \page    page17  CSL USB EXAMPLE DOCUMENTATION
 *
 * \section USB3   USB EXAMPLE3 - DMA MODE TEST
 *
 * \subsection USB3x    TEST DESCRIPTION:
 * 		This test is to verify the operation of the CSL USB module.This test
 * runs in DMA mode. USB controller is having DMA module internal to it.
 * This DMA can be used to exchange the data between USB FIFO and the CPU memory.
 * USB interrupt are used to indicate the arrival of request from the host.
 * USB interrupts are configured and ISR is  registered using CSL INTC module.
 * After initializing and configuring the USB module test waits on a while loop.
 * When there is any request from the USB host application USB ISR is triggered
 * and the requested operation is performed inside the ISR. Inside the ISR FIFO
 * read and write operations are performed using CPPI DMA module.
 *
 * This USB test can be verified by a host USB tool(c5505usb_ep_diag.exe) which can
 * send or receive 64 bytes of data to the USB device. This tool should be
 * installed on the host PC. This tool requires Jungo USB driver which can be
 * downloaded from the link www.jungo.com. inf file should be installed for
 * the C5505/C5515 USB device using driver wizard of the windriver.
 * This installation is required only when running this example for the first
 * time.
 *
 * NOTE: IN THE CURRENT CODE EP1 IS CONFIGURED FOR IN TRANSACTIONS(COMMNAD 4-->1, 0x81)
 * AND EP2 IS CONFIGURED FOR OUT TRANSACTIONS(COMMAND 4-->2, 0x02). APPROPRIATE COMMANDS
 * SHOULD BE SENT FROM THE HOST APPLICATION, OTHERWISE TEST WILL NOT WORK.
 * STARTS THE DATA COMMANDS WITH DATA READ FOR PROPER SYNCHRONIZATION.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection USB3y    TEST PROCEDURE:
 * @li Open the CCS and connect the target (C5505/C5515 EVM)
 * @li Open, build and load the USB program to the target
 * @li Set the PLL frequency to 100MHz
 * @li Run the program loaded on to the target
 * @li Open the Windriver driver wizard and install the inf file for the USB
 *     device (Only when running for first time)
 * @li Run the c5505usb_ep_diag.exe application. It displays following message
 * @verbatim

       DeviceAttach: received and accepted attach for vendor id 0x451, product id 0x901
		0, interface 0, device handle 0x00392AD8

		Main Menu (active Dev/Prod/Interface/Alt. Setting: 0x451/0x9010/0/0)
       ----------
       1. Display device configurations
       2. Change interface alternate setting
       3. Reset Pipe
       4. Read/Write from pipes
       6. Selective Suspend
       7. Refresh
       99. Exit
       Enter option:

   @endverbatim
 *
 * @li Numbers from 1 to 99 should be entered to execute the operation given
 *     against each number
 * @li For read and write operations command number should be selected depending
 *     on the end points configured for IN and OUT USB transfers. In the Current
 *     code EP1 is configured for IN transactions and EP2 is configured for OUT
 *     transactions. So 4-->2 is the command for write operation and 4-->1 is the
 *     command for read operation.
 * @li USB host application gives zero bytes of data for the first read. This
 *     is a known behavior.
 *     So the first command should be read followed by write-read... for the
 *     proper synchronization of host and target USB device.
 * @li Data transferred from the host will be copied to "usbDataBuffer".
 * @li Data in the "usbDataBuffer" will be sent to host when host sends a read
 *     command
 * @li For verifying the USB operations send write command from host and then
 *     read command. check whether the data sent by the host is transmitted back
 *     by the target or not.
 * @li During read/write operations data will be displayed by the host USB tool
 * @li USB device can be suspended using command 6. No operation will occur
 *     when USB is in suspended state
 * @li A wakeup signal can be given to USB using "STOP" button in the EVM.
 *     For verifying self wakeup send command 6 from c5505usb_ep_diag.exe which will
 *     suspend the USB device. After device suspension no data transfer will
 *     succeed. To take the USB device out of suspension press STOP key in
 *     the EVM keypad. This will generate a self wakeup signal.After USB wakeup
 *     data transfer will happen normally. It is recommanded to use the self
 *     wakeup only at higher clock frequencies - 75, 100 and 120MHz
 *
 * \subsection USB3z    TEST RESULT:
 * @li USB device should be detected by the host PC and should be accessible
 *     through the host application c5505usb_ep_diag.exe
 * @li Data written and data read should match in the host application
 * @li USB Wakeup signal should be triggred by the "STOP" button of C5505/C5515
 *     EVM key pad.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 24-Oct-2008 Created
 * 14-May-2009 Added DMA word swap work around for C5505 PG1.4
 * 06-Aug-2010 Updated
 * ============================================================================
 */

#include "ezdsp5535.h"
#include "g722record.h"
#include "csl_usb.h"
#include "usbAux.h"

#define CSL_USB_DATA_SIZE     (512)
#define CSL_USB_LRAM_SIZE     (256)
#define CSL_USB_MAX_CURRENT   (50)

extern Uint16 taskList;

extern CSL_UsbContext     gUsbContext;
extern CSL_UsbRegsOvly    usbRegisters;

pUsbEpHandle          hEpObjArray[CSL_USB_ENDPOINT_COUNT];
pUsbEpHandle          hEPx;
CSL_UsbEpObj          usbCtrlOutEpObj;
CSL_UsbEpObj          usbCtrlInEpObj;
CSL_UsbEpObj          usbBulkOutEpObj;
CSL_UsbEpObj          usbBulkInEpObj;
CSL_Status            status;
pUsbContext           pContext = &gUsbContext;
Uint16                devAddr;

#pragma DATA_SECTION(linking_ram0, ".buffer2")
Uint32 linking_ram0[CSL_USB_LRAM_SIZE];

/* USB queue manager only recognizes the lower 16 bit address for USB
   descriptors, i.e. 0x00001234 and 0xffff1234 will be treated as the same
   Allocate the descriptors such that their address will not exceed 16 bits
 */
#pragma DATA_SECTION(hpdtx, ".buffer1")
#pragma DATA_SECTION(hpdrx, ".buffer1")
CSL_UsbHostPktDescr hpdtx;
CSL_UsbHostPktDescr hpdrx;

#pragma DATA_SECTION(usbDataBuffer, ".buffer3")
Uint16    usbDataBuffer[CSL_USB_DATA_SIZE];

Uint16 deviceDesc[9] = {
	0x0112,		// bDeviceDescriptorType=1(defined by USB spec)// bLength = 18
	0x0200,		// bcdUSB USB Version 2.0 (L-Byte/H-Byte)
	0x0000,		// bDeviceSubclass// bDeviceClass
	0x4000,		// bMaxPacketSize 64 Bytes// bDeviceProtocol
	0x0451,		// idVendor(L-Byte/H-Byte) TI=0x0451
	0x5535,		// idProduct(L-Byte/H-Byte) = 5535
	0x0100,		// bcdDevice(L-Byte/H-Byte): device's release number = 1
	0x0201,		// iProduct String Index// iManufacturer String Index
	0x0103		// bNumberConfigurations// iSerialNumber String Index
	};

Uint16 cfgDesc[40] = {
	// configure descriptor
	0x0209,		// bDescriptorType = Config (constant)// bLength
	0x0020,		// wTotalLength(L/H)
	0x0101,		// bConfigValue// bNumInterfaces
	0xC001,		// bmAttributes; // iConfiguration
	0x0928,		// length = 9// MaxPower is 80mA
	// interface descriptor
	0x0004,		// type = IF; constant 4 for INTERFACE
	0x0200,		// bNumber of Endpoints = 1 for 5515// bAlternate Setting
	0x0000,		// bInterfaceSubClass// bInterfaceClass
	0x0000,		// iInterface// bInterfaceProtocol
	//endpoint 1 IN descriptor
	0x0507,		// bDescriptorType (Endpoint)// bLength
	0x0281,		// bmAttributes,  bEndpointAddress and direction
				// B1B0->transfer-type:
				// control=00; Iso=01; bulk=10; interrupt=11
	0x0200,		// wMaxPacketSize(L/H)=4
	0x0700,		// bLength// bInterval: Max latency
	//endpoint 2 OUT descriptor
	0x0205,		// bEndpointAddress and direction// bDescriptorType
	0x0002,		// wMaxPacketSize L// bmAttributes
				// B1B0->transfer-type:
				// control=00; Iso=01; bulk=10; interrupt=11
	0x0002,		// bInterval: Max latency// wMaxPacketSize H

	0x0507, 0x0282, 0x0200, 0x0700,			//endpoint 2 IN descriptor
	0x0205, 0x0002, 0x0002, 				//endpoint 2 OUT descriptor
	0x0507, 0x0283, 0x0200, 0x0700,			//endpoint 3 IN descriptor
	0x0305, 0x0002, 0x0002,					//endpoint 3 OUT descriptor
	0x0507, 0x0284, 0x0200, 0x0700,			//endpoint 4 IN descriptor
	0x0405, 0x0002, 0x0002					//endpoint 4 OUT descriptor
	};

Uint16  strDesc[4][32] = {
	// string 0 English-USA
	{0x0304, 0x0409},
	// string 1(Manufacturer) "NJU & 841"
	{0x0314, 0x4e, 0x4a, 0x55, 0x20, 0x26, 0x20, 0x38, 0x34, 0x31},
	// string 2(Product) "g722.1 recorder"
	{0x0320, 0x67, 0x37, 0x32, 0x32, 0x2e, 0x31, 0x20, 0x72, 0x65,
             0x63, 0x6f, 0x72, 0x64, 0x65, 0x72},
	// string 3(SerialNumber) "00001"
	{0x030C, 0x0030, 0x0030, 0x0030, 0x0030, 0x0031}
	};


/**
 *  \brief  USB interrupt service routine
 *
 *  \param  None
 *
 *  \return None
 */
void interrupt usb_isr(void)
{
	taskList |= USBPROCESSING;
}

/**
 *  \brief  Start transfer call back function
 *
 *  \param  vpContext - USB context structure
 *  \param  vpeps     - End point status structure pointer
 *
 *  \return CSL_Status
 */
CSL_Status CSL_startTransferCallback(void    *vpContext,
                                     void    *vpeps)
{
	pUsbContext      pContext;
	pUsbEpStatus     peps;
	pUsbTransfer     pTransfer;
	CSL_Status       status;

	status = CSL_SOK;

	pContext  = (pUsbContext)vpContext;
	peps      = (pUsbEpStatus)vpeps;

	if(!pContext->fMUSBIsReady)
		return(CSL_ESYS_INVPARAMS);

	/* The endpoint should be initialized */
	if(!peps->fInitialized)
		return(CSL_ESYS_INVPARAMS);

    pTransfer = peps->pTransfer;
    pTransfer->fComplete = FALSE;

	if(pTransfer->dwFlags == CSL_USB_OUT_TRANSFER) {
		if(peps->dwEndpoint == CSL_USB_EP0)
			status = USB_processEP0Out(pContext);
		else
			status = USB_handleRx(pContext, CSL_USB_EP2);
	} else if(pTransfer->dwFlags == CSL_USB_IN_TRANSFER) {
		if(peps->dwEndpoint == CSL_USB_EP0)
			status = USB_processEP0In(pContext);
		else
			status = USB_handleTx(pContext, CSL_USB_EP1);
	} else {
		status = CSL_ESYS_INVPARAMS;
	}

	return(status);
}

/**
 *  \brief  USB DMA test main function
 *
 *  \param  None
 *
 *  \return None
 */

Int16 BoardUSB_init(void)
{
	Uint16    eventMask;
	CSL_UsbConfig         usbConfig;

	usbConfig.opMode             = CSL_USB_OPMODE_DMA;
    usbConfig.devNum             = CSL_USB0;
	usbConfig.maxCurrent         = CSL_USB_MAX_CURRENT;
	usbConfig.appSuspendCallBack = (CSL_USB_APP_CALLBACK)NULL;
	usbConfig.appWakeupCallBack  = (CSL_USB_APP_CALLBACK)NULL;
	usbConfig.startTransferCallback    = CSL_startTransferCallback;
	usbConfig.completeTransferCallback = NULL;

	hEpObjArray[0] = &usbCtrlOutEpObj;
	hEpObjArray[1] = &usbCtrlInEpObj;
	hEpObjArray[2] = &usbBulkOutEpObj;
	hEpObjArray[3] = &usbBulkInEpObj;

	CSL_CPU_REGS->IER1 |= 0x0010;		// enable USB interrupt

	/* Initialize the USB module */
	status = USB_init(&usbConfig);	// never fail

	/* Reset the USB device */
	status = USB_resetDev(CSL_USB0);	// never fail when CSL_USB0

	/* Initialize the Control Endpoint OUT 0 */
	eventMask = CSL_USB_EVENT_RESET |
				CSL_USB_EVENT_SETUP |
				CSL_USB_EVENT_SUSPEND |
				CSL_USB_EVENT_RESUME |
				CSL_USB_EVENT_EOT;

	status = USB_initEndptObj(CSL_USB0, hEpObjArray[0], CSL_USB_OUT_EP0,
							  CSL_USB_CTRL, CSL_USB_EP0_PACKET_SIZE,
							  eventMask, NULL);
	if(status != CSL_SOK)	// USB End point init failed
		return status;

	/* Initialize the Control Endpoint IN 0 */
	status = USB_initEndptObj(CSL_USB0, hEpObjArray[1], CSL_USB_IN_EP0,
							  CSL_USB_CTRL, CSL_USB_EP0_PACKET_SIZE,
							  CSL_USB_EVENT_EOT, NULL);
	if(status != CSL_SOK)	// USB End point init failed
		return status;

	/* Initialize the Bulk Endpoint IN 1 */
	eventMask = CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT;
	status = USB_initEndptObj(CSL_USB0, hEpObjArray[2], CSL_USB_IN_EP1,
							  CSL_USB_BULK, CSL_USB_EP1_PACKET_SIZE_HS,
							  eventMask, NULL);
	if(status != CSL_SOK)	// USB End point init failed
		return status;

	/* Initialize the Bulk Endpoint OUT 2 */
	status = USB_initEndptObj(CSL_USB0, hEpObjArray[3], CSL_USB_OUT_EP2,
							  CSL_USB_BULK, CSL_USB_EP2_PACKET_SIZE_HS,
							  CSL_USB_EVENT_EOT, NULL);
	if(status != CSL_SOK)	//  USB End point init failed
		return status;

	/* Set the parameters */
	USB_setParams(CSL_USB0, hEpObjArray, TRUE);
/*
	for(looper = 0; looper < CSL_USB_ENDPOINT_COUNT; looper++) {
		gUsbEpHandle[looper] = hEpObjArray[looper];
		if(hEpObjArray[looper] == NULL)
			break;
	}
*/

	/* Connect the USB device */
	CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_SOFTCONN, TRUE);

	return CSL_SOK;
}

void usbProcessing(void)
{
	static Bool	 usbDevDisconnect = TRUE;
	CSL_UsbSetupStruct    usbSetup;
	Uint16    saveIndex;
	Uint16    *strDescPtr;
	Uint16    endpt;
	Uint16    queRegVal;
	Uint16    queuePend1;
	int i;

	if(usbDevDisconnect == TRUE) {
		// wait for connecting
		if(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) ==
					CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID) {
			usbDevDisconnect = FALSE;
			// connectDev
			CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_SOFTCONN, TRUE);
		}
	}

	if((taskList & USBPROCESSING) == 0)    return;

	/* Read the masked interrupt status register */
	pContext->dwIntSourceL = usbRegisters->INTMASKEDR1;
	pContext->dwIntSourceH = usbRegisters->INTMASKEDR2;

	/* Reset interrupt */
	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESET) {
		usbRegisters->INDEX_TESTMODE &= 0x00ff;

		if(usbBulkOutEpObj.epNum == CSL_USB_IN_EP1)
			CSL_FINS(usbRegisters->INDEX_TESTMODE,
					 USB_INDEX_TESTMODE_EPSEL, CSL_USB_EP1);
		else
			CSL_FINS(usbRegisters->INDEX_TESTMODE,
					 USB_INDEX_TESTMODE_EPSEL, CSL_USB_EP2);

		CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				 USB_PERI_CSR0_INDX_RXPKTRDY, TRUE);

		USB_initQueueManager(&hpdtx, linking_ram0);
		USB_initDma();
		USB_confDmaRx(&hpdrx, CSL_USB_EP2_PACKET_SIZE_HS, usbDataBuffer);
		USB_confDmaTx(&hpdtx, CSL_USB_EP1_PACKET_SIZE_HS, usbDataBuffer);
		USB_dmaRxStart(CSL_USB_EP2);
	}

	/* Resume interrupt */
	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESUME) {
		pContext->remoteWkpStat = CSL_USB_TRUE;

		CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_RESUME, TRUE);
		/* Give 10 msecs delay before resetting resume bit */
		EZDSP5535_waitusec(10000);
		CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_RESUME, FALSE);
	}

	/* Check end point0 interrupts */
	if(pContext->dwIntSourceL & CSL_USB_TX_RX_INT_EP0) {
		saveIndex = usbRegisters->INDEX_TESTMODE;
		CSL_FINS(usbRegisters->INDEX_TESTMODE,
				 USB_INDEX_TESTMODE_EPSEL, CSL_USB_EP0);

		USB_getSetupPacket(CSL_USB0, &usbSetup, TRUE);

		if(usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK) {
			/* Service the RXPKTRDY after reading the FIFO */
			CSL_FINS(usbRegisters->PERI_CSR0_INDX,
					 USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);

			/* GET DESCRIPTOR Req */
			switch(usbSetup.bRequest) {
			/* zero data */
			case CSL_USB_SET_FEATURE:
				switch(usbSetup.wValue) {
				case CSL_USB_FEATURE_ENDPOINT_STALL:
					/* updated set and clear endpoint stall
					 * to work with logical endpoint num
					 */
					endpt = (usbSetup.wIndex) & 0xFF;
					hEPx = USB_epNumToHandle(CSL_USB0, endpt);
					if(!(USB_getEndptStall(hEPx, &status)))
						USB_stallEndpt(hEPx);
					break;

				case CSL_USB_FEATURE_REMOTE_WAKEUP:
					if(!pContext->remoteWkpStat)
						pContext->remoteWkpStat = CSL_USB_TRUE;
					break;

				default:
					break;
				}

				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
						 USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
						 USB_PERI_CSR0_INDX_DATAEND, TRUE);
				break;

			case CSL_USB_CLEAR_FEATURE:
				switch(usbSetup.wValue) {
				case CSL_USB_FEATURE_ENDPOINT_STALL:
					endpt = (usbSetup.wIndex) & 0xFF;
					hEPx = USB_epNumToHandle(CSL_USB0, endpt);
					if(USB_getEndptStall(hEPx, &status))
						USB_clearEndptStall(hEPx);
					break;

				case CSL_USB_FEATURE_REMOTE_WAKEUP:
					if(pContext->remoteWkpStat)
						pContext->remoteWkpStat = CSL_USB_FALSE;
					break;

				default:
					 break;
				}
				break;

			case CSL_USB_SET_CONFIGURATION:
			case CSL_USB_SET_INTERFACE:
				endpt = (usbSetup.wIndex) & 0xFF;
				hEPx = USB_epNumToHandle(CSL_USB0, endpt);
				USB_postTransaction(hEPx, 0, NULL, CSL_USB_IOFLAG_NONE);

				/* DataEnd + ServicedRxPktRdy */
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_DATAEND, TRUE);
				break;

			case CSL_USB_GET_DESCRIPTOR:
				switch(usbSetup.wValue >> 8) {
				case CSL_USB_DEVICE_DESCRIPTOR_TYPE:
					status = USB_postTransaction(hEpObjArray[1],
										deviceDesc[0]&0xFF, deviceDesc,
										CSL_USB_IN_TRANSFER);
					break;

				case CSL_USB_CONFIGURATION_DESCRIPTOR_TYPE:
					if(usbSetup.wLength == 0x0009)
						status = USB_postTransaction(hEpObjArray[1],
										9, cfgDesc,
										CSL_USB_IN_TRANSFER);
					else
						status = USB_postTransaction(hEpObjArray[1],
										cfgDesc[1], cfgDesc,
										CSL_USB_IN_TRANSFER);
					break;

				case CSL_USB_STRING_DESCRIPTOR_TYPE:
					i = usbSetup.wValue & 0xFF;
					if(i < 4) {
						strDescPtr = (Uint16 *)strDesc[i];
						status = USB_postTransaction(hEpObjArray[1],
										strDesc[i][0]&0xFF, strDescPtr,
										CSL_USB_IN_TRANSFER);
					}
					break;

				default:
					break;
				}

				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_TXPKTRDY, TRUE);
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_DATAEND, TRUE);
				break;

			case CSL_USB_SET_ADDRESS:
				devAddr = usbSetup.wValue;
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);
				CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				         USB_PERI_CSR0_INDX_DATAEND, TRUE);
				break;

			default:
				break;
			}
		} else {
			if(usbSetup.bRequest == 0x05) {
				CSL_FINS(usbRegisters->FADDR_POWER,
						 USB_FADDR_POWER_FUNCADDR, FALSE);
				CSL_FINS(usbRegisters->FADDR_POWER,
						 USB_FADDR_POWER_FUNCADDR, devAddr);
			}
		}

		usbRegisters->INDEX_TESTMODE  = saveIndex;
	}

	/* Check Data Out Ready */
	if(pContext->dwIntSourceL & (CSL_USB_RX_INT_EP1 |
								 CSL_USB_RX_INT_EP2 |
								 CSL_USB_RX_INT_EP3 |
								 CSL_USB_RX_INT_EP4)) {
/*		usbRegisters->QMQN[0].CTRL1D = ((((Uint32)&hpdrx) << 1) &
											CSL_USB_16BIT_REG_MASK) | 2;
		usbRegisters->QMQN[0].CTRL2D = ((((Uint32)&hpdrx) >> 15) &
											CSL_USB_16BIT_REG_MASK) | 8;
*/		USB_confDmaRx(&hpdrx, CSL_USB_EP2_PACKET_SIZE_HS, usbDataBuffer);
		USB_dmaRxStart(CSL_USB_EP2);
	}

	/* Check Data In Ready */
	if(pContext->dwIntSourceL & (CSL_USB_TX_INT_EP1 |
								 CSL_USB_TX_INT_EP2 |
								 CSL_USB_TX_INT_EP3 |
								 CSL_USB_TX_INT_EP4)) {
/*		usbRegisters->QMQN[16].CTRL1D = ((((Uint32)&hpdtx) << 1) &
											CSL_USB_16BIT_REG_MASK) | 2;
		usbRegisters->QMQN[16].CTRL2D = ((((Uint32)&hpdtx) >> 15) &
											CSL_USB_16BIT_REG_MASK) | 8;
*/		USB_confDmaTx(&hpdtx, CSL_USB_EP1_PACKET_SIZE_HS, usbDataBuffer);
		USB_dmaTxStart(CSL_USB_EP1);
	}

	/* Read queue pending register1 */
	queuePend1 = usbRegisters->PEND1;

	if (queuePend1 & 0x0400) {
		/* pop packet descriptor from queue */
		queRegVal = usbRegisters->QMQN[26].CTRL1D;;

		USB_dmaRxStop(CSL_USB_EP2);

/*		usbRegisters->QMQN[0].CTRL1D = ((((Uint32)&hpdrx) << 1) &
											CSL_USB_16BIT_REG_MASK) | 2;
		usbRegisters->QMQN[0].CTRL2D = ((((Uint32)&hpdrx) >> 15) &
											CSL_USB_16BIT_REG_MASK) | 8;
*/		USB_confDmaRx(&hpdrx, CSL_USB_EP2_PACKET_SIZE_HS, usbDataBuffer);
		USB_dmaRxStart(CSL_USB_EP2);
	} else if (queuePend1 & 0x100) {
        // pop the transmit completion queue (24)
        ///USB_dmaPopQue(24);
        ///tempData = usbRegisters->QMQNS[24].QSTATA;
		queRegVal = usbRegisters->QMQN[24].CTRL1D;
        USB_dmaTxStop(CSL_USB_EP1);
    }


	/* Connect interrupt */
	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVCONN)
		CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_SOFTCONN, TRUE);

	/* Disconnect interrupt */
	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVDISCONN) {
		CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_SOFTCONN, FALSE);
		usbDevDisconnect = TRUE;
	}

	/* Suspend interrupt */
	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SUSPEND)
		CSL_FINS(usbRegisters->FADDR_POWER, USB_FADDR_POWER_ENSUSPM, TRUE);

	/* Clear the interrupts */
	usbRegisters->INTCLRR1 = pContext->dwIntSourceL;
	usbRegisters->INTCLRR2 = pContext->dwIntSourceH;

	CSL_FINS(usbRegisters->EOIR, USB_EOIR_EOI_VECTOR, CSL_USB_EOIR_RESETVAL);

	taskList &= ~USBPROCESSING;
}

