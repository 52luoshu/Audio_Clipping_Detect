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

void audioProcess(AVFormatContext *pFormatCtx, int streamIndex,  float period);	//音频解码与功能集成
uint64_t getEnergy(queue<int16_t>valuelist);	//短时能量
queue<float> hammingWin(int length);	//海明窗
queue<int>rectangularWin(int length);	//矩形窗
int getACF(vector<int16_t>wavFrame, int sampleRate);	//基频估计

int getLogTime(char* out, int fmt);
int getTime(char* out);
FILE* openFile(const char* fileName, const char* mode);
int writeLog(FILE* fp, const char* str, bool bLog);
int closeLog(FILE* fp);