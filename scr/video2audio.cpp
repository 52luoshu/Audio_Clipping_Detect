//video2audio.cpp -- 功能函数，从视频中解码出音频,并计算音频信号短时能量，保存在日志文件中
#include "audioqualitydetect.h"

void video2audio(AVFormatContext *pFormatCtx, int streamIndex, float audiofps, float period)
{
	/****************** 数据与日志初始化 *****************/
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

	/****************** 获取音频解码器 *****************/
	//根据索引拿到相应的流
	pCodec = avcodec_find_decoder(codecpar->codec_id);
	if (!pCodec)
	{
		printf("can't decoder audio!\n");
		exit(1);
	}

	//申请一个解码上下文
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		printf("can't allocate a audio decoding context!\n");
		exit(1);
	}

	//用流解码信息初始化编码参数
	avcodec_parameters_to_context(pCodecCtx, codecpar);
	pCodecCtx->pkt_timebase = pFormatCtx->streams[streamIndex]->time_base;

	/****************** 打开解码器 *****************/
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("can't open codec!\n");
		exit(1);
	}
	//编码数据
	AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	//解压缩数据
	AVFrame *frame = av_frame_alloc();
	//frame -> 16bit PCM 统一音频采样格式
	SwrContext *swrCtx = swr_alloc();

	//---------------- 开始重采样设置选项 ----------------
	AVSampleFormat inSampleFmt = pCodecCtx->sample_fmt;	//输入的采样格式
	AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;	//输出的采样格式
	int inSampleRate = pCodecCtx->sample_rate;	//输入的采样率
	int outSampleRate = inSampleRate;	//输出的采样率
	uint16_t inChannelLayout = pCodecCtx->channel_layout;	//输入的声道布局
	uint16_t outChannelLayout = AV_CH_LAYOUT_MONO;	//输出的声道布局
	swr_alloc_set_opts(swrCtx, outChannelLayout, outSampleFmt, outSampleRate, inChannelLayout, inSampleFmt, inSampleRate, 0, NULL);
	swr_init(swrCtx);
	//---------------- 完成重采样设置选项 ----------------
	sprintf(info_log, "[info] Format: %d SampleRate: %d Channels: %d\n", inSampleFmt, inSampleRate, inChannelLayout);
	writeLog(log, info_log, 0);
	int outChannelNb = av_get_channel_layout_nb_channels(outChannelLayout);	//获取输出声道个数
	uint8_t *outBuffer = (uint8_t*)av_malloc(2 * outSampleRate);	//存储PCM数据
	//回到流的初始位置
	av_seek_frame(pFormatCtx, streamIndex, 0, AVSEEK_FLAG_BACKWARD);
	/****************** 按帧读取压缩的音频数据AVPacket *****************/
	int outBufferSize = 0;
	uint64_t frmcnt = 0;
	int16_t value_instant = 0;
	uint32_t power_instant = 0;
	uint64_t power_period = 0;
	while (av_read_frame(pFormatCtx, packet) >= 0)
	{
		if (packet->stream_index == streamIndex)
		{
			//解码AVPacket --> AVFrame
			int ret = avcodec_send_packet(pCodecCtx, packet);
			if (ret < 0)
			{
				printf("decode error!\n");
				break;
			}
			if (avcodec_receive_frame(pCodecCtx, frame) >= 0)
			{
				swr_convert(swrCtx, &outBuffer, 4 * 44100, (const uint8_t**)frame->data, frame->nb_samples);
				//获取sample的size
				outBufferSize = av_samples_get_buffer_size(NULL, outChannelNb, frame->nb_samples, outSampleFmt, 1);
				//计算短时能量
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