
#ifndef _CSL_USBAUX_H_
#define _CSL_USBAUX_H_

#include "csl_usb.h"

/** @addtogroup CSL_USB_FUNCTION
 @{ */

/** ===========================================================================
 *   @n@b USB_swapWords
 *
 *   @b Description
 *   @n This function swaps words in a given buffer
 *
 *   @b Arguments
 *   @verbatim
            dataBuffer    Pointer to the buffer which words needs to be swapped
            dataLength    Length of data to be word swapped (In words)
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reset successful
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  Order of data in the given buffer
 *
 *   @b Example
 *   @verbatim
            #define USB_PACKET_SIZE (256)

            Uint16        usbBuf [USB_PACKET_SIZE];
            CSL_Status    status;

            status = USB_swapWords (usbBuf, USB_PACKET_SIZE);
     @endverbatim
 *  ===========================================================================
 */
CSL_Status USB_swapWords (Uint16    *dataBuffer,
                          Uint32    dataLength);

/** ============================================================================
 *   @n@b USB_initDma
 *
 *   @b Description
 *   @n Initializes the Dma
 *
 *   @b Arguments
 *   @verbatim
			None
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_init should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Initializes USB Dma
 *
 *   @b Modifies
 *   @n USB Dma registers
 *
 *   @b Example
 *   @verbatim
			CSL_UsbConfig      usbConfig;
			CSL_Status         status;

			usbConfig.maxCurrent = 50;
			.....
			.....
			status = USB_init(&usbConfig);
			....
			....
			USB_initDma();
     @endverbatim
 *  ============================================================================
 */
void USB_initDma(void);


/** ============================================================================
 *   @n@b USB_initQueueManager
 *
 *   @b Description
 *   @n Initializes the Dma queue manager.
 *
 *   @b Arguments
 *   @verbatim
 	        hpdtx         - Dma host packet transmit descriptor pointer
			pLinkingRam   - Pointer to linking RAM data buffer
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reset successful
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initDma should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Initializes queue manager
 *
 *   @b Modifies
 *   @n USB Queue manager registers
 *
 *   @b Example
 *   @verbatim
			CSL_UsbHostPacketDescriptor    hpdtx;

			USB_initDma();
			USB_initQueueManager(&hpdrx);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_initQueueManager(CSL_UsbHostPktDescr    *hpdtx,
                                Uint32                 *pLinkingRam);

/** ============================================================================
 *   @n@b USB_dmaPopQue
 *
 *   @b Description
 *   @n Function to popup the DMA queue. DMA descriptors will be removed from
 *      the queue.
 *
 *   @b Arguments
 *   @verbatim
 	        queueNum   - Queue number
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reset successful
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Popup the queue
 *
 *   @b Modifies
 *   @n USB Queue manager registers
 *
 *   @b Example
 *   @verbatim

			USB_dmaPopQue(24);
     @endverbatim
 *  ============================================================================
 */
void USB_dmaPopQue(Uint16 queueNum);

/** ============================================================================
 *   @n@b USB_dmaRxStart
 *
 *   @b Description
 *   @n Starts Dma Receive
 *
 *   @b Arguments
 *   @verbatim
 			dwEndpoint    - Endpoint number that is configured for Rx
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_confDmaRx should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Starts Dma data receive
 *
 *   @b Modifies
 *   @n Data buffer
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdrx;

			USB_initDma();
			USB_initQueueManager(&hpdrx);
			....
			....
			USB_confDmaRx(&hpdrx, CSL_USB_DATA_SIZE, usbDataBuf);
			....
			....
			USB_dmaRxStart();
     @endverbatim
 *  ============================================================================
 */
void USB_dmaRxStart(DWORD    dwEndpoint);

/** ============================================================================
 *   @n@b USB_confDmaRx
 *
 *   @b Description
 *   @n Configures Dma for Data receive
 *
 *   @b Arguments
 *   @verbatim
 	        hpdrx         - Dma host packet descriptor pointer
 	        packetsize    - Size of the packet to receive (In Bytes)
 	        pRxBuffer     - Receive Data Buffer pointer
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reset successful
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initDma and USB_initQueueManager should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Receives the data from an end point
 *
 *   @b Modifies
 *   @n USB Dma registers
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdrx;

			USB_initDma();
			USB_initQueueManager(&hpdrx);
			....
			....
			USB_confDmaRx(&hpdrx, CSL_USB_DATA_SIZE, usbDataBuf);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_confDmaRx(CSL_UsbHostPktDescr    *hpdrx,
                         Uint32                 packetsize,
                         Uint16                 *pRxBuffer);

/** ============================================================================
 *   @n@b USB_confDmaTx
 *
 *   @b Description
 *   @n Configures Dma for Data USB data transfer. For C5505 PG1.4 and earlier
 *      versions of the DSP, word swap is required for the data buffer before
 *      transferring the data. Parameter 'swapWords' provides facility to
 *      request for the word swap for the data buffer.
 *
 *   @b Arguments
 *   @verbatim
 	        hpdtx         - Dma host packet descriptor pointer
 	        packetsize    - Size of the packet to transmit (In Bytes)
 	        pTxBuffer     - Data Buffer pointer
			swapWords     - Flag to request for word swap
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reset successful
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initDma and USB_initQueueManager should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  transmits data to an end point
 *
 *   @b Modifies
 *   @n USB Dma registers
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdtx;

			USB_initDma();
			USB_initQueueManager(&hpdrx);
			....
			....
			USB_confDmaTx(&hpdrx, CSL_USB_DATA_SIZE, usbDataBuf);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_confDmaTx(CSL_UsbHostPktDescr    *hpdtx,
                         Uint32                 packetsize,
                         Uint16                 *pTxBuffer);

/** ============================================================================
 *   @n@b USB_dmaRxStop
 *
 *   @b Description
 *   @n Stops Dma data Receive operation. For C5505 PG1.4 and earlier
 *      versions of the DSP, wordswap is required for the data buffer after
 *      completing the data reception. Parameter 'swapWords' provides facility
 *      to request for the word swap for the data buffer.
 *
 *   @b Arguments
 *   @verbatim
			dwEndpoint   - Endpoint number that is configured for Rx
 			pRxBuffer    -   DMA Receive data buffer
			packetsize   -   Size of the data received (In Bytes)
			swapWords    -   Flag to swap the words in the Rx buffer
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_dmaRxStart should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Starts Dma data receive
 *
 *   @b Modifies
 *   @n USB Registers
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdrx;

			USB_confDmaRx(&hpdrx, CSL_USB_DATA_SIZE, usbDataBuf);
			....
			....
			USB_dmaRxStart();
			...
			...
			USB_dmaRxStop();
     @endverbatim
 *  ============================================================================
 */
void USB_dmaRxStop(DWORD     dwEndpoint);

/** ============================================================================
 *   @n@b USB_dmaTxStart
 *
 *   @b Description
 *   @n Starts Dma data transmit. DMA should be configure for the data
 *      transmit operation before calling this function. Data will be
 *      transmitted by the DMA as per the configured values.
 *
 *   @b Arguments
 *   @verbatim
 			dwEndpoint    - Endpoint number that is configured for Tx
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_confDmaTx should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Starts Dma data transfer
 *
 *   @b Modifies
 *   @n USB FIFO registers
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdtx;

			USB_initDma();
			USB_initQueueManager(&hpdtx);
			....
			....
			USB_confDmaTx(&hpdtx, CSL_USB_DATA_SIZE, usbDataBuf);
			....
			....
			USB_dmaTxStart();
     @endverbatim
 *  ============================================================================
 */
void USB_dmaTxStart(DWORD    dwEndpoint);


/** ============================================================================
 *   @n@b USB_dmaTxStop
 *
 *   @b Description
 *   @n Stops Dma data transmit.
 *
 *   @b Arguments
 *   @verbatim
 			dwEndpoint    - Endpoint number that is configured for Tx
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_dmaTxStart should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Stops Dma data receive
 *
 *   @b Modifies
 *   @n USB registers
 *
 *   @b Example
 *   @verbatim
			#define CSL_USB_DATA_SIZE    (256)
			Uint16    usbDataBuf[CSL_USB_DATA_SIZE];

			CSL_UsbHostPacketDescriptor    hpdtx;

			USB_confDmaTx(&hpdtx, CSL_USB_DATA_SIZE, usbDataBuf);
			....
			....
			USB_dmaTxStart();
			...
			...
			USB_dmaTxStop();
     @endverbatim
 *  ============================================================================
 */
void USB_dmaTxStop(DWORD    dwEndpoint);

/** ============================================================================
 *   @n@b USB_handleTx
 *
 *   @b Description
 *   @n Transmits the data over the non-control end points.
 *      This function can transmit the data in polled mode or using
 *      CPPI DMA. For the DMA mode operation opmode needs to be set to DMA
 *      using USB_init() API and CPPI DMA needs to be configured for USB Tx
 *      operation using USB_confDmaTx() API before calling this function.
 *
 *      USB DMA will be used when the data to be transferred is 64 or 512
 *      bytes. CPU will be used to transfer any other length of data.
 *
 *      Data to be sent will be copied to the Tx FIFO one packet at a time.
 *      Legth of the packet will be decided based on the maximum packet
 *      size assigned to the Tx FIFO. In case of poll mode operation complete
 *      transfer callback function will be invoked after copying all the packets
 *      to the Tx FIFO. In case of DMA mode operation complete transfer callback
 *      will be invoked for each packet transferred. Complete transfer callback
 *      function is supposed to wait for DMA transfer completion.
 *
 *      Number of bytes written to FIFO in each transaction will be stored in
 *      'pTransfer->prevTxferLen'. This value will be 64 or 512 when DMA mode
 *      is enabled and DMA is used for the data transfer. Other cases indicates
 *      that CPU mode is enabled or CPU is used for data transfer.
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
 	        dwEndpoint    -   End point number
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Returned for success
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Writes data to an end point
 *
 *   @b Modifies
 *   @n USB FIFO registers
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP1,
									  CSL_USB_BULK,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_handleTx(&gUsbContext, CSL_USB_EP1);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_handleTx(pUsbContext     pContext,
                        DWORD           dwEndpoint);

/** ============================================================================
 *   @n@b USB_handleRx
 *
 *   @b Description
 *   @n Receives the data over the non-control end points
 *      This function can read the data in polled mode or using
 *      CPPI DMA. For the DMA mode operation opmode needs to be set to DMA
 *      using USB_init() API and CPPI DMA needs to be configured for USB Rx
 *      operation using USB_confDmaRx() API before calling this function.
 *
 *      USB DMA will be used when the data to be received is 64 or 512
 *      bytes. CPU will be used to receive any other length of data.
 *
 *      Data to be recived will be read from the Rx FIFO one packet at a time.
 *      Legth of the packet will be decided based on the maximum packet
 *      size assigned to the Rx FIFO. In case of poll mode operation complete
 *      transfer callback function will be invoked after reading all the packets
 *      from the Rx FIFO. In case of DMA mode operation complete transfer callback
 *      will be invoked for each packet read. Complete transfer callback
 *      function is supposed to wait for DMA transfer completion.
 *
 *      Number of bytes read from FIFO in each transaction will be stored in
 *      'pTransfer->prevTxferLen'. This value will be 64 or 512 when DMA mode
 *      is enabled and DMA is used for the data transfer. Other cases indicates
 *      that CPU mode is enabled or CPU is used for data transfer.
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
 	        dwEndpoint    -   End point number
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Returned for success
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Reads data from an end point
 *
 *   @b Modifies
 *   @n USB Data buffer
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_OUT_EP2,
									  CSL_USB_BULK,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_handleRx(&gUsbContext, CSL_USB_EP2);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_handleRx(pUsbContext     pContext,
                        DWORD           dwEndpoint);

/** ============================================================================
 *   @n@b USB_processEP0Out
 *
 *   @b Description
 *   @n Reads the data from End point 0
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Returned for success
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Reads data from end point0
 *
 *   @b Modifies
 *   @n Ep0 data buffer
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_OUT_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_processEP0Out(&gUsbContext);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_processEP0Out(pUsbContext    pContext);

/** ============================================================================
 *   @n@b USB_processEP0In
 *
 *   @b Description
 *   @n Writes data to End point 0
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Returned for success
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Writes data to end point0
 *
 *   @b Modifies
 *   @n Ep0 FIFO registers
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_processEP0In(&gUsbContext);
     @endverbatim
 *  ============================================================================
 */
CSL_Status USB_processEP0In(pUsbContext     pContext);

/** ============================================================================
 *   @n@b USB_handleRxIntr
 *
 *   @b Description
 *   @n This function handles USb receive interrupts.
 *      This function is provided to be used with MSC module.
 *      For standalone MUSB operation this function is not required
 *      This function works only if EP2 configured for Rx
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Handles Rx interrupt
 *
 *   @b Modifies
 *   @n USb registers
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_handleRxIntr(&gUsbContext);
     @endverbatim
 *  ============================================================================
 */
void USB_handleRxIntr(pUsbContext     pContext);

/** ============================================================================
 *   @n@b USB_handleTxIntr
 *
 *   @b Description
 *   @n This function handles USB transmit interrupts.
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Handles Tx interrupt
 *
 *   @b Modifies
 *   @n USb registers
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_handleTxIntr(&gUsbContext);
     @endverbatim
 *  ============================================================================
 */
void USB_handleTxIntr(pUsbContext     pContext);

/** ============================================================================
 *   @n@b USB_checkSpeed
 *
 *   @b Description
 *   @n This function checks the USB bus speed
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
 	        pBusSpeed     -   USB Bus speed variable pointer
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                    TRUE  - Bus speed is valid
 *   @li                    FALSE - Bus speed is invalid
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Checks the USB bus speed
 *
 *   @b Modifies
 *   @n Bus speed variable
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			Bool              busSpeedStat;
			CSL_UsbContext    gUsbContext;
			CSL_UsbBusSpeed   *pBusSpeed

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			busSpeedStat = USB_checkSpeed(&gUsbContext, &gUsbContext.busSpeed);
     @endverbatim
 *  ============================================================================
 */
Bool USB_checkSpeed(pUsbContext        pContext,
			  		CSL_UsbBusSpeed    *pBusSpeed);


/** ============================================================================
 *   @n@b USB_readEP0Buf
 *
 *   @b Description
 *   @n This function reads the End point0 buffer
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
 	        pBusSpeed     -   USB Bus speed variable pointer
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Reads Ep0 data
 *
 *   @b Modifies
 *   @n Ep0 Buffer
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;
			CSL_UsbBusSpeed   *pBusSpeed

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_readEP0Buf(&gUsbContext, &gUsbContext.dwOutEP0Buf);
     @endverbatim
 *  ============================================================================
 */
void USB_readEP0Buf(pUsbContext    pContext,
                    DWORD          *pBuf);

/** ============================================================================
 *   @n@b USB_setConfiguration
 *
 *   @b Description
 *   @n This function sets the end point configuration
 *      This is provided to use with MSC module
 *      This function is not required for standalone MUSB testing
 *
 *   @b Arguments
 *   @verbatim
 	        pContext      -   USB context structure pointer
 	        pBusSpeed     -   USB Bus speed variable pointer
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Checks the USB bus speed
 *
 *   @b Modifies
 *   @n Bus speed variable
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			CSL_UsbContext    gUsbContext;
			CSL_UsbBusSpeed   *pBusSpeed

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			status = USB_setConfiguration(CSL_USB0, 0x02);
     @endverbatim
 *  ============================================================================
 */
void USB_setConfiguration(CSL_UsbDevNum    devNum,
                          Uint16           confVal);

/** ============================================================================
 *   @n@b USB_isValidDataInFifoOut
 *
 *   @b Description
 *   @n This function checks validity of the data in the output FIFO
 *
 *   @b Arguments
 *   @verbatim
 	        peps      -   End point status structure pointer
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                    TRUE  - FIFO out has valid data
 *   @li                    FALSE - FIFO out no has valid data
 *
 *   <b> Pre Condition </b>
 *   @n  USB_initEndptObj function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Checks the data in FIFO Out
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
			void  (*evtHandler)();

			pUsbEpHandle      hEp;
			CSL_Status        status;
			Bool              fifoStatus;
			CSL_UsbContext    gUsbContext;
			pUsbEpStatus      peps;

			status = USB_initEndptObj(CSL_USB0,
									  hEp,
									  CSL_USB_IN_EP0,
									  CSL_USB_CTRL,
									  0x40,
									  CSL_USB_EVENT_RESET,
									  evtHandler);
			....
			....
			fifoStatus = USB_isValidDataInFifoOut(peps);
     @endverbatim
 *  ============================================================================
 */
Bool USB_isValidDataInFifoOut(pUsbEpStatus    peps);

/** ============================================================================
 *   @n@b USB_flushFifo
 *
 *   @b Description
 *   @n Function to flush the Tx and Rx FIFO of the USB
 *
 *   @b Arguments
 *   @verbatim
 	        dwEndpoint  - End point to which FIFO flush is required
 	        fifoDirFlag - Flag to indicate the FIFO direction
 	                      1 - Tx FIFO, 0 - Rx FIFO
     @endverbatim
 *
 *   <b> Return Value </b> None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Flushes Tx/Rx FIFO
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            //Flush Tx FIFO
			USB_flushFifo (CSL_USB_EP1, 1);

            //Flush Rx FIFO
			USB_flushFifo (CSL_USB_EP2, 0);
     @endverbatim
 *  ============================================================================
 */
void USB_flushFifo(DWORD    dwEndpoint,
                   Bool     fifoDirFlag);

/** ============================================================================
 *   @n@b USB_serviceEp0RxPkt
 *
 *   @b Description
 *   @n Function to service the RxPkt for the Ep0
 *
 *   @b Arguments
 *   @verbatim
 	        periCsr0Val - Value of 'PERI_CSR0_INDX' register
     @endverbatim
 *
 *   <b> Return Value </b> None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Services EP0 Rx packet
 *
 *   @b Modifies
 *   @n USB registers
 *
 *   @b Example
 *   @verbatim

			USB_serviceEp0RxPkt (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK);
     @endverbatim
 *  ============================================================================
 */

/**
@} */
void USB_serviceEp0RxPkt(Uint16    periCsr0Val);

void USB_sendEpZLP(DWORD    dwEndpoint);

void USB_clearEpRxPktRdy(DWORD    dwEndpoint);

#endif    //_CSL_USBAUX_H_

