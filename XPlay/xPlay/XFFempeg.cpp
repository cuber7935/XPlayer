#include "XFFempeg.h"

#pragma comment(lib, "avutil.lib")   //工具库
#pragma comment(lib, "avformat.lib") //文件格式库
#pragma comment(lib, "avcodec.lib")  //解码库
#pragma comment(lib, "swscale.lib")  //转码库
#pragma comment(lib, "swresample.lib") //重采样

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

XFFempeg::XFFempeg()
{
	memset(_errbuf, 0, sizeof(_errbuf));

	//注册所有格式
	av_register_all();
}

//获取总时长
int XFFempeg::GetTotalMs() const
{
	return _totalMs;
}

/*获取标识视频流还是音频流*/
int XFFempeg::GetStreamFlag() const
{
	return _videoStream;
}
/*获取音频流标识*/
int XFFempeg::GetAudioFlag() const
{
	return _audioStream;
}

/*获取当前播放进度*/
int XFFempeg::GetPtsMs() const
{
	return _pts;
}
/*获取播放标识*/
bool XFFempeg::GetPlayFlag() const
{
	return _play;
}
/*设置播放标识*/
void XFFempeg::SetPlayFlag(bool play)
{
	_play = play;
}
/*获取AVPacket的帧率*/
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
/*获取音频样式*/
int XFFempeg::GetSampleRate() const
{
	return _sampleRate;
}
/*获取音频样式大小*/
int XFFempeg::GetSampleSize() const
{
	return _sampleSize;
}
/*获取声道*/
int XFFempeg::GetChannelCount() const
{
	return _channelCount;
}

XFFempeg::~XFFempeg()
{
}

//打开视频路径
bool XFFempeg::Open(const char* path)
{
	if (!path)
		return false;
	//打开之前先把上次打开的关闭
	Close();
	_mutex.lock();
	//打开文件
	int ret = avformat_open_input(&_ic, path, NULL, NULL);
	if (ret != 0)
	{
		_mutex.unlock();
		av_strerror(ret, _errbuf, sizeof(_errbuf));
		//printf("Open %s failed:  %s\n", path, _errbuf);
		return false;
	}
	//获取文件的总时长， 以毫秒为单位
	_totalMs = (_ic->duration / AV_TIME_BASE )* 1000;  

	/*打开解码器*/
	//遍历视频流
	for (int i = 0; i < _ic->nb_streams; i++)
	{
		//解码器的值
		AVCodecContext* enc = _ic->streams[i]->codec;
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)  //视频
		{
			_videoStream = i;  //记录流的编号
			//获取帧率
			_fps = r2d(_ic->streams[i]->avg_frame_rate);
			//查找解码器
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
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)   //音频
		{
			_audioStream = i;
			//找到音频的解码器
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
			//获取相关属性
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

//读取视频帧
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

//获取错误信息
std::string XFFempeg::GetError()
{
	_mutex.lock();
	std::string re = this->_errbuf;
	_mutex.unlock();
	return re;
}

//解码
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
		//创建yuv视频帧  (yuv 亮度跟色彩)
		_yuv = av_frame_alloc();
	}
	if (_pcm == NULL)
	{
		_pcm = av_frame_alloc();
	}
	//开始解码
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
	//得到实际的秒数 * 1000  为毫秒数
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
	//获取转码器的内容
	_cCtx = sws_getCachedContext(_cCtx,				//转码器对象
				videoCtx->width, videoCtx->height,  //源的宽高
				videoCtx->pix_fmt,					//源像素点的格式
				outWidth, outHeight,                //目标的宽高
				AV_PIX_FMT_BGRA,                    //目标的像素点格式
				SWS_BICUBIC,                        //用哪个算法
				NULL, NULL, NULL                    //过滤器
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

	//转码
	sws_scale(_cCtx, _yuv->data, _yuv->linesize, 0, videoCtx->height,   //源数据属性
		data, lineSize);   //目标数据属性

	_mutex.unlock();
	return true;;
}
/*重采样*/
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
		//采样的大小 固定成16位，可能会导致open时出现问题
		swr_alloc_set_opts(_aCtx, ctx->channel_layout, AV_SAMPLE_FMT_S16,
			ctx->sample_rate, ctx->channels,
			ctx->sample_fmt, ctx->sample_rate, 0, NULL);
		//初始化
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

//用户拖动进度条，实现随机读取视频
bool XFFempeg::Seek(float pos)
{
	_mutex.lock();
	if (!_ic)
	{
		_mutex.unlock();
		return false;
	}
	//存放时间磋
	int64_t stamp = 0;  
	//得到实际的位置
	stamp = _ic->streams[_videoStream]->duration * pos;
	int re = av_seek_frame(_ic, _videoStream, stamp, AVSEEK_FLAG_BACKWARD| AVSEEK_FLAG_FRAME);
	//刷新一下，否则无法继续解码
	avcodec_flush_buffers(_ic->streams[_videoStream]->codec);
	_mutex.unlock();
	if (re < 0)
	{
		return false;
	}
	return true;
}
//释放资源
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