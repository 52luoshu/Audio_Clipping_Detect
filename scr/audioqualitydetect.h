#include <stdio.h>
#include <cstdio>
#include <ctime>
#include <assert.h>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
}

void video2audio(AVFormatContext *pFormatCtx, int streamIndex,  float audiofps, float period);

int getLogTime(char* out, int fmt);
int getTime(char* out);
FILE* openFile(const char* fileName, const char* mode);
int writeLog(FILE* fp, const char* str, bool bLog);
int closeLog(FILE* fp);