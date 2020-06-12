//main.cpp -- 主函数入口(备注:功能性函数为audioProcess)

#include "audioqualitydetect.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

int main(int argc,char** argv)
{
	if (argc < 2)
	{
		printf("usage: inputVideo cycle(s)\n");
		exit(1);
	}
	const char* inputFile = argv[1];
	float fps, period;
	stringstream ss;
	ss << argv[2]; ss >> period; ss.clear();

	AVFormatContext *pFormatCtx;
	int audioStreamIndex = -1;
	int videoStreamIndex = -1;
	unsigned int i = 0;

	avdevice_register_all();	//注册组件
	pFormatCtx = avformat_alloc_context();	//封装格式上下文
	//打开输入文件
	if (avformat_open_input(&pFormatCtx, inputFile, NULL, NULL) != 0)
	{
		printf("can't open input file!\n");
		return -1;
	}
	//获取音频信息
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("can't find stream info!\n");
		return -1;
	}

	//找到音频流的索引位置
	for (i = 0; i < pFormatCtx->nb_streams; ++i)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
			break;
		}
	}
	
	if (audioStreamIndex == -1)
	{
		printf("can't find a audio stream!\n");
	}
	else
	{
		audioProcess(pFormatCtx, audioStreamIndex, period);
	}
	avformat_close_input(&pFormatCtx);
	return 0;
}