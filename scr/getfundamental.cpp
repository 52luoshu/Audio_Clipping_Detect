//getfundamental.cpp����������Ƶ��Ƶ�������ж����ں�������

#include "audioqualitydetect.h"

int getACF(vector<int16_t>wavFrame, int sampleRate)
{
	vector<float> acf;
	acf.empty();

	//ACF����
	for (int k = 0; k < wavFrame.size(); k++)
	{
		float sum = 0.0;
		for (int i = 0; i < wavFrame.size() - k; i++)
		{
			sum = sum + wavFrame[i] * wavFrame[i + k];
		}
		acf.push_back(sum);
	}

	//�����ֵ����ض���ߣ�Ϊ��Ƶ
	float max(-32768);
	int index = 0;
	for (int k = 0; k < wavFrame.size(); k++)
	{
		if (k > 25 && acf[k] > max)
		{
			max = acf[k];
			index = k;
		}
	}
	return (int)sampleRate / index;
}