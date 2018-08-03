#pragma once

#include <stdio.h>
#include <stdlib.h>
//#include "librtmp_send264.h"
#include "librtmp\rtmp.h"   
#include "librtmp\rtmp_sys.h"   
#include "librtmp\amf.h"  
//#include "sps_decode.h"

//#ifdef WIN32     
//#include <windows.h>  

//#endif 

//�����ͷ���ȣ�RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)

class RTMPSender
{
public:
	RTMPSender();
	~RTMPSender();

public:

	int RTMP264_Connect(const char* url);
	int InitSockets();

	RTMP * m_pRtmp;
};

