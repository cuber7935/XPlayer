#include "XVideoThread.h"
#include "XFFempeg.h"
#include "XAudioPlay.h"
#include <list>

static std::list<AVPacket> gvideos;
bool gIsExit = false;
static int apt = -1;

XVideoThread::XVideoThread()
{
}

void XVideoThread::run()
{
	char out[10000] = {0};
	while (!gIsExit)
	{
		//��������ͣ״̬
		if (!XFFempeg::getInstance()->GetPlayFlag())
		{
			msleep(10);   //����cpu��ʹ��
			continue;
		}
		//ѭ������list�� �ж���Ƶ ����Ƶ�Ƿ�ͬ��
		while (gvideos.size() > 0)
		{
			AVPacket pack = gvideos.front();
			if (XFFempeg::getInstance()->GetPacketFps(&pack)  > apt)
			{
				break;
			}
			XFFempeg::getInstance()->Decode(&pack);
			av_packet_unref(&pack);
			gvideos.pop_front();
		}

		int free = XAudioPlay::getInstance()->GetFree();
		if (free < 10000)
		{
			msleep(10);   //����cpu��ʹ��
			continue;
		}
		AVPacket pkt = XFFempeg::getInstance()->Read();
		if (pkt.size <= 0)
		{
			msleep(10);   //����cpu��ʹ��
			continue;
		}
		//��Ƶ��
		if (pkt.stream_index == XFFempeg::getInstance()->GetAudioFlag())
		{
			apt = XFFempeg::getInstance()->Decode(&pkt);
			av_packet_unref(&pkt);
			//�ز���
			int len = XFFempeg::getInstance()->ToPCM(out);
			//������Ƶ
			XAudioPlay::getInstance()->Write(out, len);
			continue;
		}
		//��Ƶ��
		else if (pkt.stream_index == XFFempeg::getInstance()->GetStreamFlag())
		{
			//XFFempeg::getInstance()->Decode(&pkt);
			//av_packet_unref(&pkt);
			//if (XFFempeg::getInstance()->GetFps() > 0)
			//{
			//	msleep(1000 / XFFempeg::getInstance()->GetFps());
			//}
			gvideos.push_back(pkt);
		}
		else
		{
			av_packet_unref(&pkt);
			continue;
		}
	}
}

XVideoThread::~XVideoThread()
{
}
