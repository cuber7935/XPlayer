#include "XAudioPlay.h"
#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>

class CXAudioPlay : public XAudioPlay
{
public:
	QAudioOutput* _out = nullptr;
	QIODevice* _io = nullptr;
	QMutex _mutex;
	//开始/暂停
	virtual bool Start()override
	{
		Stop();
		_mutex.lock();
		//音频格式对象
		QAudioFormat fmt;
		//设置 一秒钟采集多少声音
		fmt.setSampleRate(GetSampleRate());
		//样本大小  16位， 两个字节
		fmt.setSampleSize(GetSampleSize());
		//声道设置, 2代表双声道
		fmt.setChannelCount(GetChannelCount());
		//格式  pcm没压缩，普通的
		fmt.setCodec("audio/pcm");
		//音频数据次序(小结尾)
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		//样本的类型
		fmt.setSampleType(QAudioFormat::UnSignedInt);

		//音频输出流对象
		_out = new QAudioOutput(fmt);
		//得到输入输出io流
		_io = _out->start();
		_mutex.unlock();
		return true;
	}
	//播放
	virtual void Play(bool isPlay)override
	{
		_mutex.lock();
		if (!_out)
		{
			_mutex.unlock();
			return;
		}
		if (isPlay)
			_out->resume();
		else
			_out->suspend();

		_mutex.unlock();
	}
	//暂停
	virtual void Stop()override
	{
		_mutex.lock();
		if (_out)
		{
			_out->stop();
			delete _out;
			_out = nullptr;
			_io = nullptr;
		}
		_mutex.unlock();
	}
	//写数据
	virtual bool Write(const char* data, int size)override
	{
		_mutex.lock();
		if(_io)
			_io->write(data, size);
		_mutex.unlock();
		return true;
	}

	//获取剩余空间
	virtual int GetFree()override
	{
		_mutex.lock();
		if (!_out)
		{
			_mutex.unlock();
			return 0;
		}
		int free = _out->bytesFree();
		_mutex.unlock();
		return free;
	}
};

XAudioPlay::XAudioPlay()
{
}

XAudioPlay * XAudioPlay::getInstance()
{
	static CXAudioPlay ap;
	return &ap;
}

XAudioPlay::~XAudioPlay()
{
}

//设置采集多少声音
void XAudioPlay::SetSampleRate(int sampleRate)
{
	_sampleRate = sampleRate;
}
//设置样本大小
void XAudioPlay::SetSampleSize(int sampleSize)
{
	_sampleSize = sampleSize;
}
//设置声道
void XAudioPlay::SetChannelCount(int channelCount)
{
	_channelCount = channelCount;
}

/*获取音频样式*/
int XAudioPlay::GetSampleRate()const
{
	return _sampleRate;
}
/*获取音频样式大小*/
int XAudioPlay::GetSampleSize()const
{
	return _sampleSize;
}
/*获取声道*/
int XAudioPlay::GetChannelCount()const
{
	return _channelCount;
}