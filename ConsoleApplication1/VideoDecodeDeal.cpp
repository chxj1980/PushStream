#include "stdafx.h"
#include "VideoDecodeDeal.h"
#include <windows.h>
#include <iostream>
#include <boost/threadpool.hpp>
extern "C"
{
#include <SDL.h>
#include <SDL_thread.h>
}

//VideoDecodeDeal::
//int decodeFrame(void *data)//VideoDecodeDeal::decodeFrame(AVFrame* pFrame, uint64_t num)
void VideoDecodeDeal::decodeFrame(AVFrame* pFrame, uint64_t num)
{
	//InputDecodeObject* media = (InputDecodeObject*) data;
	//MediaManeger *manager = MediaManeger::Instance();
	//if (manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->format != AV_PIX_FMT_YUV420P)
	//{
	//	AVFrame	*pFrameYUV = av_frame_alloc();

	//	SwsContext *img_convert_ctx = sws_getContext(manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->width, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->height,
	//		(enum AVPixelFormat)manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->format/*pCodecCtx_in->pix_fmt*/, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);//SWS_FAST_BILINEAR
	//	int iSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
	//	uint8_t *out_buffer = new uint8_t[iSize];
	//	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
	//	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, manager->m_OutputVideoCodec_Ctx->height, pFrameYUV->data, pFrameYUV->linesize);
	//	//sws_scale(manager->img_convert_ctx, (const uint8_t* const*)media->frame->data, media->frame->linesize, 0, manager->m_OutputVideoCodec_Ctx->height, pFrameYUV->data, pFrameYUV->linesize);
	//	pFrameYUV->width = pFrame->width;
	//	pFrameYUV->height = pFrame->height;
	//	//pFrameYUV->width = media->frame->width;
	//	//pFrameYUV->height = media->frame->height;
	//	pFrameYUV->format = AV_PIX_FMT_YUV420P;
	//	pFrameYUV->pkt_dts = av_gettime();

	//	InputDecodeObject *pInputDecode = new InputDecodeObject();
	//	pInputDecode->frame = pFrameYUV;
	//	pInputDecode->num = num;
	//	//pInputDecode->num = media->num;
	//	pInputDecode->type = 1;
	//	//pInputDecode->out_buffer = out_buffer;
	//	pInputDecode->decodeTime = av_gettime();
	//	//manager->m_InputQueue.push(pInputDecode);
	//	manager->m_InputVideoQueue.push(pInputDecode);
	//	sws_freeContext(img_convert_ctx);
	//	av_frame_free(&pFrame);
	//}
	//else {
	//	InputDecodeObject *pInputDecode = new InputDecodeObject();
	//	pInputDecode->frame = pFrame;
	//	pInputDecode->num = num;
	//	pInputDecode->type = 1;
	//	//pInputDecode->num = media->num;
	//	//pInputDecode->size = pFrame->;
	//	//pInputDecode->out_buffer = out_buffer;
	//	pInputDecode->decodeTime = av_gettime();
	//	//manager->m_InputQueue.push(pInputDecode);
	//	manager->m_InputVideoQueue.push(pInputDecode);
	//}
	//delete pFrame;
	//delete[] out_buffer;
	
	//av_frame_free(&media->frame);
	//delete media;
	
	//delete[] out_buffer;
	return ;
}
VideoDecodeDeal::VideoDecodeDeal():bRun(true), m_poolsize(10)
{

}
VideoDecodeDeal::~VideoDecodeDeal()
{

}
void VideoDecodeDeal::threadProduce()
{

	AVPacket * packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	//VideoDecodeDeal vdd;
	//boost::threadpool::pool my_pool(MAX_THREAD_NUMBER);
	MediaManeger *manager = MediaManeger::Instance();
	AVFrame	*pFrame = av_frame_alloc();
	//SwsContext *img_convert_ctx = sws_getContext(manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->width, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->height,
	//				(enum AVPixelFormat)manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->format/*pCodecCtx_in->pix_fmt*/, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	int64_t nowCurr = av_gettime();
	int currCount = 0;
	while (bRun)
	{
		if (av_read_frame(manager->m_Input_Video_Ctx, packet) != 0)
		{
			//av_packet_unref(packet);
			continue;
		}
		
		if (packet->stream_index == manager->m_input_videostream_index)
		{
			int ret = avcodec_send_packet(manager->m_Input_VideoCodec_Ctx, packet);
			if (ret != 0) {
				av_log(NULL, AV_LOG_ERROR, "send Decoding failed\n");
			}
			while (true) {
				InputDecodeObject *pInputDecode = new InputDecodeObject();
				pInputDecode->type = 1;
				pInputDecode->decodeTime = av_gettime();
				if (avcodec_receive_frame(manager->m_Input_VideoCodec_Ctx, pFrame) < 0)
				{
					//av_frame_free(&pFrame);
					if (pInputDecode)
					{
						delete pInputDecode;
					}
					break;
				}
				//iCout++;
				manager->m_FrameVideoSize++;
				uint64_t n_decodenum = manager->m_FrameVideoSize;
				/*InputDecodeObject *pInputDecode = new InputDecodeObject();
				pInputDecode->frame = pFrame;
				pInputDecode->num = n_decodenum;
				this->startSon(pInputDecode);*/
				//boost::thread tVideoDecode(&VideoDecodeDeal::decodeFrame, pFrame, n_decodenum);
				//boost::function<void()> fun = boost::bind(&VideoDecodeDeal::decodeFrame, this, pFrame, n_decodenum);//AVFrame*, uint64_t
				//boost::thread tVideoDecode(boost::bind(&VideoDecodeDeal::decodeFrame, &vdd, pFrame, n_decodenum));
				//av_frame_copy(pFrame0, pFrame);
				//pFrame0->width = pFrame->width;
				//pFrame0->height = pFrame->height;
				//manager->my_pool.schedule(boost::bind(&VideoDecodeDeal::decodeFrame, this, pFrame, n_decodenum));
				
				AVFrame	*pFrameYUV = av_frame_alloc();
				int iSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
				uint8_t *out_buffer = (uint8_t *)av_malloc(iSize);
				av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
				sws_scale(manager->img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, manager->m_OutputVideoCodec_Ctx->height, pFrameYUV->data, pFrameYUV->linesize);
				pFrameYUV->width = pFrame->width;
				pFrameYUV->height = pFrame->height;
				pFrameYUV->format = AV_PIX_FMT_YUV420P;
				pFrameYUV->pkt_dts = av_gettime();
				//pFrameYUV->pkt_pos = iCout;
				//pFrameYUV->pkt_pts = pFrameYUV->pkt_dts;
				//std::cout << "pFrame->pkt_dts:" << pFrame->pkt_dts << std::endl;
				//std::cout << "pFrame->pkt_pts:" << pFrame->pkt_pts << std::endl;
				//std::cout << "pFrame->pts:" << pFrame->pts << std::endl;
				//std::cout << "m_FrameVideoSize:" << manager->m_FrameVideoSize << std::endl;
				
				pInputDecode->frame = pFrameYUV;
				pInputDecode->num = n_decodenum;

				manager->m_InputVideoQueue.push(pInputDecode); 
				//manager->m_InputQueue.push(pInputDecode);
				//av_usleep(50);
				//delete[] out_buffer;
			}
			
			av_usleep(10);
		}
		int64_t nextCurr = av_gettime() - nowCurr;
		if (nextCurr > 1000000)
		{
			//std::cout << "manager->m_FrameVideoSize - currCount:" << manager->m_FrameVideoSize - currCount << std::endl;
			//std::cout << "n_decodenum:" << manager->m_FrameVideoSize << std::endl;//manager->m_FrameVideoSize - currCount
			currCount = manager->m_FrameVideoSize;
			nowCurr = av_gettime();

		}
		//av_packet_unref(packet);
	}
	av_packet_unref(packet);
	av_frame_free(&pFrame);
	//sws_freeContext(img_convert_ctx);
	//av_frame_free(&pFrame);
	//av_frame_free(&pFrameYUV);
	//delete[] out_buffer;
}
void VideoDecodeDeal::start()
{
	boost::function0<void> fun = boost::bind(&VideoDecodeDeal::threadProduce, this);
	boost::thread threadVideoDecode(fun);
}

DWORD WINAPI ScreenCapThreadProc(LPVOID lpParam)
{
	//InputDecodeObject* media = (InputDecodeObject*)lpParam;
	//AVFrame	*pFrameYUV = av_frame_alloc();
	//MediaManeger *manager = MediaManeger::Instance();
	//SwsContext *img_convert_ctx = sws_getContext(manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->width, manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->height,
	//	AV_PIX_FMT_RGB24/*(enum AVPixelFormat)manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->codecpar->formatpCodecCtx_in->pix_fmt*/, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);//SWS_FAST_BILINEAR
	//int iSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
	//uint8_t *out_buffer = new uint8_t[iSize];
	////uint8_t *out_buffer = (uint8_t *)av_malloc(iSize);
	////boost::shared_ptr<uint8_t> out_buffer(new uint8_t[iSize]);
	//av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, manager->m_OutputVideoCodec_Ctx->width, manager->m_OutputVideoCodec_Ctx->height, 1);
	////sws_scale(manager->img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, manager->m_OutputVideoCodec_Ctx->height, pFrameYUV->data, pFrameYUV->linesize);
	//sws_scale(img_convert_ctx, (const uint8_t* const*)media->frame->data, media->frame->linesize, 0, manager->m_OutputVideoCodec_Ctx->height, pFrameYUV->data, pFrameYUV->linesize);
	////pFrameYUV->width = pFrame->width;
	////pFrameYUV->height = pFrame->height;
	//pFrameYUV->width = media->frame->width;
	//pFrameYUV->height = media->frame->height;
	//pFrameYUV->format = AV_PIX_FMT_YUV420P;
	//pFrameYUV->pkt_dts = av_gettime();
	////pFrameYUV->pkt_pos = iCout;
	////pFrameYUV->pkt_pts = pFrameYUV->pkt_dts;
	////std::cout << "pFrame->pkt_dts:" << pFrame->pkt_dts << std::endl;
	////std::cout << "pFrame->pkt_pts:" << pFrame->pkt_pts << std::endl;
	////std::cout << "pFrame->pts:" << pFrame->pts << std::endl;
	////std::cout << "pFrame->pos:" << manager->m_FrameVideoSize << std::endl;
	//InputDecodeObject *pInputDecode = new InputDecodeObject();
	//pInputDecode->frame = pFrameYUV;
	////pInputDecode->num = num;
	//pInputDecode->num = media->num;
	//pInputDecode->type = 1;
	////pInputDecode->out_buffer = out_buffer;
	//pInputDecode->decodeTime = av_gettime();
	////manager->m_InputQueue.push(pInputDecode);
	//manager->m_InputVideoQueue.push(pInputDecode);
	////delete[] out_buffer;
	////av_frame_free(&pFrame);
	//av_frame_free(&media->frame);
	//sws_freeContext(img_convert_ctx);
	//delete media;
	return 0;
}

void VideoDecodeDeal::startSon(InputDecodeObject* pInputDecodeObject)
{
	CreateThread(NULL, 0, ScreenCapThreadProc, (LPVOID)pInputDecodeObject, 0, NULL);
	//boost::function2<void, AVFrame*, uint64_t> fun = boost::bind(&VideoDecodeDeal::decodeFrame, this, pFrame, n_decodenum); //AVFrame*, uint64_t
	//boost::thread tVideoDecode(fun);
	//VideoDecodeDeal cdd;
	//boost::thread thrd(boost::bind(&VideoDecodeDeal::decodeFrame,this, pFrame, n_decodenum));
	//SDL_CreateThread(decodeFrame, "", pInputDecodeObject);
}