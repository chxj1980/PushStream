#include "stdafx.h"
#include "RTMPSender.h"

#pragma comment(lib,"WS2_32.lib")   
#pragma comment(lib,"winmm.lib")  


RTMPSender::RTMPSender()
{
	m_pRtmp = nullptr;
	//m_pRtmp = RTMP_Alloc();
}


RTMPSender::~RTMPSender()
{
}


int RTMPSender::InitSockets()
{

	/*#ifdef win32
		word version;
		wsadata wsadata;
		version = makeword(2, 2);
		return (wsastartup(version, &wsadata) == 0);
	#else
		return true;
	#endif   */
	WSADATA wsaData;
	int nRet;
	if ((nRet = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		//TRACE("WSAStartup failed");
		//exit(0);
		return false;
	}
	return true;
}
int RTMPSender::RTMP264_Connect(const char* url)
{

	InitSockets();
	RTMP * Rtmp;
	//m_pRtmp = {0};

	//m_pRtmp = RTMP_Alloc();
	Rtmp = RTMP_Alloc();

	m_pRtmp = Rtmp;

	if (m_pRtmp==NULL)
	{
		return false;
	}


	RTMP_Init(m_pRtmp);


	/*����URL*/
	if (RTMP_SetupURL(m_pRtmp, (char*)url) == FALSE)
	{
		RTMP_Free(m_pRtmp);
		return false;
	}
	/*���ÿ�д,��������,�����������������ǰʹ��,������Ч*/
	RTMP_EnableWrite(m_pRtmp);
	/*���ӷ�����*/
	if (RTMP_Connect(m_pRtmp, NULL) == FALSE)
	{
		RTMP_Free(m_pRtmp);
		return false;
	}

	/*������*/
	if (RTMP_ConnectStream(m_pRtmp, 0) == FALSE)
	{
		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);
		return false;
	}
	return true;

}
