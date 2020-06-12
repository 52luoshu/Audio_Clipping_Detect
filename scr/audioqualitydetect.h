#include <stdio.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <iostream>
#include <time.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
}
#define PI 3.1415926

using namespace std;

void audioProcess(AVFormatContext *pFormatCtx, int streamIndex,  float period);	//��Ƶ�����빦�ܼ���
uint64_t getEnergy(queue<int16_t>valuelist);	//��ʱ����
queue<float> hammingWin(int length);	//������
queue<int>rectangularWin(int length);	//���δ�
int getACF(vector<int16_t>wavFrame, int sampleRate);	//��Ƶ����

int getLogTime(char* out, int fmt);
int getTime(char* out);
FILE* openFile(const char* fileName, const char* mode);
int writeLog(FILE* fp, const char* str, bool bLog);
int closeLog(FILE* fp);