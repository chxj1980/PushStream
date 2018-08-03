#include "stdafx.h"
#include "MuxingEncodeDeal.h"
//#include <MediaObject.h>
#include"MediaObject.h"
#include <boost/unordered_map.hpp>
#define MAX_THREAD_NUMBER 10
MuxingEncodeDeal::MuxingEncodeDeal() :bRun(true), bVideo(false), bAudio(false)
{

}
MuxingEncodeDeal::~MuxingEncodeDeal() {

}
//void MuxingEncodeDeal::threadProduce() {
//	int aud_next_pts = 0;
//	int vid_next_pts = 0;
//	int framecnt = 0;
//	int m_decodeVideoNums = 0;
//	int m_decodeAudioNums = 0;
//	int currCnt = 0;
//	int nb_samples = 0;
//	int64_t n_sendBytes = 0;
//	int64_t currBytes = 0;
//	AVRational time_base_q = { 1, AV_TIME_BASE };
//	int ret = -1;
//	//start decode and encode
//	int64_t start_time = av_gettime();
//	int64_t nowCurr = start_time;
//	MediaManeger *manager = MediaManeger::Instance();
//	boost::unordered_map<int, InputDecodeObject*> m_decodeVideoData, m_decodeAudioData;
//	//Write PTS
//	AVRational v_time_base = manager->m_Output_Ctx->streams[0]->time_base;//{ 1, 1000 };
//	AVRational v_r_framerate1 = manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->r_frame_rate;//{ 50, 2 }; 
//																													  //Duration between 2 frames (us)
//	int64_t v_calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(v_r_framerate1));	//内部时间戳
//	AVPacket *output_packet = new AVPacket;
//	av_init_packet(output_packet);
//	output_packet->data = NULL;
//	output_packet->size = 0;
//	AVPacket enc_pkt;
//	av_init_packet(&enc_pkt);
//	enc_pkt.data = NULL;
//	enc_pkt.size = 0;
//	while (bRun)
//	{
//		InputDecodeObject *pIDO;
//		while (manager->m_InputQueue.pop(pIDO)) {
//			if (pIDO->type == 0)//Audio
//			{
//				InputDecodeObject *pInputDecode = pIDO;
//
//				if (pInputDecode->frame) {
//					nb_samples += pInputDecode->frame->nb_samples;
//				}
//				ret = avcodec_send_frame(manager->m_OutputAudioCodec_Ctx, pInputDecode->frame);
//				while ((ret = avcodec_receive_packet(manager->m_OutputAudioCodec_Ctx, output_packet)) == 0) {
//					output_packet->stream_index = 1;// manager->m_Output_Audio_Stream->index;
//
//					AVRational time_base = manager->m_Output_Ctx->streams[1]->time_base;
//					AVRational r_framerate1 = { manager->m_Input_Audio_Ctx->streams[manager->m_input_audiostream_index]->codecpar->sample_rate, 1 };// { 44100, 1};  
//					int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));  //内部时间戳  
//
//					output_packet->pts = av_rescale_q(nb_samples*calc_duration, time_base_q, time_base);
//					output_packet->dts = output_packet->pts;
//					output_packet->duration = pInputDecode->frame->nb_samples;
//
//					//printf("audio pts : %d\n", output_packet.pts);
//					aud_next_pts = nb_samples*calc_duration;
//
//					int64_t pts_time = av_rescale_q(output_packet->pts, time_base, time_base_q);
//					int64_t now_time = av_gettime() - start_time;
//					if ((pts_time > now_time) && ((aud_next_pts + pts_time - now_time) < vid_next_pts))
//						av_usleep(pts_time - now_time);
//
//					if ((ret = av_interleaved_write_frame(manager->m_Output_Ctx, output_packet)) < 0) {
//						printf("Could not write audio frame\n");
//						//av_packet_unref(output_packet);
//						continue;
//					}
//
//					//av_packet_unref(output_packet);
//				}
//				if (pInputDecode)
//				{
//					av_frame_free(&pInputDecode->frame);
//					//delete pInputDecode->out_buffer;
//					delete pInputDecode;
//					pInputDecode = NULL;
//				}
//				m_decodeAudioNums++;
//				//m_decodeAudioData.erase(ite);
//			}
//			else if (pIDO->type == 1) {
//				InputDecodeObject *pInputDecode = pIDO;
//				avcodec_send_frame(manager->m_OutputVideoCodec_Ctx, pInputDecode->frame);
//
//				while ((ret = avcodec_receive_packet(manager->m_OutputVideoCodec_Ctx, &enc_pkt)) == 0) {
//					framecnt++;
//					enc_pkt.stream_index = manager->m_Output_Video_Stream->index;
//					//Parameters
//					//enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
//					enc_pkt.pts = av_rescale_q(framecnt*v_calc_duration, time_base_q, v_time_base);
//					enc_pkt.dts = enc_pkt.pts;
//					enc_pkt.duration = av_rescale_q(v_calc_duration, time_base_q, v_time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
//					enc_pkt.pos = -1;
//					//printf("video packet size : %d\n", enc_pkt.size);
//					n_sendBytes += enc_pkt.size;
//					vid_next_pts = framecnt*v_calc_duration; //general timebase
//
//															 //Delay
//					int64_t pts_time = av_rescale_q(enc_pkt.pts, v_time_base, time_base_q);
//					int64_t now_time = av_gettime() - start_time;
//					if ((pts_time > now_time) && ((vid_next_pts + pts_time - now_time) < aud_next_pts))
//						av_usleep(pts_time - now_time);
//
//					ret = av_interleaved_write_frame(manager->m_Output_Ctx, &enc_pkt);
//					int64_t nextCurr = av_gettime() - nowCurr;
//					if (nextCurr > 1000000)
//					{
//						std::cout << "video speed:" << (n_sendBytes - currBytes) / 1000 << " KB/S" << std::endl;//(n_sendBytes - currBytes)/1000
//						std::cout << "1 secode video packet numbers:" << (framecnt - currCnt) << "frame" << std::endl;
//						currBytes = n_sendBytes;
//						currCnt = framecnt;
//						nowCurr = av_gettime();
//
//					}
//				}
//				if (pInputDecode)
//				{
//					//delete[] pInputDecode->out_buffer;
//					av_frame_free(&pInputDecode->frame);
//
//					delete pInputDecode;
//					pInputDecode = NULL;
//				}
//				m_decodeVideoNums++;
//				av_usleep(10);
//			}
//		}
//		//av_usleep(10000);
//	}
//	av_packet_unref(output_packet);
//	av_packet_unref(&enc_pkt);
//}
void MuxingEncodeDeal::threadProduce() {
	int aud_next_pts = 0;
	int vid_next_pts = 0;
	int framecnt = 0;
	int m_decodeVideoNums = 0;
	int m_decodeAudioNums = 0;
	int currCnt = 0;
	int nb_samples = 0;
	int64_t n_sendBytes = 0;
	int64_t currBytes = 0;
	AVRational time_base_q = { 1, AV_TIME_BASE };
	int ret = -1;
	//start decode and encode
	int64_t start_time = av_gettime();
	int64_t nowCurr = start_time;
	MediaManeger *manager = MediaManeger::Instance();
	boost::unordered_map<int, InputDecodeObject*> m_decodeVideoData, m_decodeAudioData;
	//Write PTS
	AVRational v_time_base = manager->m_Output_Ctx->streams[0]->time_base;//{ 1, 1000 };
	AVRational v_r_framerate1 = manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->r_frame_rate;//{ 50, 2 }; 
	//Duration between 2 frames (us)
	int64_t v_calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(v_r_framerate1));	//内部时间戳
	AVPacket *output_packet = new AVPacket;
	av_init_packet(output_packet);
	output_packet->data = NULL;
	output_packet->size = 0;
	AVPacket enc_pkt;
	av_init_packet(&enc_pkt);
	enc_pkt.data = NULL;
	enc_pkt.size = 0;
	while (bRun)
	{
		while ((bAudio&&!bVideo)  || bAudio && av_compare_ts(vid_next_pts, time_base_q, aud_next_pts, time_base_q) > 0) {
			InputDecodeObject *pIDO;
			while (manager->m_InputAudioQueue.pop(pIDO)) {
				m_decodeAudioData.insert(std::pair<int, InputDecodeObject*>(pIDO->num, pIDO));
			}
			boost::unordered_map<int, InputDecodeObject*>::iterator ite;
			while ((ite = m_decodeAudioData.find(m_decodeAudioNums + 1)) != m_decodeAudioData.end()) {
				InputDecodeObject *pInputDecode = ite->second;

				if (pInputDecode->frame) {
					nb_samples += pInputDecode->frame->nb_samples;
				}
				ret = avcodec_send_frame(manager->m_OutputAudioCodec_Ctx, pInputDecode->frame);
				while ((ret = avcodec_receive_packet(manager->m_OutputAudioCodec_Ctx, output_packet)) == 0) {
					output_packet->stream_index = 1;// manager->m_Output_Audio_Stream->index;

					AVRational time_base = manager->m_Output_Ctx->streams[1]->time_base;
					AVRational r_framerate1 = { manager->m_Input_Audio_Ctx->streams[manager->m_input_audiostream_index]->codecpar->sample_rate, 1 };// { 44100, 1};  
					int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));  //内部时间戳  

					output_packet->pts = av_rescale_q(nb_samples*calc_duration, time_base_q, time_base);
					output_packet->dts = output_packet->pts;
					output_packet->duration = pInputDecode->frame->nb_samples;

					//printf("audio pts : %d\n", output_packet.pts);
					aud_next_pts = nb_samples*calc_duration;

					int64_t pts_time = av_rescale_q(output_packet->pts, time_base, time_base_q);
					int64_t now_time = av_gettime() - start_time;
					if ((pts_time > now_time) && ((aud_next_pts + pts_time - now_time) < vid_next_pts))
						av_usleep(pts_time - now_time);

					if ((ret = av_interleaved_write_frame(manager->m_Output_Ctx, output_packet)) < 0) {
						printf("Could not write audio frame\n");
						//av_packet_unref(output_packet);
						continue;
					}
					//av_packet_unref(output_packet);
				}
				if (pInputDecode)
				{
					av_frame_free(&pInputDecode->frame);
					//delete pInputDecode->out_buffer;
					delete pInputDecode;
					pInputDecode = NULL;
				}
				m_decodeAudioNums++;
				m_decodeAudioData.erase(ite);
			}
		}
		if ((!bAudio&&bVideo) || (bVideo && (ret=av_compare_ts(vid_next_pts, time_base_q, aud_next_pts, time_base_q)) <= 0))
		{
			InputDecodeObject *pIDO;
			while (manager->m_InputVideoQueue.pop(pIDO)) {
				m_decodeVideoData.insert(std::pair<int, InputDecodeObject*>(pIDO->num, pIDO));
			}
			boost::unordered_map<int, InputDecodeObject*>::iterator ite;
			//int u64_num = m_decodenums + 1;
			while ((ite = m_decodeVideoData.find(m_decodeVideoNums + 1))!= m_decodeVideoData.end()) {
				//m_decodeData.erase(ite);
				InputDecodeObject *pInputDecode = ite->second;
				avcodec_send_frame(manager->m_OutputVideoCodec_Ctx, pInputDecode->frame);

				while ((ret = avcodec_receive_packet(manager->m_OutputVideoCodec_Ctx, &enc_pkt)) == 0) {
					framecnt++;
					enc_pkt.stream_index = manager->m_Output_Video_Stream->index;
					//Parameters
					//enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
					enc_pkt.pts = av_rescale_q(framecnt*v_calc_duration, time_base_q, v_time_base);
					enc_pkt.dts = enc_pkt.pts;
					enc_pkt.duration = av_rescale_q(v_calc_duration, time_base_q, v_time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
					enc_pkt.pos = -1;
					//printf("video packet size : %d\n", enc_pkt.size);
					n_sendBytes += enc_pkt.size;
					vid_next_pts = framecnt*v_calc_duration; //general timebase

					//Delay
					int64_t pts_time = av_rescale_q(enc_pkt.pts, v_time_base, time_base_q);
					int64_t now_time = av_gettime() - start_time;
					if ((pts_time > now_time) && ((vid_next_pts + pts_time - now_time) < aud_next_pts))
						av_usleep(pts_time - now_time);
					/*AVPacket send_pkt;
					av_init_packet(&send_pkt);
					
					if (enc_pkt.data)
					{
						if (enc_pkt.buf)
						{
							AVBufferRef *ref = av_buffer_ref(enc_pkt.buf);
							send_pkt.buf = ref;
							send_pkt.data = ref->data;
							send_pkt.buf->data = ref->data;
							send_pkt.buf->size = enc_pkt.size;
							send_pkt.buf->buffer = enc_pkt.buf->buffer;
						}
						else
						{
						send_pkt.buf = new AVBufferRef;
						send_pkt.buf->data = new uint8_t[enc_pkt.size];
						send_pkt.buf->buffer = enc_pkt.buf->buffer;
						send_pkt.buf->size = enc_pkt.size;
						memcpy(send_pkt.buf->data, enc_pkt.buf->data, enc_pkt.size);
						send_pkt.data = send_pkt.buf->data;
						}
						send_pkt.size = enc_pkt.size;
					}
					else {
						std::cout << "video package size " << enc_pkt.size << std::endl;
					}
					send_pkt.pts = enc_pkt.pts;
					send_pkt.dts = enc_pkt.dts;
					send_pkt.duration = enc_pkt.duration;
					send_pkt.stream_index = enc_pkt.stream_index;
					send_pkt.flags = enc_pkt.flags;
					send_pkt.pos = enc_pkt.pos;
					av_packet_copy_props(&send_pkt, &enc_pkt);*/

					
					/*int64_t nowCurr = av_gettime() - pInputDecode->decodeTime;
					if (nowCurr > 1000)
					{
						if (pInputDecode)
						{
							av_frame_free(&pInputDecode->frame);
							//delete pInputDecode->out_buffer;
							delete pInputDecode;
							pInputDecode = NULL;
						}
						continue;
					}*/
					ret = av_interleaved_write_frame(manager->m_Output_Ctx, &enc_pkt);

					//av_packet_unref(&send_pkt);
					//av_packet_unref(&enc_pkt);
					int64_t nextCurr = av_gettime() - nowCurr;
					if (nextCurr > 1000000)
					{
						std::cout << "video speed:" << (n_sendBytes - currBytes) / 1000 <<" KB/S" << std::endl;//(n_sendBytes - currBytes)/1000
						std::cout << "1 secode video packet numbers:" << (framecnt - currCnt)  << "frame" << std::endl;
						currBytes = n_sendBytes;
						currCnt = framecnt;
						nowCurr = av_gettime();

					}

				}
				if (pInputDecode)
				{
					//delete[] pInputDecode->out_buffer;
					av_frame_free(&pInputDecode->frame);
					
					delete pInputDecode;
					pInputDecode = NULL;
				}
				m_decodeVideoNums++;
				//u64_num = m_decodenums + 1;
				m_decodeVideoData.erase(ite);
			}
			av_usleep(10);
		}
		

	}
	av_packet_unref(output_packet);
	av_packet_unref(&enc_pkt);
}
//void MuxingEncodeDeal::threadProduce() {
//	int aud_next_pts = 0;
//	int vid_next_pts = 0;
//	int framecnt = 0;
//	int m_decodeVideoNums = 0;
//	int m_decodeAudioNums = 0;
//	int currCnt = 0;
//	int nb_samples = 0;
//	int64_t n_sendBytes = 0;
//	int64_t currBytes = 0;
//	AVRational time_base_q = { 1, AV_TIME_BASE };
//	int ret = -1;
//	//start decode and encode
//	int64_t start_time = av_gettime();
//	int64_t nowCurr = start_time;
//	MediaManeger *manager = MediaManeger::Instance();
//	//boost::unordered_map<int, InputDecodeObject*> m_decodeVideoData, m_decodeAudioData;
//	std::vector<InputDecodeObject*> m_decodeVideoData, m_decodeAudioData;
//	//Write PTS
//	AVRational v_time_base = manager->m_Output_Ctx->streams[0]->time_base;//{ 1, 1000 };
//	AVRational v_r_framerate1 = manager->m_Input_Video_Ctx->streams[manager->m_input_videostream_index]->r_frame_rate;//{ 50, 2 }; 
//																													  //Duration between 2 frames (us)
//	int64_t v_calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(v_r_framerate1));	//内部时间戳
//	AVPacket *output_packet = new AVPacket;
//	av_init_packet(output_packet);
//	output_packet->data = NULL;
//	output_packet->size = 0;
//	AVPacket enc_pkt;
//	av_init_packet(&enc_pkt);
//	enc_pkt.data = NULL;
//	enc_pkt.size = 0;
//	while (bRun)
//	{
//		InputDecodeObject *pIDO;
//		while (manager->m_InputAudioQueue.pop(pIDO)) {
//			m_decodeAudioData.push_back(pIDO);
//		}
//		while (manager->m_InputVideoQueue.pop(pIDO)) {
//			m_decodeVideoData.push_back(pIDO);
//		}
//		std::vector<InputDecodeObject*>::iterator ite_aud = m_decodeAudioData.begin();
//		std::vector<InputDecodeObject*>::iterator ite_vid = m_decodeVideoData.begin();
//		while (ite_aud != m_decodeAudioData.end() && ite_vid != m_decodeVideoData.end())
//		{
//			if ((*ite_aud)->decodeTime < (*ite_vid)->decodeTime) 
//			{
//				InputDecodeObject *pInputDecode = (*ite_aud);
//
//				if (pInputDecode->frame) {
//					nb_samples += pInputDecode->frame->nb_samples;
//				}
//				ret = avcodec_send_frame(manager->m_OutputAudioCodec_Ctx, pInputDecode->frame);
//				while ((ret = avcodec_receive_packet(manager->m_OutputAudioCodec_Ctx, output_packet)) == 0) {
//					output_packet->stream_index = 1;// manager->m_Output_Audio_Stream->index;
//
//					AVRational time_base = manager->m_Output_Ctx->streams[1]->time_base;
//					AVRational r_framerate1 = { manager->m_Input_Audio_Ctx->streams[manager->m_input_audiostream_index]->codecpar->sample_rate, 1 };// { 44100, 1};  
//					int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));  //内部时间戳  
//
//					output_packet->pts = av_rescale_q(nb_samples*calc_duration, time_base_q, time_base);
//					output_packet->dts = output_packet->pts;
//					output_packet->duration = pInputDecode->frame->nb_samples;
//
//					//printf("audio pts : %d\n", output_packet.pts);
//					aud_next_pts = nb_samples*calc_duration;
//
//					int64_t pts_time = av_rescale_q(output_packet->pts, time_base, time_base_q);
//					int64_t now_time = av_gettime() - start_time;
//					if ((pts_time > now_time) && ((aud_next_pts + pts_time - now_time) < vid_next_pts))
//						av_usleep(pts_time - now_time);
//
//					if ((ret = av_interleaved_write_frame(manager->m_Output_Ctx, output_packet)) < 0) {
//						printf("Could not write audio frame\n");
//						//av_packet_unref(output_packet);
//						continue;
//					}
//					//av_packet_unref(output_packet);
//				}
//				if (pInputDecode)
//				{
//					av_frame_free(&pInputDecode->frame);
//					//delete pInputDecode->out_buffer;
//					delete pInputDecode;
//					pInputDecode = NULL;
//				}
//				m_decodeAudioNums++;
//				ite_aud=m_decodeAudioData.erase(ite_aud);
//			}
//			else {
//				InputDecodeObject *pInputDecode = (*ite_vid);
//				avcodec_send_frame(manager->m_OutputVideoCodec_Ctx, pInputDecode->frame);
//
//				while ((ret = avcodec_receive_packet(manager->m_OutputVideoCodec_Ctx, &enc_pkt)) == 0) {
//					framecnt++;
//					enc_pkt.stream_index = manager->m_Output_Video_Stream->index;
//					//Parameters
//					//enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
//					enc_pkt.pts = av_rescale_q(framecnt*v_calc_duration, time_base_q, v_time_base);
//					enc_pkt.dts = enc_pkt.pts;
//					enc_pkt.duration = av_rescale_q(v_calc_duration, time_base_q, v_time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
//					enc_pkt.pos = -1;
//					//printf("video packet size : %d\n", enc_pkt.size);
//					n_sendBytes += enc_pkt.size;
//					vid_next_pts = framecnt*v_calc_duration; //general timebase
//
//															 //Delay
//					int64_t pts_time = av_rescale_q(enc_pkt.pts, v_time_base, time_base_q);
//					int64_t now_time = av_gettime() - start_time;
//					if ((pts_time > now_time) && ((vid_next_pts + pts_time - now_time) < aud_next_pts))
//						av_usleep(pts_time - now_time);
//
//					ret = av_interleaved_write_frame(manager->m_Output_Ctx, &enc_pkt);
//
//					//av_packet_unref(&send_pkt);
//					//av_packet_unref(&enc_pkt);
//					int64_t nextCurr = av_gettime() - nowCurr;
//					if (nextCurr > 1000000)
//					{
//						std::cout << "video speed:" << (n_sendBytes - currBytes) / 1000 << " KB/S" << std::endl;//(n_sendBytes - currBytes)/1000
//						std::cout << "1 secode video packet numbers:" << (framecnt - currCnt) << "frame" << std::endl;
//						currBytes = n_sendBytes;
//						currCnt = framecnt;
//						nowCurr = av_gettime();
//
//					}
//
//				}
//				if (pInputDecode)
//				{
//					//delete[] pInputDecode->out_buffer;
//					av_frame_free(&pInputDecode->frame);
//
//					delete pInputDecode;
//					pInputDecode = NULL;
//				}
//				m_decodeVideoNums++;
//				ite_vid = m_decodeVideoData.erase(ite_vid);
//			}
//		}
//	}
//	av_packet_unref(output_packet);
//	av_packet_unref(&enc_pkt);
//}
void MuxingEncodeDeal::start()
{
	boost::function0<void> fun = boost::bind(&MuxingEncodeDeal::threadProduce, this);
	boost::thread threadMuxingencode(fun);
}