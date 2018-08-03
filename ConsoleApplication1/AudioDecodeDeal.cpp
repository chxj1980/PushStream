#include "stdafx.h"
#include "AudioDecodeDeal.h"
//#include <MediaObject.h>
#include"MediaObject.h"
AudioDecodeDeal::AudioDecodeDeal() :bRun(true)
{

}
AudioDecodeDeal::~AudioDecodeDeal() {

}

void AudioDecodeDeal::threadProduce() {
	/*AVPacket *pkt = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(pkt);
	pkt->data = NULL;
	pkt->size = 0;*/

	MediaManeger *manager = MediaManeger::Instance();
	struct SwrContext *aud_convert_ctx = swr_alloc_set_opts(NULL,
		av_get_default_channel_layout(manager->m_OutputAudioCodec_Ctx->channels),
		manager->m_OutputAudioCodec_Ctx->sample_fmt,
		manager->m_OutputAudioCodec_Ctx->sample_rate,
		av_get_default_channel_layout(manager->m_Input_Audio_Ctx->streams[manager->m_input_audiostream_index]->codecpar->channels),
		manager->m_Input_AudioCodec_Ctx->sample_fmt,//sample_fmt
		manager->m_Input_AudioCodec_Ctx->sample_rate,
		0, NULL);
	swr_init(aud_convert_ctx);
	AVAudioFifo *fifo = NULL;
	fifo = av_audio_fifo_alloc(manager->m_OutputAudioCodec_Ctx->sample_fmt, manager->m_OutputAudioCodec_Ctx->channels, 1);
	//int output_frame_size = manager->m_OutputAudioCodec_Ctx->frame_size;
	uint8_t **converted_input_samples = NULL;
	if (!(converted_input_samples = (uint8_t**)calloc(manager->m_OutputAudioCodec_Ctx->channels,
		sizeof(**converted_input_samples)))) {
		printf("Could not allocate converted input sample pointers\n");
		return;
	}
	/*manager->aud_convert_ctx = swr_alloc_set_opts(NULL,
		av_get_default_channel_layout(manager->m_OutputAudioCodec_Ctx->channels),
		manager->m_OutputAudioCodec_Ctx->sample_fmt,
		manager->m_OutputAudioCodec_Ctx->sample_rate,
		av_get_default_channel_layout(manager->m_Input_Audio_Ctx->streams[manager->m_input_audiostream_index]->codecpar->channels),
		manager->m_OutputAudioCodec_Ctx->sample_fmt,//sample_fmt
		manager->m_OutputAudioCodec_Ctx->sample_rate,
		0, NULL);

	/**
	* Perform a sanity check so that the number of converted samples is
	* not greater than the number of samples to be converted.
	* If the sample rates differ, this case has to be handled differently
	*/
	//av_assert0(pCodecCtx_a->sample_rate == ifmt_ctx_a->streams[audioindex]->codec->sample_rate);

	//swr_init(manager->aud_convert_ctx);
	//Initialize the FIFO buffer to store audio samples to be encoded. 
	//AVAudioFifo *fifo = NULL;
	//fifo = av_audio_fifo_alloc(manager->m_OutputAudioCodec_Ctx->sample_fmt, manager->m_OutputAudioCodec_Ctx->channels, 1);

	//Initialize the buffer to store converted samples to be encoded.
	//uint8_t **converted_input_samples = NULL;
	/**
	* Allocate as many pointers as there are audio channels.
	* Each pointer will later point to the audio samples of the corresponding
	* channels (although it may be NULL for interleaved formats).
	*/
	/*
	if (!(converted_input_samples = (uint8_t**)calloc(manager->m_OutputAudioCodec_Ctx->channels,
		sizeof(**converted_input_samples)))) {
		printf("Could not allocate converted input sample pointers\n");
		return;
	}*/

	AVFrame *frame = av_frame_alloc();
	AVPacket input_packet;
	av_init_packet(&input_packet);
	input_packet.data = NULL;
	input_packet.size = 0;
	const int output_frame_size = manager->m_OutputAudioCodec_Ctx->frame_size;
	int ret = -1;
	while (bRun)
	{
		
		while (av_audio_fifo_size(fifo) < output_frame_size) {

			if (ret = av_read_frame(manager->m_Input_Audio_Ctx, &input_packet) < 0)
			{
				if (ret == AVERROR_EOF)
				{
					//encode_audio = 0;
					continue;
				}
				else
				{
					printf("Could not read audio frame\n");
					return;
				}
			}

			ret = avcodec_send_packet(manager->m_Input_AudioCodec_Ctx, &input_packet);
			while (avcodec_receive_frame(manager->m_Input_AudioCodec_Ctx, frame) == 0) {

				/**
				* Allocate memory for the samples of all channels in one consecutive
				* block for convenience.
				*/
				if ((ret = av_samples_alloc(converted_input_samples, NULL, manager->m_OutputAudioCodec_Ctx->channels, frame->nb_samples, manager->m_OutputAudioCodec_Ctx->sample_fmt, 0)) < 0) {
					printf("Could not allocate converted input samples\n");
					av_freep(&(*converted_input_samples)[0]);
					free(*converted_input_samples);
					return;
				}

				/**
				* Convert the input samples to the desired output sample format.
				* This requires a temporary storage provided by converted_input_samples.
				*/
				/** Convert the samples using the resampler. *///converted_input_samples
				if ((ret = swr_convert(aud_convert_ctx, converted_input_samples, frame->nb_samples, (const uint8_t**)frame->extended_data, frame->nb_samples)) < 0) {
					printf("Could not convert input samples\n");
					return;
				}

				if ((ret = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame->nb_samples)) < 0) {
					printf("Could not reallocate FIFO\n");
					return;
				}

				/** Store the new samples in the FIFO buffer. */
				if (av_audio_fifo_write(fifo, (void **)converted_input_samples,
					frame->nb_samples) < frame->nb_samples) {
					printf("Could not write data to FIFO\n");
					return;
				}
			}
		}
		InputDecodeObject *pInputDecode = new InputDecodeObject();
		pInputDecode->type = 0;
		pInputDecode->decodeTime = av_gettime();
		if (av_audio_fifo_size(fifo) >= output_frame_size)
		{

			const int frame_size = FFMIN(av_audio_fifo_size(fifo),
				output_frame_size);
			AVFrame *convertFrame = av_frame_alloc();
			convertFrame->nb_samples = frame_size;// output_frame_size;
			convertFrame->channel_layout = manager->m_OutputAudioCodec_Ctx->channel_layout;
			convertFrame->format = manager->m_OutputAudioCodec_Ctx->sample_fmt;
			convertFrame->sample_rate = manager->m_OutputAudioCodec_Ctx->sample_rate;
			/**
			* Allocate the samples of the created frame. This call will make
			* sure that the audio frame can hold as many samples as specified.
			*/
			if ((ret = av_frame_get_buffer(convertFrame, 0)) < 0) {
				printf("Could not allocate output frame samples\n");
				av_frame_free(&convertFrame);
				return;
			}
			/**
			* Read as many samples from the FIFO buffer as required to fill the frame.
			* The samples are stored in the frame temporarily.
			*/
			if (av_audio_fifo_read(fifo, (void **)convertFrame->data, frame_size) < frame_size) {
				printf("Could not read data from FIFO\n");
				av_frame_free(&convertFrame);
				return;
			}
			manager->m_FrameAudioSize++;
			uint64_t n_decodenum = manager->m_FrameAudioSize;

			pInputDecode->frame = convertFrame;
			pInputDecode->num = n_decodenum;

			manager->m_InputAudioQueue.push(pInputDecode);
			//manager->m_InputQueue.push(pInputDecode);
		}
		else {
			if (pInputDecode)
			{
				delete pInputDecode;
			}
		}
	}
	av_packet_unref(&input_packet);
	//av_frame_free(&convertFrame);
	av_frame_free(&frame);
	//av_packet_free(&pkt);
	swr_free(&aud_convert_ctx);
}
void AudioDecodeDeal::start()
{
	boost::function0<void> fun = boost::bind(&AudioDecodeDeal::threadProduce, this);
	boost::thread threadAudioDecode(fun);
	//this->threadProduce();
}