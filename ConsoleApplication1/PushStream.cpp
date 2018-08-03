// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//
#define USEFILTER 0
#include "stdafx.h"

extern "C"
{
#include <SDL.h>
#include <SDL_thread.h>
}
#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp> 
#include <windows.h>
#include <vector>
#include <iostream>
#include "IniFile.h"
#include"NVRMediaObject.h"
//#include <NVRMediaObject.h>

#include "DS_AudioVideoDevices.h"
#include "AudioDecodeDeal.h"
#include "VideoDecodeDeal.h"
#include "MuxingEncodeDeal.h"
#include "NVRDecodeDeal.h"
//#define _CRT_SECURE_NO_WARNINGS
#undef main
using namespace std;
//using namespace ConsoleApplication1;


char *dup_wchar_to_char(const wchar_t *w)//szStr
{
	/*int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;*/
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *)av_malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}

#define BUFFER_SIZE 100
static const char *MYCONFIGFILE = "Mycfg";
static const char *CLIENT_SECTION = "ClientConfig";

int main(int argv, char* argc[])
{
	int m_CameraNumbers = -1;
	char chConn[600];
	char chUrl[600];
	int ret = 0;
	if ((m_CameraNumbers = read_config_int(CLIENT_SECTION, "CameraNumbers", -1, MYCONFIGFILE)) == -1)
	{
		perror("read CameraNumbers failed in ini file. \n");
		ret = -1;
		getchar();
		return ret;
	}
	if (!read_config_string(CLIENT_SECTION, "CHANNELCONN", chConn, 600, NULL, MYCONFIGFILE))
	{
		perror("read ini file CHANNELCONN fail\n");
		ret = -2;
		getchar();
		return ret;
	}
	if (!read_config_string(CLIENT_SECTION, "PUSHURL", chUrl, 600, NULL, MYCONFIGFILE))
	{
		perror("read ini file PUSHURL fail\n");
		ret = -3;
		getchar();
		return ret;
	}
	string str1(chConn);
	vector<string> SplitVec_CONN;
	boost::split(SplitVec_CONN, str1, boost::is_any_of(" "), boost::token_compress_on);
	for (size_t i = 0; i<SplitVec_CONN.size(); i++)
	{
		std::cout << SplitVec_CONN[i] << std::endl;
	}
	string str2(chUrl);
	vector<string> SplitVec_URL;
	boost::split(SplitVec_URL, str2, boost::is_any_of(" "), boost::token_compress_on);
	for (size_t i = 0; i<SplitVec_URL.size(); i++)
	{
		std::cout << SplitVec_URL[i] << std::endl;
	}
	if (SplitVec_URL.size() != m_CameraNumbers&&SplitVec_CONN.size() != m_CameraNumbers)
	{
		cout << "当前推流路数位:" << m_CameraNumbers << "，与配置输入输出地址不一致！" << endl;
		ret = -4;
		getchar();
		return ret;
	}
	vector<NVRMediaObject*> v_pNVRMediaObject;
	for (size_t i = 0; i<SplitVec_CONN.size(); i++)
	{
		NVRMediaObject* obj = new NVRMediaObject;
		AVDictionary* options = nullptr;
		av_dict_set(&options, "rtsp_transport", "tcp", 0);
		if (obj->OpenInputNVR(SplitVec_CONN[i].c_str(), options))
		{
			std::cout << "打开输入流:" << SplitVec_CONN[i] << std::endl;
		}
		if (obj->OpenOutputFromNVR("flv", SplitVec_URL[i].c_str()))
		//if (obj->OpenOutputFromNVR("rtsp", SplitVec_URL[i].c_str()))
		{
			std::cout << "打开输出流:" << SplitVec_URL[i] << std::endl;
		}
		v_pNVRMediaObject.push_back(obj);
	}

	for (size_t i = 0; i < m_CameraNumbers; i++)
	{
		std::cout << "开始传输视频" << i+1 << std::endl;
		v_pNVRMediaObject[i]->start();
	}
	getchar();
	return ret;

}

//int main(int argv, char* argc[])
//{
//	//MediaManeger::Instance()->print_ok();
//	std::vector<TDeviceName> vector_VideoDevices;
//	std::vector<TDeviceName> vector_AudioDevices;
//	const GUID guid_VideoValue = CLSID_VideoInputDeviceCategory;// CLSID_VideoInputDeviceCategory;//CLSID_AudioInputDeviceCategory
//	const GUID guid_AudioValue = CLSID_AudioInputDeviceCategory;
//	DS_GetAudioVideoInputDevices(vector_VideoDevices, guid_VideoValue);
//	DS_GetAudioVideoInputDevices(vector_AudioDevices, guid_AudioValue);
//	
//	char *ch_VideoInputDevice = NULL;
//	char *ch_AudioInputDevice = NULL;
//	//std::wstring s_audioname;
//	if (vector_VideoDevices.size() > 0)
//	{
//		std::vector<TDeviceName>::iterator it = vector_VideoDevices.begin();
//		//vector_VideoDevices[1].FriendlyName
//		ch_VideoInputDevice = dup_wchar_to_char(it->FriendlyName);//it->FriendlyName
//		cout << "find input video source:" << ch_VideoInputDevice << endl;
//	}else
//		cout << "Cannot find input video source!\n" << endl;
//	if (vector_AudioDevices.size() > 0)
//	{
//		std::vector<TDeviceName>::iterator it = vector_AudioDevices.begin();
//		//wcstombs_s()
//		//s_audioname.append(it->FriendlyName);// = CW2A(it->FriendlyName, CP_UTF8);
//		ch_AudioInputDevice = dup_wchar_to_char(it->FriendlyName);
//		cout << "find input audio source:" << ch_AudioInputDevice <<"  LEN:"<< strlen(ch_AudioInputDevice)<< endl;
//	}
//	else {
//		cout << "Cannot find input audio source!\n" << endl;
//	}
//	// Set device params
//	AVDictionary *device_param = 0;
//	//if not setting rtbufsize, error messages will be shown in cmd, but you can still watch or record the stream correctly in most time
//	//setting rtbufsize will erase those error messages, however, larger rtbufsize will bring latency
//	//av_dict_set(&device_param, "rtbufsize", "5M", 0);
//	//av_dict_set(&device_param, "rtsp_transport", "tcp", 0);
//	//av_dict_set(&device_param, "framerate", "30", NULL);
//	char device_name[80];
//	char device_name_a[80];
//	//char device_name_aa[80];
//	sprintf_s(device_name, "video=%s", ch_VideoInputDevice);
//	sprintf_s(device_name_a, "audio=%s", ch_AudioInputDevice);
//	std::string s_audioname(device_name_a);
//	bool b_Video = false;
//	bool b_Audio = false;
//	bool b_NVR = false;
//	//AVDictionary* options = nullptr;
//	//av_dict_set(&options, "rtsp_transport", "tcp", 0);
//	//if (b_NVR = MediaManeger::Instance()->OpenInputNVR("rtsp://admin:Changeme@192.168.0.252:554/ch1/main/av_stream", options))
//	//{
//	//	cout << "Open input NVR source!\n" << endl;
//	//}
//	if (b_Video=MediaManeger::Instance()->OpenVideoCapture(device_name, device_param))
//	{
//		cout << "Open input video source!\n" << endl;
//	}
//	//AVDictionary *device_param1 = 0;
//	//av_dict_set(&device_param1, "rtbufsize", "100M", 0);
//	//if(b_Audio = MediaManeger::Instance()->OpenAudioCapture(s_audioname.c_str(), device_param1))
//	//	cout << "Open input audio source!\n" << endl;
//
//	//if (!MediaManeger::Instance()->OpenOutputFlv("rtmp://120.77.203.179:1935/openstream/test", b_Video, b_Audio))
//	if (!MediaManeger::Instance()->OpenOutputFlv("rtmp://47.97.169.87:1935/openstream/test", b_Video, b_Audio))
//	{
//		cout << "Open Output audio source error!!!" << endl;
//		getchar();
//		return -1;
//	}
//	//MediaManeger::Instance()->OpenOutputRtsp("rtsp://39.108.16.11:554/live.sdp", b_Video, b_Audio);
//	//MediaManeger::Instance()->OpenOutputFromNVR("flv","rtmp://39.108.16.11:1935/openstream/test");
//	if (b_NVR)
//	{
//		NVRDecodeDeal nvrDeal;
//		nvrDeal.start();
//	}
//	if (b_Video)
//	{
//		VideoDecodeDeal videoDeal;
//		videoDeal.start();
//	}
//	if (b_Audio)
//	{
//		AudioDecodeDeal audioDeal;
//		audioDeal.start();
//	}
//	MuxingEncodeDeal muxingDeal;
//	muxingDeal.bVideo = b_Video;
//	muxingDeal.bAudio = b_Audio;
//	muxingDeal.start();
//	getchar();
//    return 0;
//}

