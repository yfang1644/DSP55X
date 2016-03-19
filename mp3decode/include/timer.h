# ifndef LIBMAD_TIMER_H
# define LIBMAD_TIMER_H

typedef struct {
  signed long seconds;		/* whole seconds */
  unsigned long fraction;	/* 1/MAD_TIMER_RESOLUTION seconds */
} mad_timer_t;

extern mad_timer_t const mad_timer_zero;

# define MAD_TIMER_RESOLUTION	352800000UL

# define mad_timer_reset(timer)	((void) (*(timer) = mad_timer_zero))

void mad_timer_set(mad_timer_t *, unsigned long, unsigned long, unsigned long);


# endif
