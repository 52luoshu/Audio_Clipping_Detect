//audioprcess.cpp -- ���ܺ���������Ƶ�н������Ƶ,��������Ƶ�źŶ�ʱ��������������־�ļ���
#include "audioqualitydetect.h"

void audioProcess(AVFormatContext *pFormatCtx, int streamIndex, float period)
{
	/****************** SWITCH INIT *****************/
	bool bypass_energy = 0;
	bool bypass_acf = 1;
	int cycle_acf = 1;			//��Ƶ�������֡��

	/****************** ��������־��ʼ�� *****************/
	uint64_t frmcnt = 0;		//֡��ͳ��
	int16_t value_instant = 0;	//˲ʱ����
	uint64_t power_period = 0;	//����������ź�����
	vector<int16_t>wavFrame;	//��֡��sample��
	queue<int16_t>valuelist;	//��֡�洢
	int acf = 0;				//��Ƶ

	/****************** ��������־��ʼ�� *****************/
	float cycle = period;
	char logtime[20] = { 0 };
	getTime(logtime);
	char logfile[80];
	sprintf(logfile, "%s%s%s", (char*)"../log/AQ_", logtime, (char*)".log");
	FILE* log = fopen(logfile, "a+");
	char info_log[80];

	AVCodec *pCodec;
	AVCodecContext *pCodecCtx;
	AVCodecParameters *codecpar = pFormatCtx->streams[streamIndex]->codecpar;

	/****************** ��ȡ��Ƶ������ *****************/
	//���������õ���Ӧ����
	pCodec = avcodec_find_decoder(codecpar->codec_id);
	if (!pCodec)
	{
		printf("can't decoder audio!\n");
		exit(1);
	}

	//����һ������������
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		printf("can't allocate a audio decoding context!\n");
		exit(1);
	}

	//����������Ϣ��ʼ���������
	avcodec_parameters_to_context(pCodecCtx, codecpar);
	pCodecCtx->pkt_timebase = pFormatCtx->streams[streamIndex]->time_base;

	/****************** �򿪽����� *****************/
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("can't open codec!\n");
		exit(1);
	}
	
	AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));	//��������
	AVFrame *frame = av_frame_alloc();	//��ѹ������
	SwrContext *swrCtx = swr_alloc();	//frame -> 16bit PCM ͳһ��Ƶ������ʽ

	//---------------- ��ʼ�ز�������ѡ�� ----------------
	AVSampleFormat inSampleFmt = pCodecCtx->sample_fmt;	//����Ĳ�����ʽ
	AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;	//����Ĳ�����ʽ
	int inSampleRate = pCodecCtx->sample_rate;	//����Ĳ�����
	int outSampleRate = inSampleRate;	//����Ĳ�����
	float fps = outSampleRate / 1024;
	uint16_t inChannelLayout = pCodecCtx->channel_layout;	//�������������
	uint16_t outChannelLayout = AV_CH_LAYOUT_MONO;	//�������������
	swr_alloc_set_opts(swrCtx, outChannelLayout, outSampleFmt, outSampleRate, inChannelLayout, inSampleFmt, inSampleRate, 0, NULL);
	swr_init(swrCtx);
	//---------------- ����ز�������ѡ�� ----------------
	sprintf(info_log, "[info] Format: %d SampleRate: %d Channels: %llu\n", inSampleFmt, inSampleRate, inChannelLayout);
	writeLog(log, info_log, 0);
	int fullcycle = (int)(fps*cycle);
	int halfcycle = (int)(fps*cycle / 2);

	int outChannelNb = av_get_channel_layout_nb_channels(outChannelLayout);	//��ȡ�����������
	uint8_t *outBuffer = (uint8_t*)av_malloc(2 * outSampleRate);	//�洢PCM����
	av_seek_frame(pFormatCtx, streamIndex, 0, AVSEEK_FLAG_BACKWARD);	//�ص����ĳ�ʼλ��
	/****************** ��֡��ȡѹ������Ƶ����AVPacket *****************/

	for (int i = 0; i < fullcycle; i++)
	{
		valuelist.push(0);
	}
	while (av_read_frame(pFormatCtx, packet) >= 0)
	{
		if (packet->stream_index == streamIndex)
		{
			//����AVPacket --> AVFrame
			int ret = avcodec_send_packet(pCodecCtx, packet);
			if (ret < 0)
			{
				printf("decode error!\n");
				break;
			}
			if (avcodec_receive_frame(pCodecCtx, frame) >= 0)
			{
				swr_convert(swrCtx, &outBuffer, 4 * outSampleRate, (const uint8_t**)frame->data, frame->nb_samples);
				value_instant = *(short *)outBuffer;


				if (bypass_acf && frmcnt%cycle_acf == 0)
				{
					//��Ƶ��Ƶ���
					wavFrame.clear();
					for (int i = 0; i < frame->nb_samples; i++)
					{
						wavFrame.push_back(((uint8_t)*(frame->data[0] + i)) * 257 - 32768);
					}
					acf = getACF(wavFrame, outSampleRate);
					sprintf(info_log, "[frm%llu] ACF: %d", frmcnt, acf);
					writeLog(log, info_log, 0);
				}
				
				if (bypass_energy)
				{
					//�Ծ��λ�������ʽ����������������
					valuelist.pop();
					valuelist.push(value_instant);
					if (frmcnt%halfcycle == 0)
					{
						power_period = getEnergy(valuelist);
						sprintf(info_log, "[frm%llu] power: %llu", frmcnt, power_period);
						writeLog(log, info_log, 0);
					}
				}

				frmcnt += 1;
			}
		}
		av_packet_unref(packet);
	}
	closeLog(log);
	av_frame_free(&frame);
	av_free(outBuffer);
	swr_free(&swrCtx);
	avcodec_close(pCodecCtx);
	return;
}