# ifndef LIBMAD_SYNTH_H
# define LIBMAD_SYNTH_H

# include "fixed.h"
# include "frame.h"

struct mad_pcm {
  unsigned int samplerate;		/* sampling frequency (Hz) */
  unsigned short channels;		/* number of channels */
  unsigned short length;		/* number of samples per channel */
  mad_fixed_t samples[2][1152];		/* PCM output samples [ch][sample] */
};

struct mad_synth {
  mad_fixed_t filter[2][2][2][16][8];	/* polyphase filterbank outputs */
  					/* [ch][eo][peo][s][v] */

  unsigned short phase;			/* current processing phase */

  struct mad_pcm pcm;			/* PCM output */
};

/* single channel PCM selector */
enum {
  MAD_PCM_CHANNEL_SINGLE = 0
};

/* dual channel PCM selector */
enum {
  MAD_PCM_CHANNEL_DUAL_1 = 0,
  MAD_PCM_CHANNEL_DUAL_2 = 1
};

/* stereo PCM selector */
enum {
  MAD_PCM_CHANNEL_STEREO_LEFT  = 0,
  MAD_PCM_CHANNEL_STEREO_RIGHT = 1
};

void mad_synth_init(struct mad_synth *);

# define mad_synth_finish(synth)  /* nothing */

void mad_synth_mute(struct mad_synth *);

void mad_synth_frame(struct mad_synth *, struct mad_frame const *);

# endif
