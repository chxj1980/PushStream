#include "stdafx.h"



#include "basefunction.h"
#include<boost/thread.hpp>

//#include "RTMPSender.h"
//**//
#include <stdio.h>
#include <stdlib.h>
//#include "librtmp_send264.h"
#include "librtmp\rtmp.h"   
#include "librtmp\rtmp_sys.h"   
#include "librtmp\amf.h"  
#pragma comment(lib,"WS2_32.lib")   
#pragma comment(lib,"winmm.lib")  
//**//

//#include <NVRMediaObject.h>
#include"NVRMediaObject.h"
#include <windows.h>
#include <iostream>

//**//
//#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//RTMP * m_pRtmp;
#include "RTMPStream.h"
#include "SpsDecode.h"

CRTMPStream rtmpSender;




FILE *fp;

const char nal_start[] = {0,0,0,1};



//**//
NVRMediaObject::NVRMediaObject()
{
	m_input_videostream_index = -1;
	m_input_audiostream_index = -1;
	avcodec_register_all();
	av_register_all();
	//Register Device
	avdevice_register_all();
	avformat_network_init();
	bRun = true;

	//

	 spsLength=0;
	 ppsLength=0;
	 startcode[0]=00;
	 startcode[1] = 00;
	 startcode[2] = 00;
	 startcode[3] = 01;
	 
}

NVRMediaObject::~NVRMediaObject()
{
	bRun = false;
	if (m_Input_VideoCodec_Ctx)
		avcodec_free_context(&m_Input_VideoCodec_Ctx);
	avio_close(m_Output_Ctx->pb);
	avformat_free_context(m_Input_Video_Ctx);
	avformat_free_context(m_Output_Ctx);
}

bool NVRMediaObject::OpenInputNVR(const char *chURL, AVDictionary *device_param)
{
	if (chURL)
	{
		if (avformat_open_input(&m_Input_Video_Ctx, chURL, NULL, &device_param) != 0) {

			printf("Couldn't open input video stream.���޷�����������\n");
			return false;
		}

		//input video initialize
		if (avformat_find_stream_info(m_Input_Video_Ctx, &device_param) < 0)//avformat_find_stream_info
		{
			printf("Couldn't find video stream information.���޷���ȡ����Ϣ��\n");
			return false;
		}



		for (int i = 0; i < m_Input_Video_Ctx->nb_streams; i++)
		{
			if (m_Input_Video_Ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				m_input_videostream_index = i;
				break;
			}
			if (m_input_videostream_index == -1)
			{
				printf("Couldn't find a video stream.��û���ҵ���Ƶ����\n");
				return false;
			}
		}


		// ��Ƶ����������
		if (!(pCodec_InputVideo = avcodec_find_decoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id)))
		{
			printf("Can not find input video decoder! (û���ҵ����ʵĽ�������)\n");
			return false;
		}

		m_Input_VideoCodec_Ctx = avcodec_alloc_context3(pCodec_InputVideo);
		/* Copy codec parameters from input stream to output codec context */
		if (avcodec_parameters_to_context(m_Input_VideoCodec_Ctx, m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
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
bool NVRMediaObject::OpenOutputFromNVR(char *ch_FormatName, const char *ch_FilePath)
{
	//output initialize
	avformat_alloc_output_context2(&m_Output_Ctx, NULL, ch_FormatName, ch_FilePath);//"flv"
	/*pCodec_Video = avcodec_find_encoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id);//AV_CODEC_ID_H264
	if (!pCodec_Video) {
		printf("Can not find output encoder! (û���ҵ����ʵı�������)\n");
		return false;
	}*/
	AVDictionary* options = nullptr;
	//av_dict_set(&options, "enable-demuxer", "rtsp", 0);
	//av_dict_set(&options, "enable-muxer", "rtsp", 0);
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	int ret = -1;
	char errorBuff[80];
	ret = avio_open2(&m_Output_Ctx->pb, ch_FilePath, AVIO_FLAG_WRITE, NULL, &options);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "open avio failed");
		fprintf(stderr, "Could not open outfile '%s': %s", ch_FilePath, av_make_error_string(errorBuff, 80, ret));
		goto Error;
	}
	AVCodec *p_Codec_Video = avcodec_find_encoder(m_Input_Video_Ctx->streams[m_input_videostream_index]->codecpar->codec_id);
	if (!p_Codec_Video) {
		printf("Can not find output encoder! (û���ҵ����ʵı�������)\n");
		goto Error;
	}
	AVStream * stream = avformat_new_stream(m_Output_Ctx, pCodec_InputVideo);
	stream->time_base.num = m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.num;
	stream->time_base.den = m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base.den;//90000
	if (avcodec_parameters_from_context(stream->codecpar, m_Input_VideoCodec_Ctx) < 0) {
		fprintf(stderr, "Failed to copy %s codec context to encoder parameters\n",
			av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
		goto Error;
	}
	
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

	//RTMPSender sender;

void NVRMediaObject::start()
{
	//д�ļ�
	fp = fopen("123.h264", "wb+");
	//RTMP��ʼ��
	bool bRet = rtmpSender.Connect("rtmp://47.97.169.87:1935/openstream/test2");

	//rtmpSender.SendH264File("123.h264");

	boost::function0<void> fun = boost::bind(&NVRMediaObject::threadProduce, this);
	boost::thread threadVideoDecode(fun);
}


const int delttime = 40;
void NVRMediaObject::threadProduce()
{

	AVPacket * packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	AVRational time_base_q = { 1, AV_TIME_BASE };
	//Write PTS
	AVRational time_base = m_Output_Ctx->streams[0]->time_base;//{ 1, 1000 };
	AVRational r_framerate1 = m_Input_Video_Ctx->streams[m_input_videostream_index]->r_frame_rate;//{ 50, 2 }; 
	int64_t n_sendBytes = 0;																												//Duration between 2 frames (us)
	int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));	//�ڲ�ʱ���
	int64_t start_time = av_gettime();
	int64_t nowCurr = start_time;
	int currCount = 0;
	int framecnt = 0;
	int64_t currBytes = 0;
	bool blStart = false;
	int flag = 1;

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;


	unsigned int tick = 0;
	while (bRun)
	{
		//**//

		AVStream *in_stream, *out_stream;

		//int ret = av_read_frame(m_Input_Video_Ctx, packet);

		int ret = av_read_frame(m_Input_Video_Ctx, &pkt);

		if (ret < 0)
			break;
		in_stream = m_Input_Video_Ctx->streams[pkt.stream_index];
		out_stream = m_Output_Ctx->streams[pkt.stream_index];

		AVPacket tmppkt;

		NaluUnit naluUnit;
		

		if (in_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{

			if (flag&&(pkt.flags &AV_PKT_FLAG_KEY))
			//if (pkt.flags &AV_PKT_FLAG_KEY)
			{

				pkt.data[0] = 0x00;
				pkt.data[1] = 0x00;
				pkt.data[2] = 0x00;
				pkt.data[3] = 0x01;
				fwrite(pkt.data, pkt.size, 1, fp);
				/////////////////////////////////////////////

				//****************planA********begin*********///////////////

				//h264����
				//rtmpSender.m_nFileBufSize += pkt.size;
				rtmpSender.m_nFileBufSize = pkt.size;


				//memcpy(rtmpSender.m_pFileBuf+rtmpSender.m_nwritePos, pkt.data, pkt.size);
				//rtmpSender.m_nwritePos = rtmpSender.m_nFileBufSize;

				memcpy(rtmpSender.m_pFileBuf  , pkt.data, pkt.size );


				RTMPMetadata metaData;
				memset(&metaData, 0, sizeof(RTMPMetadata));
				// ��ȡSPS֡
				rtmpSender.ReadOneNaluFromBuf(naluUnit);
				metaData.nSpsLen = naluUnit.size;
				memcpy(metaData.Sps, naluUnit.data, naluUnit.size);
				//��ȡPPS֡
				rtmpSender.ReadOneNaluFromBuf(naluUnit);
				metaData.nPpsLen = naluUnit.size;
				memcpy(metaData.Pps, naluUnit.data, naluUnit.size);
				// ����SPS,��ȡ��Ƶͼ�������Ϣ
				int width = 0, height = 0;
				h264_decode_sps(metaData.Sps, metaData.nSpsLen, width, height);
				metaData.nWidth = width;
				metaData.nHeight = height;
				metaData.nFrameRate = 25;
				// ����MetaData
				rtmpSender.SendMetadata(&metaData);
				while (rtmpSender.ReadOneNaluFromBuf(naluUnit))
				{
					bool bKeyframe = (naluUnit.type == 0x05) ? TRUE : FALSE;
					// ����H264����֡
					rtmpSender.SendH264Packet(naluUnit.data, naluUnit.size, bKeyframe, tick);
					msleep(delttime);
					tick += delttime;
				}
				rtmpSender.m_nCurPos = 0;
				memset(rtmpSender.m_pFileBuf, 0, FILEBUFSIZE);
				//****************planA********end*********///////////////
				flag = 0;
			}
			else if(flag==0)
			{

				//****************planA********begin*********///////////////
				pkt.data[0] = 0x00;
				pkt.data[1] = 0x00;
				pkt.data[2] = 0x00;
				pkt.data[3] = 0x01;

				fwrite(pkt.data, pkt.size, 1, fp);

				//rtmpSender.m_nFileBufSize += pkt.size;

				rtmpSender.m_nFileBufSize = pkt.size;

				//memset(rtmpSender.m_pFileBuf, 0, sizeof(rtmpSender.m_pFileBuf));
				
				//memcpy(rtmpSender.m_pFileBuf + rtmpSender.m_nwritePos, pkt.data, pkt.size);
				//rtmpSender.m_nwritePos = rtmpSender.m_nFileBufSize;


				memcpy(rtmpSender.m_pFileBuf  , pkt.data, pkt.size );
				//rtmpSender.m_nwritePos = rtmpSender.m_nFileBufSize;

				while (rtmpSender.ReadOneNaluFromBuf(naluUnit))
				{
					bool bKeyframe = (naluUnit.type == 0x05) ? TRUE : FALSE;
					// ����H264����֡
					rtmpSender.SendH264Packet(naluUnit.data, naluUnit.size, bKeyframe, tick);
					msleep(delttime);
					tick += delttime;
					//printf("send");
				}


				rtmpSender.m_nCurPos = 0;
				//m_pFileBuf = new unsigned char[FILEBUFSIZE];
				memset(rtmpSender.m_pFileBuf, 0, FILEBUFSIZE);
				//****************planA********end*********///////////////
			}


				//fwrite(startcode, 4, 1, fp);
				//fwrite(sps, spsLength, 1, fp);
				//fwrite(startcode, 4, 1, fp);
				//fwrite(pps, ppsLength, 1, fp);

				//packet->data[0] = 0x00;
				//packet->data[1] = 0x00;
				//packet->data[2] = 0x00;
				//packet->data[3] = 0x01;

				//memcpy(rtmppacket->m_body, packet->data, packet->size);

				//rtmppacket->m_packetType = RTMP_PACKET_TYPE_VIDEO;
				//rtmppacket->m_nBodySize = packet->size;
				//rtmppacket->m_nChannel = 0x04;
				//rtmppacket->m_nTimeStamp = 0;
				//rtmppacket->m_hasAbsTimestamp = 0;
				//rtmppacket->m_headerType = RTMP_PACKET_SIZE_MEDIUM;

				//rtmppacket->m_nInfoField2 = m_pRtmp->m_stream_id;
				////pre_frame_time = timestamp;
				//RTMP_SendPacket(m_pRtmp, rtmppacket, true);
				//fwrite(startcode, 4, 1, fp);
				//fwrite(nal_start, 4, 1, fp);
				//fwrite(packet->data + 4, packet->size - 4, 1, fp);



				//printf("s");
				
				//free(rtmppacket);    //�ͷ��ڴ�
				
			//pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
			//pkt.pos = -1;

			//pkt.stream_index = 0;
			//ret = av_interleaved_write_frame(oc, &pkt);
		}


		//	n_sendBytes += packet->size;
		//	if (packet->pts != AV_NOPTS_VALUE)
		//		packet->pts = av_rescale_q(packet->pts, m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base, m_Output_Ctx->streams[0]->time_base);

		//	if (packet->dts != AV_NOPTS_VALUE)
		//		packet->dts = av_rescale_q(packet->dts, m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base,  m_Output_Ctx->streams[0]->time_base);

		//	if (packet->duration > 0)
		//		packet->duration = av_rescale_q(packet->duration, m_Input_Video_Ctx->streams[m_input_videostream_index]->time_base, m_Output_Ctx->streams[0]->time_base);
		//	int ret = av_interleaved_write_frame(m_Output_Ctx, packet);
		av_packet_unref(packet);

	}


}