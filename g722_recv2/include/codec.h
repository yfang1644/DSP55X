//
// FILENAME : codec.h
// This file defines global constants
//

#ifndef	_CODEC_H
#define	_CODEC_H

#define	BITRATE		16	// available bps:16k,24k,32k,48k
						// 8kbps test version
#define	BANDWIDTH	7	//BANDWIDTH 14kHz, only 7 or 14 available

typedef enum _CHANNEL {
	LEFT = 0,
	RIGHT = 1,
	STEREO = 2
} CHANNEL;

#define	AUDIOPROCESSING		1
#define	FILEPROCESSING		2
#define	SPIPROCESSING		4
#define	GPIOPROCESSING		8

#endif	// _CODEC_H
// End of codec.h

