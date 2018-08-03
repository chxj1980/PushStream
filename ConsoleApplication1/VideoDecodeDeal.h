#pragma once
#include<boost/thread.hpp>
//#include <MediaObject.h>
#include"MediaObject.h"
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
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
}
#define BOOST_DATE_TIME_SOURCE
#define BOOST_THREAD_NO_LIB
class VideoDecodeDeal
{
public:
	VideoDecodeDeal();
	~VideoDecodeDeal();
	void threadProduce();
	void start();
	bool bRun;
	int m_poolsize;
	//int decodeFrame(void *data);

	void decodeFrame(AVFrame* pFrame, uint64_t num);
	void startSon(InputDecodeObject* pInputDecodeObject);
};