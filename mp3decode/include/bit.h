# ifndef LIBMAD_BIT_H
# define LIBMAD_BIT_H

#define	CHAR_BIT	8

struct mad_bitptr {
  unsigned char const *byte;
  unsigned short cache;
  unsigned short left;
};

void mad_bit_init(struct mad_bitptr *, unsigned char const *);

# define mad_bit_finish(bitptr)		/* nothing */

unsigned long mad_bit_length(struct mad_bitptr const *,
			    struct mad_bitptr const *);

# define mad_bit_bitsleft(bitptr)  ((bitptr)->left)
unsigned char const *mad_bit_nextbyte(struct mad_bitptr const *);

void mad_bit_skip(struct mad_bitptr *, unsigned short);
unsigned long mad_bit_read(struct mad_bitptr *, unsigned short);
unsigned short mad_bit_crc(struct mad_bitptr, unsigned short, unsigned long);

# endif
