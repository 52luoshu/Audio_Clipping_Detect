//window.cpp -- 音频信号处理用到的一些窗函数

#include "audioqualitydetect.h"

queue<float> hammingWin(int length)	//海明窗
{
	queue<float>hammingwin;
	for (int n = 0; n < length; n++)
	{
		hammingwin.push(0.54 - 0.46*cos(2 * PI*(float)n / (length - 1)));
	}
	return hammingwin;
}

queue<int>rectangularWin(int length)	//矩形窗
{
	queue<int>rectangularwin;
	for (int m = 0; m < length; m++)
	{
		rectangularwin.push(1);
	}
	return rectangularwin;
}