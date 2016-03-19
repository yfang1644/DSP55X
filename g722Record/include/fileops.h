#ifndef		_FILEOPS_H_
#define		_FILEOPS_H_

#define	RECORD			  0x0004		/* record 0000 0000 0000 01XX */
#define	REPLAY			  0x0008		/* replay 0000 0000 0000 10XX */

#define	RAW				  0x0000		/* raw data					  */
#define	COD1			  0x0001		/* g722.1 encode			  */

#define	FILE_DMAMODE	1
#undef	FILE_DMAMODE

#define	MAX_FILELENGTH	64
enum {
	NO_OPERATION,
	FILE_RECORD_RAW		= RECORD|RAW,
	FILE_RECORD_COD1	= RECORD|COD1,
	FILE_REPLAY_RAW		= REPLAY|RAW,
	FILE_REPLAY_COD1	= REPLAY|COD1,
	DIRECT_IO = 0x0080,
	MONITOR
};

typedef struct _recordStruct {
	Int8 *curFileName;		// yyyymmdd/hhmmsslr.cod, lr=left/right,cod=raw,16k,24k,32k
	Uint16 code;			// 0=raw, 0x0100=g722.1, 0x0116
							// code bps:low=0x0116(16k)
							//          medium=0x0124(24k)
							//          high=0x0132(32k)
	Uint16 channel;			// left=1, right=2, stereo=3
	Uint16 function;
	Uint16 framesize;		// read/write framesize
	Uint32 split;			// for split reload
	Uint32 frames;			// file splits in frame(20ms)
	Uint8 sampleRate;		// sampling rate
} recordStruct;


void fileProcessing(void);
void writeFileProcessing(int channel);
void readFileProcessing(int channel);
void File_init(void);

#endif

