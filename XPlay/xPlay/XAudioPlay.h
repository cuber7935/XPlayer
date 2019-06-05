#ifndef _XAUDIO_PLAY_H_
#define _XAUDIO_PLAY_H_


class XAudioPlay
{
public:
	static XAudioPlay* getInstance();
	//开始
	virtual bool Start() = 0;
	//暂停
	virtual void Stop() = 0;
	//播放
	virtual void Play(bool isPlay)= 0;
	//写数据
	virtual bool Write(const char* data, int size) = 0;
	//获取剩余空间
	virtual int GetFree() = 0;
	
	//设置采集多少声音
	void SetSampleRate(int sampleRate);
	//设置样本大小
	void SetSampleSize(int sampleSize);
	//设置声道
	void SetChannelCount(int channelCount);

	/*获取音频样式*/
	int GetSampleRate()const;
	/*获取音频样式大小*/
	int GetSampleSize()const;
	/*获取声道*/
	int GetChannelCount()const;

	virtual ~XAudioPlay();
	

protected:
	XAudioPlay();
private:
	int _sampleRate = 48000;                   //一秒钟采集多少声音	
	int _sampleSize = 16;                      //样本大小  16位， 两个字节
	int _channelCount = 2;                     //声道设置, 2代表双声道
};

#endif