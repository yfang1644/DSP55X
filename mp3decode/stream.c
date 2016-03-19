# include "bit.h"
# include "stream.h"

/*
 * NAME:	stream->init()
 * DESCRIPTION:	initialize stream struct
 */
void mad_stream_init(struct mad_stream *stream)
{
	stream->buffer     = 0;
	stream->bufend     = 0;
	stream->skiplen    = 0;

	stream->sync       = 0;
	stream->freerate   = 0;

	stream->this_frame = 0;
	stream->next_frame = 0;
	mad_bit_init(&stream->ptr, 0);

	mad_bit_init(&stream->anc_ptr, 0);
	stream->anc_bitlen = 0;

	stream->md_len     = 0;

	stream->error      = MAD_ERROR_NONE;
}

/*
 * NAME:	stream->finish()
 * DESCRIPTION:	deallocate any dynamic memory associated with stream
 */
void mad_stream_finish(struct mad_stream *stream)
{
	mad_bit_finish(&stream->anc_ptr);
	mad_bit_finish(&stream->ptr);
}

/*
 * NAME:	stream->buffer()
 * DESCRIPTION:	set stream buffer pointers
 */
void mad_stream_buffer(struct mad_stream *stream,
		       unsigned char const *buffer, unsigned short length)
{
	stream->buffer = buffer;
	stream->bufend = buffer + length;

	stream->this_frame = buffer;
	stream->next_frame = buffer;

	stream->sync = 1;

	mad_bit_init(&stream->ptr, buffer);
}


/*
 * NAME:	stream->sync()
 * DESCRIPTION:	locate the next stream sync word
 */
short mad_stream_sync(struct mad_stream *stream)
{
	unsigned char const *ptr, *end;

	ptr = mad_bit_nextbyte(&stream->ptr);
	end = stream->bufend;

	while ((ptr < end - 1) && !((ptr[0] == 0xff) && (ptr[1] & 0xe0) == 0xe0))
		++ptr;

	if (end - ptr < MAD_BUFFER_GUARD)
		return -1;

	mad_bit_init(&stream->ptr, ptr);

	return 0;
}

