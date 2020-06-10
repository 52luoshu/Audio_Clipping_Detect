//video2audio.cpp -- ���ܺ���������Ƶ�н������Ƶ,��������Ƶ�źŶ�ʱ��������������־�ļ���
#include "audioqualitydetect.h"

void video2audio(AVFormatContext *pFormatCtx, int streamIndex, float audiofps, float period)
{
	/****************** ��������־��ʼ�� *****************/
	float fps = audiofps, cycle = period;
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
	//��������
	AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	//��ѹ������
	AVFrame *frame = av_frame_alloc();
	//frame -> 16bit PCM ͳһ��Ƶ������ʽ
	SwrContext *swrCtx = swr_alloc();

	//---------------- ��ʼ�ز�������ѡ�� ----------------
	AVSampleFormat inSampleFmt = pCodecCtx->sample_fmt;	//����Ĳ�����ʽ
	AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;	//����Ĳ�����ʽ
	int inSampleRate = pCodecCtx->sample_rate;	//����Ĳ�����
	int outSampleRate = inSampleRate;	//����Ĳ�����
	uint16_t inChannelLayout = pCodecCtx->channel_layout;	//�������������
	uint16_t outChannelLayout = AV_CH_LAYOUT_MONO;	//�������������
	swr_alloc_set_opts(swrCtx, outChannelLayout, outSampleFmt, outSampleRate, inChannelLayout, inSampleFmt, inSampleRate, 0, NULL);
	swr_init(swrCtx);
	//---------------- ����ز�������ѡ�� ----------------
	sprintf(info_log, "[info] Format: %d SampleRate: %d Channels: %d\n", inSampleFmt, inSampleRate, inChannelLayout);
	writeLog(log, info_log, 0);
	int outChannelNb = av_get_channel_layout_nb_channels(outChannelLayout);	//��ȡ�����������
	uint8_t *outBuffer = (uint8_t*)av_malloc(2 * outSampleRate);	//�洢PCM����
	//�ص����ĳ�ʼλ��
	av_seek_frame(pFormatCtx, streamIndex, 0, AVSEEK_FLAG_BACKWARD);
	/****************** ��֡��ȡѹ������Ƶ����AVPacket *****************/
	int outBufferSize = 0;
	uint64_t frmcnt = 0;
	int16_t value_instant = 0;
	uint32_t power_instant = 0;
	uint64_t power_period = 0;
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
				swr_convert(swrCtx, &outBuffer, 4 * 44100, (const uint8_t**)frame->data, frame->nb_samples);
				//��ȡsample��size
				outBufferSize = av_samples_get_buffer_size(NULL, outChannelNb, frame->nb_samples, outSampleFmt, 1);
				//�����ʱ����
				value_instant = *(short *)outBuffer;
				power_instant = pow(value_instant, 2);

				if ((frmcnt % (int)(fps*cycle)) != 0)
				{
					power_period += power_instant;
				}
				if ((frmcnt % (int)(fps*cycle)) == 0)
				{
					power_period += power_instant;
					sprintf(info_log, "frm%d: %lld", frmcnt, power_period);
					writeLog(log, info_log, 0);
					power_instant = 0;
					power_period = 0;
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