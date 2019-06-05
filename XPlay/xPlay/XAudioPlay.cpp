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
	//��ʼ/��ͣ
	virtual bool Start()override
	{
		Stop();
		_mutex.lock();
		//��Ƶ��ʽ����
		QAudioFormat fmt;
		//���� һ���Ӳɼ���������
		fmt.setSampleRate(GetSampleRate());
		//������С  16λ�� �����ֽ�
		fmt.setSampleSize(GetSampleSize());
		//��������, 2����˫����
		fmt.setChannelCount(GetChannelCount());
		//��ʽ  pcmûѹ������ͨ��
		fmt.setCodec("audio/pcm");
		//��Ƶ���ݴ���(С��β)
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		//����������
		fmt.setSampleType(QAudioFormat::UnSignedInt);

		//��Ƶ���������
		_out = new QAudioOutput(fmt);
		//�õ��������io��
		_io = _out->start();
		_mutex.unlock();
		return true;
	}
	//����
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
	//��ͣ
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
	//д����
	virtual bool Write(const char* data, int size)override
	{
		_mutex.lock();
		if(_io)
			_io->write(data, size);
		_mutex.unlock();
		return true;
	}

	//��ȡʣ��ռ�
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

//���òɼ���������
void XAudioPlay::SetSampleRate(int sampleRate)
{
	_sampleRate = sampleRate;
}
//����������С
void XAudioPlay::SetSampleSize(int sampleSize)
{
	_sampleSize = sampleSize;
}
//��������
void XAudioPlay::SetChannelCount(int channelCount)
{
	_channelCount = channelCount;
}

/*��ȡ��Ƶ��ʽ*/
int XAudioPlay::GetSampleRate()const
{
	return _sampleRate;
}
/*��ȡ��Ƶ��ʽ��С*/
int XAudioPlay::GetSampleSize()const
{
	return _sampleSize;
}
/*��ȡ����*/
int XAudioPlay::GetChannelCount()const
{
	return _channelCount;
}