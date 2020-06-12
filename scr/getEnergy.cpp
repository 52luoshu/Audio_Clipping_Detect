//getEnergy.cpp -- ���ڶ�ʱ���������ź�������ȡ

#include "audioqualitydetect.h"

uint64_t getEnergy(queue<int16_t>valuelist)
{
	queue<int16_t>value = valuelist;
	uint64_t power_period = 0;

	queue<int>window = rectangularWin(value.size());	//���δ�
	for (int j = 0; j < value.size(); j++)
	{
		power_period += pow((value.front()*window.front()), 2);
		window.pop();
		value.pop();

	}
	return power_period;
}
