#pragma once
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
//#include <atomic>
#include <boost/atomic/atomic.hpp>
#include <boost/lockfree/queue.hpp>
//#pragma comment(lib, "libboost_atomic-vc140-mt-1_60.lib") 
//#include <queue>
//#include <SDL/SDL.h>
//#include <SDL/SDL_thread.h>
extern "C"
{

#include "libavformat/avformat.h"
};
#define QUEUE_CAPACITY 65534

class PacketQueue
{
	public:
		PacketQueue();
		~PacketQueue();
	    bool addPacket(AVPacket* packet);
		bool removePacket(AVPacket *packet);
		

	private:
		//boost::mutex m_MutexPacket;
		//boost::atomics::atomic_int32_t nb_packets;
		//std::atomic_int32_t nb_Pckets;
		//std::atomic_int64_t m_Size;
		boost::atomics::atomic_int32_t nb_Pckets;
		boost::atomics::atomic_int64_t m_Size;
		//boost::atomics::atomic_int64_t size;
		boost::lockfree::queue<AVPacket* ,boost::lockfree::capacity<QUEUE_CAPACITY>> m_Packets;
};