#pragma once
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
}
//#include "RTMPSender.h"


class NVRMediaObject
{
public:
	NVRMediaObject();
	~NVRMediaObject();
	bool OpenInputNVR(const char *chURL, AVDictionary *device_param);
	bool OpenOutputFromNVR(char *ch_FormatName, const char *ch_FilePath);
	int m_input_videostream_index, m_input_audiostream_index;
	AVFormatContext * m_Input_Video_Ctx;
	AVFormatContext * m_Output_Ctx;//out
	AVCodecContext * m_Input_VideoCodec_Ctx;
	AVCodec *pCodec_InputVideo;
	AVCodec *pCodec_Video; //out
	void threadProduce();
	void start();
	bool bRun;

	///////////////////ºóÐøÌí¼Ó////////////


	uint8_t sps[100];
	uint8_t pps[100];
	int spsLength ;
	int ppsLength ;
	uint8_t startcode[4] ;

	class RTMPSender * sender;
};