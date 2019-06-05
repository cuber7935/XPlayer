#ifndef _XFFEMPEG_H_
#define _XFFEMPEG_H_

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>   //�ز���
}

#include <string>
#include <QMutex>

class XFFempeg
{
public:
	/*����ģʽ*/
	static XFFempeg* getInstance()
	{
		static XFFempeg ff;
		return &ff;
	}
	/*
	**@����Ƶ·��  ��֮ǰ���Ȱ��ϴδ򿪵Ĺر�
	**@para   path  ��Ƶ�ļ�·��
	**@return bool  ʧ��ͨ��GetError()��ȡ������Ϣ
	*/
	bool Open(const char* path);
	
	/*
	**@��ȡ��Ƶ֡
	**@return AVPacket�ṹ��   ����ֵ��Ҫ�û�����
	*/
	AVPacket Read();

	/*
	**@��ȡ������Ϣ
	*/
	std::string GetError();
	
	/*
	**@����
	**@para   pkt  ��Ƶ֡�ṹ��
	**@return AVFrame   ���ؽ����� yuv
	*/
	int Decode(const AVPacket* pkt);

	bool ToRGB(char* out, int outWidth, int outHeight);

	/*�ز���*/
	int ToPCM(char* out);

	/*
	**@�û��϶���������ʵ�������ȡ��Ƶ
	**@para   pos  ���λ�ã� �ٷֱ�
	*/
	bool Seek(float pos);

	/*�ͷ���Դ*/
	void Close();

	/*��ȡ��ʱ��*/
	int GetTotalMs()const;
	/*��ȡ��Ƶ����ʶ*/
	int GetStreamFlag()const;
	/*��ȡ��Ƶ����ʶ*/
	int GetAudioFlag()const;
	/*��ȡ��ǰ���Ž���*/
	int GetPtsMs()const;
	/*��ȡ���ű�ʶ*/
	bool GetPlayFlag()const;
	/*���ò��ű�ʶ*/
	void SetPlayFlag(bool play);

	/*��ȡAVPacket��֡��*/
	int GetPacketFps(const AVPacket* pkt);

	/*��ȡ��Ƶ��ʽ*/
	int GetSampleRate()const;
	/*��ȡ��Ƶ��ʽ��С*/
	int GetSampleSize()const;
	/*��ȡ����*/
	int GetChannelCount()const;

	virtual ~XFFempeg();
private:
	XFFempeg();
	
	AVFormatContext* _ic = NULL;			 //��Ŵ򿪵��ļ��������Ϣ
	AVFrame*		 _yuv = NULL;			 //��Ž�������Ƶ
	AVFrame*         _pcm = NULL;            //��Ž�������Ƶ
	SwsContext*		 _cCtx = NULL;           //ת����
	SwrContext*      _aCtx = NULL;           //��Ƶ�ز���Ԥ��
	char			 _errbuf[1024];			 //���������Ϣ
	QMutex			 _mutex;				 //�������
	int				 _totalMs = 0;			 //��ʱ��	
	int				 _videoStream = 0;		 //��Ƶ����
	int              _audioStream = 0;       //��Ƶ���� //������Ļ��
	int              _fps = 0;               //֡��
	int              _pts = 0;               //��ǰ���Ž���
	bool             _play = false;          //��ʶ�Ƿ񲥷�
	int				 _sampleRate = 48000;    //һ���Ӳɼ���������	
	int				 _sampleSize = 16;       //������С  16λ�� �����ֽ�
	int				 _channelCount = 2;      //��������, 2����˫����
	//int              _pktFps = 0;            //AVPacket��֡��
};

#endif