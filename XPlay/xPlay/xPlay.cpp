#include "xPlay.h"
#include <QMessageBox>
#include <QFileDialog>
#include "XFFempeg.h"
#include "XAudioPlay.h"

static bool gIsPress = false;
static bool gIsPlay = true;      //默认是播放状态
#define PAUSESHEETSTYLE "QPushButton{border-image:url(:/xPlay/Resources/pause_normal.png); }"
#define PLAYSHEETSTYLE "QPushButton{border-image:url(:/xPlay/Resources/play_normal.png); }"


xPlay::xPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->startTimer(40);
}

xPlay::~xPlay()
{

}

void xPlay::timerEvent(QTimerEvent * ev)
{
	char buf[1024] = { 0 };
	int hour = 0;
	int min = (XFFempeg::getInstance()->GetPtsMs() / 1000) / 60;
	if (min > 60)
	{
		hour = (min / 60);
		min = min % 60;
	}
	int sec = (XFFempeg::getInstance()->GetPtsMs() / 1000) % 60;
	sprintf(buf, "%02d:%02d:%02d/", hour, min, sec);
	ui.playTime->setText(buf);

	//进度条设置
	if (XFFempeg::getInstance()->GetTotalMs() > 0)
	{
		float rate = (float)XFFempeg::getInstance()->GetPtsMs() / (float)XFFempeg::getInstance()->GetTotalMs();
		if (!gIsPress)
		{
			ui.playSlider->setValue(rate * 1000);
		}
	}
}
//主窗口发生变化
void xPlay::resizeEvent(QResizeEvent * ev)
{
	//将视频窗口设置和主窗口大小一样
	ui.openGLWidget->resize(size());

	ui.playBtn->move(this->width() -100, this->height() - 60);
	ui.openBtn->move(this->width() / 2 -10, this->height() - 60);

	ui.playSlider->move(20, this->height() - 80);
	ui.playSlider->resize(this->width() - 20-20, ui.playSlider->height());

	ui.playTime->move(20, this->height() - 50);
	ui.totalTime->move(115, this->height() - 50);
}

//打开文件
void xPlay::open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	if (name.isEmpty())
	{
		return;
	}
	this->setWindowTitle(name);

	if (!XFFempeg::getInstance()->Open(name.toLocal8Bit()))
	{
		QMessageBox::information(this, "err", "file open failed!");
		return;
	}
	
	XAudioPlay::getInstance()->SetSampleRate(XFFempeg::getInstance()->GetSampleRate());
	XAudioPlay::getInstance()->SetSampleSize(XFFempeg::getInstance()->GetSampleSize());
	XAudioPlay::getInstance()->SetChannelCount(XFFempeg::getInstance()->GetChannelCount());

	XAudioPlay::getInstance()->Start();

	char buf[1024] = {0};
	int hour = 0;
	int min = (XFFempeg::getInstance()->GetTotalMs() /1000)/60;
	if (min > 60)
	{
		hour = min / 60;
		min = min % 60;
	}
	int sec = (XFFempeg::getInstance()->GetTotalMs() / 1000) %60;
	sprintf(buf, "%02d:%02d:%02d",hour, min, sec);
	ui.totalTime->setText(buf);

	//gIsPlay = true;
	play();
}

//播放按钮
void xPlay::play()
{
	gIsPlay = !gIsPlay;
	if (gIsPlay)  //停止状态
	{
		//切换按钮样式
		ui.playBtn->setStyleSheet(PLAYSHEETSTYLE);
		XFFempeg::getInstance()->SetPlayFlag(false);
	}
	else  //播放
	{
		ui.playBtn->setStyleSheet(PAUSESHEETSTYLE);
		XFFempeg::getInstance()->SetPlayFlag(true);
	}
}

void xPlay::sliderPress()
{
	gIsPress = true;
}
void xPlay::sliderRelease()
{
	gIsPress = false;
	float pos = (float)ui.playSlider->value() / (float)(ui.playSlider->maximum() + 1);
	XFFempeg::getInstance()->Seek(pos);
}