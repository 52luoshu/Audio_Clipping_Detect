//window.cpp -- ��Ƶ�źŴ����õ���һЩ������

#include "audioqualitydetect.h"

queue<float> hammingWin(int length)	//������
{
	queue<float>hammingwin;
	for (int n = 0; n < length; n++)
	{
		hammingwin.push(0.54 - 0.46*cos(2 * PI*(float)n / (length - 1)));
	}
	return hammingwin;
}

queue<int>rectangularWin(int length)	//���δ�
{
	queue<int>rectangularwin;
	for (int m = 0; m < length; m++)
	{
		rectangularwin.push(1);
	}
	return rectangularwin;
}