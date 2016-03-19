#include "stream.h"
#include "frame.h"
#include "synth.h"
#include "decoder.h"

/*
 * NAME:	decoder->init()
 * DESCRIPTION:	initialize a decoder object with callback routines
 */
void mad_decoder_init(struct mad_decoder *decoder, void *data,
		      enum mad_flow (*input_func)(void *,
						  struct mad_stream *),
		      enum mad_flow (*header_func)(void *,
						   struct mad_header const *),
		      enum mad_flow (*filter_func)(void *,
						   struct mad_stream const *,
						   struct mad_frame *),
		      enum mad_flow (*output_func)(void *,
						   struct mad_header const *,
						   struct mad_pcm *),
		      enum mad_flow (*error_func)(void *,
						  struct mad_stream *,
						  struct mad_frame *),
		      enum mad_flow (*message_func)(void *,
						    void *, unsigned int *))
{
	decoder->cb_data      = data;

	decoder->input_func   = input_func;
	decoder->header_func  = header_func;
	decoder->filter_func  = filter_func;
	decoder->output_func  = output_func;
	decoder->error_func   = error_func;
	decoder->message_func = message_func;
}

short mad_decoder_run(struct mad_decoder *decoder)
{
	void *error_data;
	struct mad_stream *stream;
	struct mad_frame *frame;
	struct mad_synth *synth;
	short result = 0;
	enum mad_flow decode_result;

	error_data = decoder->cb_data;

	stream = &decoder->sync.stream;
	frame  = &decoder->sync.frame;
	synth  = &decoder->sync.synth;

	mad_stream_init(stream);
	mad_frame_init(frame);
	mad_synth_init(synth);

	do {
		decode_result = decoder->input_func(decoder->cb_data, stream);
		switch (decode_result) {
		case MAD_FLOW_STOP:
			goto done;
		case MAD_FLOW_BREAK:
			goto fail;
		case MAD_FLOW_IGNORE:
			continue;
		case MAD_FLOW_CONTINUE:
			break;
		}

 		while (1) {
			if (decoder->header_func) {
				if (mad_header_decode(&frame->header, stream) == -1) {
					if (!MAD_RECOVERABLE(stream->error))
						break;

					decode_result = decoder->error_func(error_data,
														stream, frame);
					switch (decode_result) {
					case MAD_FLOW_STOP:
						goto done;
					case MAD_FLOW_BREAK:
						goto fail;
					case MAD_FLOW_IGNORE:
					case MAD_FLOW_CONTINUE:
					default:
						continue;
					}
				}

				decode_result = decoder->header_func(decoder->cb_data,
													 &frame->header);
				switch (decode_result) {
				case MAD_FLOW_STOP:
					goto done;
				case MAD_FLOW_BREAK:
					goto fail;
				case MAD_FLOW_IGNORE:
					continue;
				case MAD_FLOW_CONTINUE:
					break;
				}
			}

			if (mad_frame_decode(frame, stream) == -1) {
				if (!MAD_RECOVERABLE(stream->error))
					break;

				decode_result = decoder->error_func(error_data, stream, frame);
				switch (decode_result) {
				case MAD_FLOW_STOP:
					goto done;
				case MAD_FLOW_BREAK:
					goto fail;
				case MAD_FLOW_IGNORE:
					break;
				case MAD_FLOW_CONTINUE:
				default:
					continue;
				}
			}

			if (decoder->filter_func) {
				decode_result = decoder->filter_func(decoder->cb_data,
													 stream, frame);
				switch (decode_result) {
				case MAD_FLOW_STOP:
					goto done;
				case MAD_FLOW_BREAK:
					goto fail;
				case MAD_FLOW_IGNORE:
					continue;
				case MAD_FLOW_CONTINUE:
					break;
				}
      		}

			mad_synth_frame(synth, frame);

			if (decoder->output_func) {
				decode_result = decoder->output_func(decoder->cb_data,
											&frame->header, &synth->pcm);
				switch (decode_result) {
				case MAD_FLOW_STOP:
					goto done;
				case MAD_FLOW_BREAK:
					goto fail;
				case MAD_FLOW_IGNORE:
				case MAD_FLOW_CONTINUE:
					break;
				}
			}
		}
	} while (stream->error == MAD_ERROR_BUFLEN);

fail:
	result = -1;

done:
	mad_synth_finish(synth);
	mad_frame_finish(frame);
	mad_stream_finish(stream);

	return result;
}


