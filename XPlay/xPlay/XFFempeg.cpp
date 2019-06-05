#include "XFFempeg.h"

#pragma comment(lib, "avutil.lib")   //���߿�
#pragma comment(lib, "avformat.lib") //�ļ���ʽ��
#pragma comment(lib, "avcodec.lib")  //�����
#pragma comment(lib, "swscale.lib")  //ת���
#pragma comment(lib, "swresample.lib") //�ز���

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

XFFempeg::XFFempeg()
{
	memset(_errbuf, 0, sizeof(_errbuf));

	//ע�����и�ʽ
	av_register_all();
}

//��ȡ��ʱ��
int XFFempeg::GetTotalMs() const
{
	return _totalMs;
}

/*��ȡ��ʶ��Ƶ��������Ƶ��*/
int XFFempeg::GetStreamFlag() const
{
	return _videoStream;
}
/*��ȡ��Ƶ����ʶ*/
int XFFempeg::GetAudioFlag() const
{
	return _audioStream;
}

/*��ȡ��ǰ���Ž���*/
int XFFempeg::GetPtsMs() const
{
	return _pts;
}
/*��ȡ���ű�ʶ*/
bool XFFempeg::GetPlayFlag() const
{
	return _play;
}
/*���ò��ű�ʶ*/
void XFFempeg::SetPlayFlag(bool play)
{
	_play = play;
}
/*��ȡAVPacket��֡��*/
int XFFempeg::GetPacketFps(const AVPacket* pkt)
{
	_mutex.lock();
	if (!_ic)
	{
		_mutex.unlock();
		return -1;
	}
	int fps = r2d(_ic->streams[pkt->stream_index]->time_base)*(pkt->pts) * 1000;
	_mutex.unlock();
	return fps;
}
/*��ȡ��Ƶ��ʽ*/
int XFFempeg::GetSampleRate() const
{
	return _sampleRate;
}
/*��ȡ��Ƶ��ʽ��С*/
int XFFempeg::GetSampleSize() const
{
	return _sampleSize;
}
/*��ȡ����*/
int XFFempeg::GetChannelCount() const
{
	return _channelCount;
}

XFFempeg::~XFFempeg()
{
}

//����Ƶ·��
bool XFFempeg::Open(const char* path)
{
	if (!path)
		return false;
	//��֮ǰ�Ȱ��ϴδ򿪵Ĺر�
	Close();
	_mutex.lock();
	//���ļ�
	int ret = avformat_open_input(&_ic, path, NULL, NULL);
	if (ret != 0)
	{
		_mutex.unlock();
		av_strerror(ret, _errbuf, sizeof(_errbuf));
		//printf("Open %s failed:  %s\n", path, _errbuf);
		return false;
	}
	//��ȡ�ļ�����ʱ���� �Ժ���Ϊ��λ
	_totalMs = (_ic->duration / AV_TIME_BASE )* 1000;  

	/*�򿪽�����*/
	//������Ƶ��
	for (int i = 0; i < _ic->nb_streams; i++)
	{
		//��������ֵ
		AVCodecContext* enc = _ic->streams[i]->codec;
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)  //��Ƶ
		{
			_videoStream = i;  //��¼���ı��
			//��ȡ֡��
			_fps = r2d(_ic->streams[i]->avg_frame_rate);
			//���ҽ�����
			AVCodec* codec = avcodec_find_decoder(enc->codec_id);
			if (!codec)
			{
				_mutex.unlock();
				//printf("video codec not found!\n");
				return false;
			}
			int err = avcodec_open2(enc, codec, NULL);
			if (err != 0)
			{
				_mutex.unlock();
				av_strerror(err, _errbuf, sizeof(_errbuf));
				return false;
			}
		}
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)   //��Ƶ
		{
			_audioStream = i;
			//�ҵ���Ƶ�Ľ�����
			AVCodec* codec = avcodec_find_decoder(enc->codec_id);
			if (!codec)
			{
				_mutex.unlock();
				return false;
			}
			int re = avcodec_open2(enc, codec, NULL);
			if (re < 0)
			{
				_mutex.unlock();
				av_strerror(re, _errbuf, sizeof(_errbuf));
				return false;
			}
			//��ȡ�������
			this->_sampleRate = enc->sample_rate;
			this->_channelCount = enc->channels;
			switch (enc->sample_fmt)
			{
			case AV_SAMPLE_FMT_S16:
				this->_sampleSize = 16;
				break;
			case AV_SAMPLE_FMT_S32:
				this->_sampleSize = 32;
				break;
			default:
				break;
			}
			//printf("sampleRate: %d, sampleSize: %d, channelCount: %d\n", _sampleRate, _sampleSize, _channelCount);
		}
	}
	_mutex.unlock();
	return true;
}

//��ȡ��Ƶ֡
AVPacket XFFempeg::Read()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(pkt));
	_mutex.lock();

	if (!_ic)
	{
		_mutex.unlock();
		return pkt;
	}

	int err = av_read_frame(_ic, &pkt);
	if (err != 0)
	{
		_mutex.unlock();
		av_strerror(err, _errbuf, sizeof(_errbuf));
		return pkt;
	}
	_mutex.unlock();
	return pkt;
}

//��ȡ������Ϣ
std::string XFFempeg::GetError()
{
	_mutex.lock();
	std::string re = this->_errbuf;
	_mutex.unlock();
	return re;
}

//����
int XFFempeg::Decode(const AVPacket* pkt)
{
	_mutex.lock();

	if (!_ic || !pkt)
	{
		_mutex.unlock();
		return NULL;
	}
	if (_yuv == NULL)
	{
		//����yuv��Ƶ֡  (yuv ���ȸ�ɫ��)
		_yuv = av_frame_alloc();
	}
	if (_pcm == NULL)
	{
		_pcm = av_frame_alloc();
	}
	//��ʼ����
	int ret = avcodec_send_packet(_ic->streams[pkt->stream_index]->codec, pkt);
	if (ret != 0)
	{
		_mutex.unlock();
		av_strerror(ret, _errbuf, sizeof(_errbuf));
		return NULL;
	}

	AVFrame* frame = nullptr;
	if (pkt->stream_index == _audioStream)
	{
		frame = _pcm;
	}
	else if(pkt->stream_index == _videoStream)
	{
		frame = _yuv;
	}
	ret = avcodec_receive_frame(_ic->streams[pkt->stream_index]->codec, frame);
	if (ret != 0)
	{
		_mutex.unlock();
		av_strerror(ret, _errbuf, sizeof(_errbuf));
		return NULL;
	}
	//�õ�ʵ�ʵ����� * 1000  Ϊ������
	if (pkt->stream_index == _audioStream)
	{
		_pts = r2d(_ic->streams[pkt->stream_index]->time_base)*(frame->pts) * 1000;
	}
	_mutex.unlock();
	return _pts;
}
bool XFFempeg::ToRGB(char * out, int outWidth, int outHeight)
{
	_mutex.lock();
	if (!_ic )
	{
		_mutex.unlock();
		return false;
	}
	if (!_yuv)
	{
		_mutex.unlock();
		return false;
	}
	AVCodecContext* videoCtx = _ic->streams[this->_videoStream]->codec;
	//��ȡת����������
	_cCtx = sws_getCachedContext(_cCtx,				//ת��������
				videoCtx->width, videoCtx->height,  //Դ�Ŀ��
				videoCtx->pix_fmt,					//Դ���ص�ĸ�ʽ
				outWidth, outHeight,                //Ŀ��Ŀ��
				AV_PIX_FMT_BGRA,                    //Ŀ������ص��ʽ
				SWS_BICUBIC,                        //���ĸ��㷨
				NULL, NULL, NULL                    //������
			);
	if (!_cCtx)
	{
		_mutex.unlock();
		printf("sws_getCachedContext failed\n");
		return false;
	}

	uint8_t* data[AV_NUM_DATA_POINTERS] = {0};
	data[0] = (uint8_t*)out;
	int lineSize[AV_NUM_DATA_POINTERS] = {0};
	lineSize[0] = outWidth * 4;

	//ת��
	sws_scale(_cCtx, _yuv->data, _yuv->linesize, 0, videoCtx->height,   //Դ��������
		data, lineSize);   //Ŀ����������

	_mutex.unlock();
	return true;;
}
/*�ز���*/
int XFFempeg::ToPCM(char * out)
{
	_mutex.lock();
	if (!_ic || !_pcm || !out)
	{
		_mutex.unlock();
		return 0;
	}

	AVCodecContext* ctx = _ic->streams[_audioStream]->codec;
	if (!_aCtx)
	{
		_aCtx = swr_alloc();
		//�����Ĵ�С �̶���16λ�����ܻᵼ��openʱ��������
		swr_alloc_set_opts(_aCtx, ctx->channel_layout, AV_SAMPLE_FMT_S16,
			ctx->sample_rate, ctx->channels,
			ctx->sample_fmt, ctx->sample_rate, 0, NULL);
		//��ʼ��
		swr_init(_aCtx);
	}

	uint8_t* data[1] = { 0 };
	data[0] = (uint8_t*)out;
	
	int len = swr_convert(_aCtx, data, 10000, (const uint8_t**)_pcm->data, _pcm->nb_samples);
	if (len <= 0)
	{
		_mutex.unlock();
		av_strerror(len, _errbuf, sizeof(_errbuf));
		return 0;
	}
	int outSize = av_samples_get_buffer_size(NULL, ctx->channels, _pcm->nb_samples,
				AV_SAMPLE_FMT_S16, 0);
	_mutex.unlock();
	return outSize;
}

//�û��϶���������ʵ�������ȡ��Ƶ
bool XFFempeg::Seek(float pos)
{
	_mutex.lock();
	if (!_ic)
	{
		_mutex.unlock();
		return false;
	}
	//���ʱ���
	int64_t stamp = 0;  
	//�õ�ʵ�ʵ�λ��
	stamp = _ic->streams[_videoStream]->duration * pos;
	int re = av_seek_frame(_ic, _videoStream, stamp, AVSEEK_FLAG_BACKWARD| AVSEEK_FLAG_FRAME);
	//ˢ��һ�£������޷���������
	avcodec_flush_buffers(_ic->streams[_videoStream]->codec);
	_mutex.unlock();
	if (re < 0)
	{
		return false;
	}
	return true;
}
//�ͷ���Դ
void XFFempeg::Close()
{
	_mutex.lock();
	if (_ic)
		avformat_close_input(&_ic);

	if (_yuv)
		av_frame_free(&_yuv);

	if (_pcm)
		av_frame_free(&_pcm);

	if (_cCtx)
	{
		sws_freeContext(_cCtx);
		_cCtx = NULL;
	}

	if (_aCtx)
	{
		swr_free(&_aCtx);
	}
	_mutex.unlock();
}