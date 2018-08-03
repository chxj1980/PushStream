#include "stdafx.h"
#include "basefunction.h"
//#include <MediaObject.h>
#include"MediaObject.h"
#include <windows.h>
#include <iostream>

char *dup_wchar_to_utf8(const wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_ACP, 0, w, -1, 0, 0, 0, 0);
	s = (char *)malloc(l);
	if (s)
		WideCharToMultiByte(CP_ACP, 0, w, -1, s, l, 0, 0);//CP_UTF8
	return s;
}

//MediaObject* MediaObject::manager;
bool MediaManeger::OpenInputNVR(char *chURL, AVDictionary *device_param)
{
	if (chURL)
	{
		if (avformat_open_input(&m_Input_Video_Ctx, chURL, NULL, &device_param) != 0) {

			printf("Couldn't open input video stream.（无法打开输入流）\n");
			return false;
		}

		//input video initialize
		if (avformat_find_stream_info(m_Input_Video_Ctx, &device_param) < 0)//avformat_find_stream_info
		{
			printf("Couldn't find video stream information.（无法获取流信息）\n");
			return false;
		}
		for (int i = 0; i < m_Input_Video_Ctx->nb_streams; i++)
			if (m_Input_Video_Ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				m_input_videostream_index = i;
				break;
			}
		if (m_input_videostream_index == -1)
		{
			printf("Couldn't find a video stream.（没有找到视频流）\n");
			return false;
		}

		// 视频解码器查找
		if (!(pCodec_InputVideo = avcodec_find_decoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id)))
		{
			printf("Can not find input video decoder! (没有找到合适的解码器！)\n");
			return false;
		}

		m_Input_VideoCodec_Ctx = avcodec_alloc_context3(pCodec_InputVideo);
		/* Copy codec parameters from input stream to output codec context */
		if (avcodec_parameters_to_context(m_Input_VideoCodec_Ctx, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
			return false;
		}
		//m_Input_VideoCodec_Ctx->framerate = 25;
		/*
		if (avcodec_open2(m_Input_VideoCodec_Ctx, pCodec_InputVideo, NULL) < 0)//&device_param
		{
			fprintf(stderr, "Failed to open %s codec\n",
				av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
			return false;
		}*/
		printf("Input Source:video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d\n",
			m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->width, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->height, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->format,//pix_fmt
			m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.num, m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.den,
			m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->sample_aspect_ratio.num, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->sample_aspect_ratio.den);

		return true;
	}
	return false;
}
bool MediaManeger::OpenVideoCapture(char *chInputVideo, AVDictionary *device_param)
{

	if (chInputVideo)
	{
		if(m_Input_Format==NULL)
			m_Input_Format = av_find_input_format("dshow");
		if (avformat_open_input(&m_Input_Video_Ctx, chInputVideo, m_Input_Format, &device_param) != 0) {

			printf("Couldn't open input video stream.（无法打开输入流）\n");
			return false;
		}
		AVDictionary* pOptions = NULL;
		//m_Input_Video_Ctx->probesize = 100 * 1024;
		//m_Input_Video_Ctx->max_analyze_duration = 5 * AV_TIME_BASE;
		//std::vector<shared_ptr<AVPacket>> pak;
		//input video initialize
		if (avformat_find_stream_info(m_Input_Video_Ctx, &pOptions) < 0)//avformat_find_stream_info
		{
			printf("Couldn't find video stream information.（无法获取流信息）\n");
			return false;
		}
		for (int i = 0; i < m_Input_Video_Ctx->nb_streams; i++)
			if (m_Input_Video_Ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				m_input_videostream_index = i;
				break;
			}
		if (m_input_videostream_index == -1)
		{
			printf("Couldn't find a video stream.（没有找到视频流）\n");
			return false;
		}
		/*if ((m_input_videostream_index = av_find_best_stream(m_Input_Video_Ctx, AVMEDIA_TYPE_VIDEO, 1, -1, NULL, 0)) < 0)
		{
			printf("Couldn't find a video stream.（没有找到视频流）\n");
			return false;
		}*/
		/*
		if (pFormatCtx_Video->streams[0]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
		{
			printf("Couldn't find video stream information.（无法获取视频流信息）\n");
			return -1;
		}*/
		// 视频解码器查找
		if (!(pCodec_InputVideo = avcodec_find_decoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id)))
		{
			printf("Can not find input video decoder! (没有找到合适的解码器！)\n");
			return false;
		}

		m_Input_VideoCodec_Ctx = avcodec_alloc_context3(pCodec_InputVideo);
		/* Copy codec parameters from input stream to output codec context */
		if (avcodec_parameters_to_context(m_Input_VideoCodec_Ctx, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
			return false;
		}
		//m_Input_VideoCodec_Ctx->framerate = 25;
		if (avcodec_open2(m_Input_VideoCodec_Ctx, pCodec_InputVideo, NULL) < 0)//&device_param
		{
			fprintf(stderr, "Failed to open %s codec\n",
				av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
			return false;
		}
		printf("Input Source:video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d\n",
			m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->width, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->height, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->format,//pix_fmt
			m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.num, m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.den,
			m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->sample_aspect_ratio.num, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->sample_aspect_ratio.den);

		return true;
	}
	return false;
}

bool MediaManeger::OpenAudioCapture(const char *chInputAudio, AVDictionary *device_param)
{
	if (chInputAudio)
	{
		if (!m_Input_Format)
			m_Input_Format = av_find_input_format("dshow");
		//char * psDevName = dup_wchar_to_utf8(L"audio=麦克风 (Realtek High Definition Audio)");
		if (avformat_open_input(&m_Input_Audio_Ctx, chInputAudio, m_Input_Format, NULL) != 0) {

			printf("Couldn't open input audio stream.（无法打开输入流）\n");
			return false;
		}
		//input audio initialize
		if (avformat_find_stream_info(m_Input_Audio_Ctx, NULL) < 0)
		{
			printf("Couldn't find audio stream information.（无法获取流信息）\n");
			return false;
		}
		for (int i = 0; i < m_Input_Audio_Ctx->nb_streams; i++)
			if (m_Input_Audio_Ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				m_input_audiostream_index = i;
				break;
			}
		if (m_input_audiostream_index == -1)
		{
			printf("Couldn't find a video stream.（没有找到视频流）\n");
			return false;
		}
		/*if ((m_input_audiostream_index = av_find_best_stream(m_Input_Audio_Ctx, AVMEDIA_TYPE_AUDIO, 1, -1, NULL, 0)) < 0)
		{
			printf("Couldn't find a video stream.（没有找到视频流）\n");
			return false;
		}*/
		// 音频解码器查找
		if (!(pCodec_InputAudio = avcodec_find_decoder(m_Input_Audio_Ctx->streams[m_input_audiostream_index]->codecpar->codec_id)))
		{
			printf("Can not find input audio decoder! (没有找到合适的解码器！)\n");
			return false;
		}

		m_Input_AudioCodec_Ctx = avcodec_alloc_context3(pCodec_InputAudio);
		/* Copy codec parameters from input stream to output codec context */
		if (avcodec_parameters_to_context(m_Input_AudioCodec_Ctx, m_Input_Audio_Ctx->streams[m_input_audiostream_index]->codecpar) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
			return false;
		}
		if (avcodec_open2(m_Input_AudioCodec_Ctx, pCodec_InputAudio, NULL) < 0)//&device_param
		{
			fprintf(stderr, "Failed to open %s codec\n",
				av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
			return false;
		}

		return true;
	}
	return false;
}
bool MediaManeger::OpenOutputFromNVR(char *ch_FormatName, char *ch_FilePath)
{
	//output initialize
	avformat_alloc_output_context2(&m_Output_Ctx, NULL, ch_FormatName, ch_FilePath);//"flv"
	pCodec_Video = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pCodec_Video) {
		printf("Can not find output encoder! (没有找到合适的编码器！)\n");
		return false;
	}
	int ret = avio_open2(&m_Output_Ctx->pb, ch_FilePath, AVIO_FLAG_READ_WRITE, NULL, NULL);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "open avio failed");
		goto Error;
	}
	AVCodec *p_Codec_Video = avcodec_find_encoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id);
	if (!p_Codec_Video) {
		printf("Can not find output encoder! (没有找到合适的编码器！)\n");
		goto Error;
	}
	AVStream * stream = avformat_new_stream(m_Output_Ctx, p_Codec_Video);
	stream->time_base.num = 1;// m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.num;
	stream->time_base.den = 25; //m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.den;//90000
	if (avcodec_parameters_from_context(stream->codecpar, m_Input_VideoCodec_Ctx) < 0) {
		fprintf(stderr, "Failed to copy %s codec context to encoder parameters\n",
			av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
		goto Error;
	}
	//for (int i = 0; i < m_Input_Video_Ctx->nb_streams; i++)
	//{
		
		/*stream->codecpar->codec_tag = 0;
		if (m_Output_Ctx->oformat->flags & AVFMT_GLOBALHEADER)
			stream->codecpar->flags |= CODEC_FLAG_GLOBAL_HEADER;*/
	//}

	ret = avformat_write_header(m_Output_Ctx, NULL);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "format write header failed");
		goto Error;
	}

	av_log(NULL, AV_LOG_FATAL, " Open output file success %s\n", ch_FilePath);
	return ret;
Error:
	if (m_Output_Ctx)
	{
		avformat_close_input(&m_Output_Ctx);
	}
	return ret;

}
bool MediaManeger::OpenOutputFlv(char *ch_FilePath, bool bVideo, bool bAudio)
{
	return this->OpenOutput("flv", ch_FilePath, bVideo, bAudio);
}
bool MediaManeger::OpenOutputRtsp(char *ch_FilePath, bool bVideo, bool bAudio)
{
	return this->OpenOutput("rtsp", ch_FilePath, bVideo, bAudio);
}
bool MediaManeger::OpenOutput(char *ch_FormatName, char *ch_FilePath, bool bVideo, bool bAudio)
{
	//output initialize
	int ret = avformat_alloc_output_context2(&m_Output_Ctx, NULL, ch_FormatName, ch_FilePath);//"flv"
	if (bVideo)
	{
		pCodec_Video = avcodec_find_encoder(AV_CODEC_ID_H264);//AV_CODEC_ID_H264
		if (!pCodec_Video) {
			printf("Can not find output encoder! (没有找到合适的编码器！)\n");
			return false;
		}
		m_OutputVideoCodec_Ctx = avcodec_alloc_context3(pCodec_Video);

		m_OutputVideoCodec_Ctx->pix_fmt = AV_PIX_FMT_YUV420P;// PIX_FMT_YUV420P;
		m_OutputVideoCodec_Ctx->width = m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->width;
		m_OutputVideoCodec_Ctx->height = m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->height;
		m_OutputVideoCodec_Ctx->time_base.num = 1;
		m_OutputVideoCodec_Ctx->time_base.den = 25;//25
		m_OutputVideoCodec_Ctx->bit_rate = 400000;
		m_OutputVideoCodec_Ctx->gop_size = 25;//10s
		m_OutputVideoCodec_Ctx->framerate = { 25, 1 };

		/* Some formats want stream headers to be separate. */
		if (m_Output_Ctx->oformat->flags & AVFMT_GLOBALHEADER)
			m_OutputVideoCodec_Ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;

		//H264 codec param
		//pCodecCtx->me_range = 16;
		//pCodecCtx->max_qdiff = 4;
		//m_OutputVideoCodec_Ctx->qcompress = 0.6;
		//m_OutputVideoCodec_Ctx->flags |= CODEC_FLAG_QSCALE;
		//m_OutputVideoCodec_Ctx->qmin = 10;
		//m_OutputVideoCodec_Ctx->qmax = 51;//50
		//m_OutputVideoCodec_Ctx->keyint_min = 10; //关键帧的最小间隔帧数，取值范围10-51
		//m_OutputVideoCodec_Ctx->refs = 2;	//运动补偿
		//Optional Param
		//m_OutputVideoCodec_Ctx->max_b_frames = 2;//0
		//m_OutputVideoCodec_Ctx->rc_max_rate = 200000;//最大码流，x264中单位kbps，ffmpeg中单位bps
		//m_OutputVideoCodec_Ctx->rc_min_rate = 512000;//最小码流
		//m_OutputVideoCodec_Ctx->rc_buffer_size = 2000000;
		// Set H264 preset and tune
		AVDictionary *param=0;
		//av_dict_set(&param, "preset", "slow", 0);//ultrafast
		//av_dict_set(&param, "tune", "zerolatency", 0);
		if (m_OutputVideoCodec_Ctx->codec_id == AV_CODEC_ID_H264)
		{
			av_dict_set(&param, "preset", "superfast", 0);
			av_dict_set(&param, "tune", "zerolatency", 0);
		}
		//H.265    
		if (m_OutputVideoCodec_Ctx->codec_id == AV_CODEC_ID_H265)
		{
			av_dict_set(&param, "x265-params", "qp=20", 0);
			av_dict_set(&param, "preset", "slow", 0);
			av_dict_set(&param, "tune", "zero-latency", 0);
		}

		if (avcodec_open2(m_OutputVideoCodec_Ctx, pCodec_Video, &param) < 0) {
			printf("Failed to open output video encoder! (编码器打开失败！)\n");
			return false;
		}
		//Add a new stream to output,should be called by the user before avformat_write_header() for muxing
		m_Output_Video_Stream = avformat_new_stream(m_Output_Ctx, pCodec_Video);
		if (m_Output_Video_Stream == NULL) {
			printf("Could not allocate video stream\n");
			return false;
		}
		m_Output_Video_Stream->time_base.num = 1;
		m_Output_Video_Stream->time_base.den = 25;//90000
		if (avcodec_parameters_from_context(m_Output_Video_Stream->codecpar, m_OutputVideoCodec_Ctx) < 0) {
			fprintf(stderr, "Failed to copy %s codec context to encoder parameters\n",
				av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
			return false;
		}
		//camera data may has a pix fmt of RGB or sth else,convert it to YUV420
		img_convert_ctx = sws_getContext(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->width, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->height,
			(enum AVPixelFormat)m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->format/*pCodecCtx_in->pix_fmt*/, m_OutputVideoCodec_Ctx->width, m_OutputVideoCodec_Ctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);//SWS_FAST_BILINEAR SWS_BICUBIC
	}
	if (bAudio)
	{
		pCodec_Audio = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if (!pCodec_Audio) {
			printf("Can not find output audio encoder! (没有找到合适的编码器！)\n");
			return false;
		}
		m_OutputAudioCodec_Ctx = avcodec_alloc_context3(pCodec_Audio);
		m_OutputAudioCodec_Ctx->channels = 2;
		m_OutputAudioCodec_Ctx->channel_layout = av_get_default_channel_layout(2);
		m_OutputAudioCodec_Ctx->sample_rate = m_Input_Audio_Ctx->streams[m_input_audiostream_index]->codecpar->sample_rate;
		m_OutputAudioCodec_Ctx->sample_fmt = pCodec_Audio->sample_fmts[0];
		m_OutputAudioCodec_Ctx->bit_rate = 115200;//64000
		m_OutputAudioCodec_Ctx->time_base.num = 1;
		m_OutputAudioCodec_Ctx->time_base.den = m_OutputAudioCodec_Ctx->sample_rate;
		/** Allow the use of the experimental AAC encoder */
		m_OutputAudioCodec_Ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
		/* Some formats want stream headers to be separate. */
		if (m_Output_Ctx->oformat->flags & AVFMT_GLOBALHEADER)
			m_OutputAudioCodec_Ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;

		//Add a new stream to output,should be called by the user before avformat_write_header() for muxing
		m_Output_Audio_Stream = avformat_new_stream(m_Output_Ctx, pCodec_Audio);
		if (m_Output_Audio_Stream == NULL) {
			printf("Could not allocate audio stream\n");
			return false;
		}
		m_Output_Audio_Stream->time_base.num = 1;
		m_Output_Audio_Stream->time_base.den = m_OutputAudioCodec_Ctx->sample_rate;
		//audio_st->codec = pCodecCtx_a;
		if (avcodec_open2(m_OutputAudioCodec_Ctx, pCodec_Audio, NULL) < 0) {
			printf("Failed to open ouput audio encoder! (编码器打开失败！)\n");
			return false;
		}
		/* copy the stream parameters to the muxer */
		if (avcodec_parameters_from_context(m_Output_Audio_Stream->codecpar, m_OutputAudioCodec_Ctx) < 0) {
			fprintf(stderr, "Failed to copy %s codec context to encoder parameters for audio\n",
				av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
			return false;
		}

	}
	//Open output URL,set before avformat_write_header() for muxing
	AVDictionary *device_param = 0;
	//if not setting rtbufsize, error messages will be shown in cmd, but you can still watch or record the stream correctly in most time
	//setting rtbufsize will erase those error messages, however, larger rtbufsize will bring latency
	//av_dict_set(&device_param, "rtbufsize", "5M", 0);
	av_dict_set(&device_param, "f", "rtsp", 0);
	av_dict_set(&device_param, "rtsp_transport", "tcp", 0);
	char errorBuff[80];
	if ((ret = avio_open2(&m_Output_Ctx->pb, ch_FilePath, AVIO_FLAG_READ_WRITE, NULL, &device_param)) < 0) {//AVIO_FLAG_READ_WRITE
		printf("Failed to open output file! (输出文件打开失败！)\n");
		fprintf(stderr, "Could not open outfile '%s': %s", ch_FilePath, av_make_error_string(errorBuff, 80, ret));
		return false;
	}
	
	//Show some Information
	av_dump_format(m_Output_Ctx, 0, ch_FilePath, 1);

	//Write File Header
	avformat_write_header(m_Output_Ctx, NULL);
	/*if (bAudio)
	{
		aud_convert_ctx = swr_alloc_set_opts(NULL,
			av_get_default_channel_layout(m_OutputAudioCodec_Ctx->channels),
			m_OutputAudioCodec_Ctx->sample_fmt,
			m_OutputAudioCodec_Ctx->sample_rate,
			av_get_default_channel_layout(m_Input_Audio_Ctx->streams[m_input_audiostream_index]->codecpar->channels),
			m_OutputAudioCodec_Ctx->sample_fmt,//sample_fmt
			m_OutputAudioCodec_Ctx->sample_rate,
			0, NULL);
		swr_init(aud_convert_ctx);
		fifo = av_audio_fifo_alloc(m_OutputAudioCodec_Ctx->sample_fmt, m_OutputAudioCodec_Ctx->channels, 1);
		output_frame_size = m_OutputAudioCodec_Ctx->frame_size;
	}*/

	return true;
	//avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	//Initialize the buffer to store YUV frames to be encoded.
	//pFrameYUV = av_frame_alloc();
	//uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_OutputVideoCodec_Ctx->width, m_OutputVideoCodec_Ctx->height, 1));
	//av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, m_OutputVideoCodec_Ctx->width, m_OutputVideoCodec_Ctx->height, 1);


}
/*
bool MediaManeger::addAVFrameVideo(AVFrame* frame)
{
	if (frame) {
		m_InputVideoQueue.push(frame);
		nb_InputVideos++;
		m_Size += frame->size;
		return true;
	}
	else {
		return false;
	}
}
bool MediaManeger::removeAVFrameVideo(AVFrame *frame)
{

}*/

MediaManeger::MediaManeger()
{
	m_input_videostream_index = -1;
	m_input_audiostream_index = -1;
	nb_InputVideos = 0;
	m_InputVideoSize = 0;
	m_FrameVideoSize = 0;
	m_FrameAudioSize = 0;
	m_Input_Format = NULL;
	av_register_all();
	//Register Device
	avdevice_register_all();
	avformat_network_init();
	my_pool.size_controller().resize(MAX_THREAD_NUMBER);
}

MediaManeger::~MediaManeger()
{
}
