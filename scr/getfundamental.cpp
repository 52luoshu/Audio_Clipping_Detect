//getfundamental.cpp——估计音频基频，用于判断属于何种声音

#include "audioqualitydetect.h"

int getACF(vector<int16_t>wavFrame, int sampleRate)
{
	vector<float> acf;
	acf.empty();

	//ACF计算
	for (int k = 0; k < wavFrame.size(); k++)
	{
		float sum = 0.0;
		for (int i = 0; i < wavFrame.size() - k; i++)
		{
			sum = sum + wavFrame[i] * wavFrame[i + k];
		}
		acf.push_back(sum);
	}

	//以最大值（相关度最高）为基频
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