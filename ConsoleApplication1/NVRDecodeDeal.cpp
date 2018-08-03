#include <stdafx.h>
#include "NVRDecodeDeal.h"
NVRDecodeDeal::NVRDecodeDeal() :bRun(true), m_poolsize(10)
{

}
NVRDecodeDeal::~NVRDecodeDeal()
{

}

void NVRDecodeDeal::start()
{
	boost::function0<void> fun = boost::bind(&NVRDecodeDeal::threadProduce, this);
	boost::thread threadVideoDecode(fun);
}

void NVRDecodeDeal::threadProduce()
{

	AVPacket * packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	MediaManeger *manager = MediaManeger::Instance();
	AVRational time_base_q = { 1, AV_TIME_BASE };
	//Write PTS
	AVRational time_base = manager->m_Output_Ctx->streams[0]->time_base;//{ 1, 1000 };
	AVRational r_framerate1 = manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->r_frame_rate;//{ 50, 2 }; 
	int64_t n_sendBytes = 0;																												//Duration between 2 frames (us)
	int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));	//内部时间戳
	int64_t start_time = av_gettime();
	int64_t nowCurr = start_time;
	int currCount = 0;
	int framecnt = 0;
	int64_t currBytes = 0;
	bool blStart = false;
	while (bRun)
	{
		if (av_read_frame(manager->m_Input_Video_Ctx, packet) < 0)
		{
			av_packet_unref(packet);
			//av_usleep(20);
			continue;
		}

		if (packet->stream_index == manager->m_input_videostream_index)
		{

			framecnt++;
			if (blStart && (packet->flags & AV_PKT_FLAG_KEY))
			{

			}
			//packet->pts = av_rescale_q(currCount*calc_duration, time_base_q, time_base);
			//packet->dts = packet->pts;
			//packet->duration = av_rescale_q(calc_duration, time_base_q, time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
			//packet->pos = -1;
			//packet->pts = currCount;
			//packet->dts = packet->pts;
			n_sendBytes += packet->size;
			if (packet->pts != AV_NOPTS_VALUE)
				packet->pts = av_rescale_q(packet->pts, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->time_base, manager->m_Output_Ctx->streams[0]->time_base);
			if (packet->dts != AV_NOPTS_VALUE)
				packet->dts = av_rescale_q(packet->dts, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->time_base, manager->m_Output_Ctx->streams[0]->time_base);
			if (packet->duration > 0)
				packet->duration = av_rescale_q(packet->duration, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->time_base, manager->m_Output_Ctx->streams[0]->time_base);
			int ret = av_interleaved_write_frame(manager->m_Output_Ctx, packet);

			
			av_usleep(10);
		}
		int64_t nextCurr = av_gettime() - nowCurr;
		if (nextCurr > 1000000)
		{
			//std::cout << "manager->m_FrameVideoSize - currCount:" << manager->m_FrameVideoSize - currCount << std::endl;
			//std::cout << "n_decodenum:" << manager->m_FrameVideoSize << std::endl;//manager->m_FrameVideoSize - currCount
			std::cout << "packet->pts:" << packet->pts << " packet->dts:"<< packet->dts << std::endl;
			std::cout << "sent speed:" << (n_sendBytes - currBytes) / 1000 << " KB/S" << std::endl;
			std::cout << "1 secode packet numbers:" << (framecnt - currCount) << "frame" << std::endl;
			currCount = framecnt;
			currBytes = n_sendBytes;
			nowCurr = av_gettime();


		}
		av_packet_unref(packet);

	}
	//sws_freeContext(img_convert_ctx);
	//av_frame_free(&pFrame);
	//av_frame_free(&pFrameYUV);
	//delete[] out_buffer;
}