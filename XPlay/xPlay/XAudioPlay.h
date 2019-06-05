#ifndef _XAUDIO_PLAY_H_
#define _XAUDIO_PLAY_H_


class XAudioPlay
{
public:
	static XAudioPlay* getInstance();
	//��ʼ
	virtual bool Start() = 0;
	//��ͣ
	virtual void Stop() = 0;
	//����
	virtual void Play(bool isPlay)= 0;
	//д����
	virtual bool Write(const char* data, int size) = 0;
	//��ȡʣ��ռ�
	virtual int GetFree() = 0;
	
	//���òɼ���������
	void SetSampleRate(int sampleRate);
	//����������С
	void SetSampleSize(int sampleSize);
	//��������
	void SetChannelCount(int channelCount);

	/*��ȡ��Ƶ��ʽ*/
	int GetSampleRate()const;
	/*��ȡ��Ƶ��ʽ��С*/
	int GetSampleSize()const;
	/*��ȡ����*/
	int GetChannelCount()const;

	virtual ~XAudioPlay();
	

protected:
	XAudioPlay();
private:
	int _sampleRate = 48000;                   //һ���Ӳɼ���������	
	int _sampleSize = 16;                      //������С  16λ�� �����ֽ�
	int _channelCount = 2;                     //��������, 2����˫����
};

#endif