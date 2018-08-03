#pragma once
#include<boost/thread.hpp>
#include <boost/threadpool.hpp>

//#include <Singleton.h>
#include"Singleton.h"
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#include <boost/atomic/atomic.hpp>
#include <boost/lockfree/queue.hpp>
//#include <boost/unordered_map.hpp>
//#include <PacketQueue.h>
#define QUEUE_CAPACITY 6000//65534
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
#define MAX_THREAD_NUMBER 15
struct InputDecodeObject {
	AVFrame* frame;
	uint64_t num;
	uint64_t decodeTime;
	//uint8_t *out_buffer;
	int type;
};
class MediaManeger :public Singleton<MediaManeger>
{
public:
	/*static MediaObject *instance() {
		if (!manager)
			manager = new MediaObject();
		return manager;
	}*/
	bool OpenVideoCapture(char *chInputVideo, AVDictionary *device_param);
	bool OpenInputNVR(char *chURL, AVDictionary *device_param);
	bool OpenAudioCapture(const char *chInputAudio, AVDictionary *device_param);
	bool OpenOutput(char *ch_FormatName, char *ch_FilePath, bool bVideo, bool bAudio);
	bool OpenOutputFromNVR(char *ch_FormatName, char *ch_FilePath);
	bool OpenOutputFlv(char *ch_FilePath,bool bVideo,bool bAudio);
	bool OpenOutputRtsp(char *ch_FilePath, bool bVideo, bool bAudio);
	//bool addAVFrameVideo(AVFrame* frame);
	//bool removeAVFrameVideo(AVFrame *frame);
	MediaManeger();
	virtual ~MediaManeger();
	int m_input_videostream_index, m_input_audiostream_index;
	// 定义输入音视频及输出上下文
	AVFormatContext * m_Input_Video_Ctx, *m_Input_Audio_Ctx, *m_Output_Ctx;
	//定义输入音视频编码器上下文
	AVCodecContext * m_Input_VideoCodec_Ctx, *m_Input_AudioCodec_Ctx, *m_OutputVideoCodec_Ctx, *m_OutputAudioCodec_Ctx;
	AVCodec* pCodec_InputVideo, *pCodec_InputAudio, *pCodec_Video, *pCodec_Audio;
	// 定义输出音视频流
	AVStream* m_Output_Video_Stream, *m_Output_Audio_Stream;
	AVInputFormat * m_Input_Format;
	SwsContext *img_convert_ctx;
	//SwrContext *aud_convert_ctx;
	boost::atomics::atomic_int32_t nb_InputVideos;
	boost::atomics::atomic_int64_t m_InputVideoSize;
	boost::atomics::atomic_int64_t m_FrameVideoSize;
	boost::atomics::atomic_int64_t m_FrameAudioSize;
	boost::lockfree::queue<InputDecodeObject*, boost::lockfree::capacity<QUEUE_CAPACITY>> m_InputVideoQueue;
	boost::lockfree::queue<InputDecodeObject*, boost::lockfree::capacity<QUEUE_CAPACITY>> m_InputAudioQueue;
	//boost::lockfree::queue<InputDecodeObject*, boost::lockfree::capacity<QUEUE_CAPACITY>> m_InputQueue;
	//AVAudioFifo *fifo;
	//int output_frame_size;
	boost::threadpool::pool my_pool;
	
private:
	//MediaManeger();
	//virtual ~MediaManeger();
	//AVFrame *pFrame, *pFrameYUV;
protected:

	//static MediaObject *manager;
	//boost::mutex m_MutexPacket;
	//boost::atomics::atomic_int32_t nb_packets;
	//std::atomic_int32_t nb_Pckets;
	//std::atomic_int64_t m_Size;
	//boost::atomics::atomic_int32_t nb_Pckets;
	//boost::atomics::atomic_int64_t m_Size;
	//boost::atomics::atomic_int64_t size;
	//boost::lockfree::queue<AVPacket*, boost::lockfree::capacity<QUEUE_CAPACITY>> m_Packets;
};