/******************************************************************************\
\*  USB.h	V1.00																*\
\*  Author: Fang Yuan
\******************************************************************************/
#ifndef _C5535_USB_
#define _C5535_USB_


#define VR_TYPE_OUT	     	0x40
#define	VR_TYPE_IN	     	0xC0
#define VR_RESET		 	0xB0
#define VR_ENDPOINT0READ 	0xB1
#define VR_REGWRITE     	0xB2
#define VR_REGREAD		 	0xB3
#define VR_ENDPOINT0WRITE	0xB4

#define VR_BULK_WRITE       0xB6

#define VR_LED_OPTION		0xB7
#define VR_USB_VERSION		0xB8

#define	VR_CHANNEL_MONITOR	0xBA
#define	VR_SAMPLING_RATE	0xBB
#define	VR_REF_CHANNEL		0xBC
#define	VR_SET_STEPSIZE		0xBD
#define	VR_SET_SPECDEEP		0xBE
#define	VR_SET_BEAMERSTEP	0xBF

#define	VR_SET_ADAPT_L		0xC0
#define	VR_SET_DELAY		0xC1
#define	VR_SET_GAIN			0xC2
#define VR_SET_BEAM_L		0xC3
#define VR_SET_TOTALCHN		0xC4

#define	VR_CHANNEL_READ		0xC8
#define	VR_FUNCTION_SET		0xC9
	#define	NULL_FUNCTION	0
	#define	FILTER			1
	#define	ADAPTIVE		2
	#define	SPEC_SUB		3
	#define	BEAMFORM		4
	#define	SPEC_CAN		5

#define	VR_BUFFER_READ		0xCC
#define	VR_POLLING			0xCD
#define	VR_FILTER_WRITE		0xCE

#endif
