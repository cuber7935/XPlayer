#ifndef _XFFEMPEG_H_
#define _XFFEMPEG_H_

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>   //重采样
}

#include <string>
#include <QMutex>

class XFFempeg
{
public:
	/*单例模式*/
	static XFFempeg* getInstance()
	{
		static XFFempeg ff;
		return &ff;
	}
	/*
	**@打开视频路径  打开之前会先把上次打开的关闭
	**@para   path  视频文件路径
	**@return bool  失败通过GetError()获取错误信息
	*/
	bool Open(const char* path);
	
	/*
	**@读取视频帧
	**@return AVPacket结构体   返回值需要用户清理
	*/
	AVPacket Read();

	/*
	**@获取错误信息
	*/
	std::string GetError();
	
	/*
	**@解码
	**@para   pkt  视频帧结构体
	**@return AVFrame   返回解码后的 yuv
	*/
	int Decode(const AVPacket* pkt);

	bool ToRGB(char* out, int outWidth, int outHeight);

	/*重采样*/
	int ToPCM(char* out);

	/*
	**@用户拖动进度条，实现随机读取视频
	**@para   pos  相对位置， 百分比
	*/
	bool Seek(float pos);

	/*释放资源*/
	void Close();

	/*获取总时长*/
	int GetTotalMs()const;
	/*获取视频流标识*/
	int GetStreamFlag()const;
	/*获取音频流标识*/
	int GetAudioFlag()const;
	/*获取当前播放进度*/
	int GetPtsMs()const;
	/*获取播放标识*/
	bool GetPlayFlag()const;
	/*设置播放标识*/
	void SetPlayFlag(bool play);

	/*获取AVPacket的帧率*/
	int GetPacketFps(const AVPacket* pkt);

	/*获取音频样式*/
	int GetSampleRate()const;
	/*获取音频样式大小*/
	int GetSampleSize()const;
	/*获取声道*/
	int GetChannelCount()const;

	virtual ~XFFempeg();
private:
	XFFempeg();
	
	AVFormatContext* _ic = NULL;			 //存放打开的文件的相关信息
	AVFrame*		 _yuv = NULL;			 //存放解码后的视频
	AVFrame*         _pcm = NULL;            //存放解码后的音频
	SwsContext*		 _cCtx = NULL;           //转换器
	SwrContext*      _aCtx = NULL;           //音频重采样预设
	char			 _errbuf[1024];			 //保存错误信息
	QMutex			 _mutex;				 //互斥变量
	int				 _totalMs = 0;			 //总时长	
	int				 _videoStream = 0;		 //视频流，
	int              _audioStream = 0;       //音频流， //还是字幕流
	int              _fps = 0;               //帧率
	int              _pts = 0;               //当前播放进度
	bool             _play = false;          //标识是否播放
	int				 _sampleRate = 48000;    //一秒钟采集多少声音	
	int				 _sampleSize = 16;       //样本大小  16位， 两个字节
	int				 _channelCount = 2;      //声道设置, 2代表双声道
	//int              _pktFps = 0;            //AVPacket的帧率
};

#endif