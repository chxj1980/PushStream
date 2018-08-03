#include "stdafx.h"
#include"PacketQueue.h"
//#include <PacketQueue.h>

PacketQueue::PacketQueue()
	:nb_Pckets(0), m_Size(0)
{

}

PacketQueue::~PacketQueue()
{

}

bool PacketQueue::addPacket(AVPacket* packet)
{
	if (packet) {
		m_Packets.push(packet);
		nb_Pckets++;
		m_Size += packet->size;
		return true;
	}
	else {
		return false;
	}
}

bool PacketQueue::removePacket(AVPacket *packet)
{
	if (packet) {
		av_packet_unref(packet);
		packet = NULL;
	}
	if (m_Packets.pop(packet)) {
		nb_Pckets--;
		m_Size -= packet->size;
		return true;
	}
	else
		return false;
}