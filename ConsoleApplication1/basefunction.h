#pragma once
//#include <vector>
//#include <chrono>
//#include <thread>

extern "C"
{
//#include "stdint.h"
#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
}
using namespace std;
/*

enum {
	FLV_TAG_TYPE_AUDIO = 0x08,
	FLV_TAG_TYPE_VIDEO = 0x09,
	FLV_TAG_TYPE_META = 0x12,
};

static AVStream *create_stream(AVFormatContext *s, int codec_type)
{
	AVStream *st = avformat_new_stream(s, NULL);
	if (!st)
		return NULL;
	st->codec->codec_type = codec_type;
	return st;
}
static AVPacket *add_to_pktbuf(AVPacketList **packet_buffer, AVPacket *pkt, AVPacketList **plast_pktl)
{
	AVPacketList *pktl = (AVPacketList *)av_mallocz(sizeof(AVPacketList));
	if (!pktl)
		return NULL;

	if (*packet_buffer)
		(*plast_pktl)->next = pktl;
	else
		*packet_buffer = pktl;

	//Add the packet in the buffered packet list. 
	*plast_pktl = pktl;
	pktl->pkt = *pkt;
	return &pktl->pkt;
}

static int get_video_extradata(AVFormatContext *s, int video_index)
{
	int  type, size, flags, pos, stream_type;
	int ret = -1;
	int64_t dts;
	bool got_extradata = false;

	if (!s || video_index < 0 || video_index > 2)
		return ret;

	for (;; avio_skip(s->pb, 4)) {
		pos = avio_tell(s->pb);
		type = avio_r8(s->pb);
		size = avio_rb24(s->pb);
		dts = avio_rb24(s->pb);
		dts |= avio_r8(s->pb) << 24;
		avio_skip(s->pb, 3);

		if (0 == size)
			break;
		if (FLV_TAG_TYPE_AUDIO == type || FLV_TAG_TYPE_META == type) {

			avio_seek(s->pb, size, SEEK_CUR);
		}
		else if (type == FLV_TAG_TYPE_VIDEO) {

			size -= 5;
			s->streams[video_index]->codec->extradata = xmalloc(size + FF_INPUT_BUFFER_PADDING_SIZE);
			if (NULL == s->streams[video_index]->codec->extradata)
				break;
			memset(s->streams[video_index]->codec->extradata, 0, size + FF_INPUT_BUFFER_PADDING_SIZE);
			memcpy(s->streams[video_index]->codec->extradata, s->pb->buf_ptr + 5, size);
			s->streams[video_index]->codec->extradata_size = size;
			ret = 0;
			got_extradata = true;
		}
		else {

			break;
		}

		if (got_extradata)
			break;
	}

	return ret;
}

static int init_decode(AVFormatContext *s)
{
	int video_index = -1;
	int audio_index = -1;
	int ret = -1;

	if (!s)
		return ret;


	if (0 == s->nb_streams) {
		create_stream(s, AVMEDIA_TYPE_VIDEO);
		create_stream(s, AVMEDIA_TYPE_AUDIO);
		video_index = 0;
		audio_index = 1;
	}
	else if (1 == s->nb_streams) {
		if (AVMEDIA_TYPE_VIDEO == s->streams[0]->codec->codec_type) {
			create_stream(s, AVMEDIA_TYPE_AUDIO);
			video_index = 0;
			audio_index = 1;
		}
		else if (AVMEDIA_TYPE_AUDIO == s->streams[0]->codec->codec_type) {
			create_stream(s, AVMEDIA_TYPE_VIDEO);
			video_index = 1;
			audio_index = 0;
		}
	}
	else if (2 == s->nb_streams) {
		if (AVMEDIA_TYPE_VIDEO == s->streams[0]->codec->codec_type) {
			video_index = 0;
			audio_index = 1;
		}
		else if (AVMEDIA_TYPE_VIDEO == s->streams[1]->codec->codec_type) {
			video_index = 1;
			audio_index = 0;
		}
	}

	if (video_index != 0 && video_index != 1)
		return ret;

	//Init the audio codec(AAC).
	s->streams[audio_index]->codecpar->codec_id = AV_CODEC_ID_AAC;
	s->streams[audio_index]->codecpar->sample_rate = 8000;
	s->streams[audio_index]->codecpar->time_base.den = 44100;
	s->streams[audio_index]->codecpar->time_base.num = 1;
	s->streams[audio_index]->codecpar->bits_per_coded_sample = 16; //
	s->streams[audio_index]->codecpar->channels = 1;
	s->streams[audio_index]->codecpar->channel_layout = 4;
	s->streams[audio_index]->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	s->streams[audio_index]->codecpar->bit_rate = 16000;
	s->streams[audio_index]->codecpar->refs = 1;
	s->streams[audio_index]->codecpar->sample_fmt = AV_SAMPLE_FMT_FLTP;
	s->streams[audio_index]->codecpar->profile = 1;
	s->streams[audio_index]->codecpar->level = -99;

	s->streams[audio_index]->pts_wrap_bits = 32;
	s->streams[audio_index]->time_base.den = 1000;
	s->streams[audio_index]->time_base.num = 1;

	//Init the video codec(H264).
	s->streams[video_index]->codec->codec_id = AV_CODEC_ID_H264;
	s->streams[video_index]->codec->width = 1280;
	s->streams[video_index]->codec->height = 720;
	s->streams[video_index]->codec->ticks_per_frame = 2;
	s->streams[video_index]->codec->pix_fmt = AV_PIX_FMT_YUV420P;
	s->streams[video_index]->codec->time_base.den = 2000;
	s->streams[video_index]->codec->time_base.num = 1;
	s->streams[video_index]->codec->sample_fmt = AV_SAMPLE_FMT_NONE;
	s->streams[video_index]->codec->frame_size = 0;
	s->streams[video_index]->codec->frame_number = 7;
	s->streams[video_index]->codec->has_b_frames = 0;
	s->streams[video_index]->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	s->streams[video_index]->codec->codec_tag = 0;
	s->streams[video_index]->codec->bit_rate = 0;
	s->streams[video_index]->codec->refs = 1;
	s->streams[video_index]->codec->sample_rate = 0;
	s->streams[video_index]->codec->channels = 0;
	s->streams[video_index]->codec->profile = 66;
	s->streams[video_index]->codec->level = 31;
	s->streams[video_index]->pts_wrap_bits = 32;
	s->streams[video_index]->time_base.den = 1000;
	s->streams[video_index]->time_base.num = 1;
	s->streams[video_index]->avg_frame_rate.den = 1;
	s->streams[video_index]->avg_frame_rate.num = 25;

	char option_key[] = "encoder";
	char option_value[] = "Lavf57.0.100";
	ret = av_dict_set(&(s->metadata), option_key, option_value, 0);
	AVDictionaryEntry *tag = NULL;
	tag = av_dict_get((s->metadata), "", tag, AV_DICT_IGNORE_SUFFIX);

	s->duration = 0;
	s->start_time = 0;
	s->bit_rate = 0;
	s->iformat->flags = 0;
	s->duration_estimation_method = AVFMT_DURATION_FROM_STREAM;

	AVPacket packet;
	av_init_packet(&packet);
	while (true)
	{
		int ret1 = av_read_frame(s, &packet);
		if (packet.flags & AV_PKT_FLAG_KEY)
		{
			break;
		}
	}
	//AVFormatInternal* internal = s->internal;
	
	//add_to_pktbuf(&(internalpacket_buffer), &packet, &(s->internal->packet_buffer_end));

	s->streams[video_index]->r_frame_rate.den = 1;
	s->streams[video_index]->r_frame_rate.num = 25;

	//Update the AVFormatContext Info/
	s->nb_streams = 2;

	s->pb->pos = (int64_t)s->pb->buf_end;

	return ret;
}*/
