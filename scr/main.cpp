#include "audioqualitydetect.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

int main(int argc,char** argv)
{
	if (argc < 3)
	{
		printf("usage: inputVideo fps cycle(s)\n");
		exit(1);
	}
	const char* inputFile = argv[1];
	float fps, period;
	stringstream ss;
	ss << argv[2]; ss >> fps; ss.clear();
	ss << argv[3]; ss >> period; ss.clear();

	AVFormatContext *pFormatCtx;
	int audioStreamIndex = -1;
	int videoStreamIndex = -1;
	unsigned int i = 0;


	//ע�����
	avdevice_register_all();
	//��װ��ʽ������
	pFormatCtx = avformat_alloc_context();
	//�������ļ�
	if (avformat_open_input(&pFormatCtx, inputFile, NULL, NULL) != 0)
	{
		printf("can't open input file!\n");
		return -1;
	}
	//��ȡ��Ƶ��Ϣ
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("can't find stream info!\n");
		return -1;
	}

	//�ҵ���Ƶ��������λ��
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
		video2audio(pFormatCtx, audioStreamIndex, fps,period);
	}
	avformat_close_input(&pFormatCtx);
	return 0;
}