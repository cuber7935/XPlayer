#include "VideoWidget.h"
#include <QPainter>
#include "XFFempeg.h"
#include "XVideoThread.h"

VideoWidget::VideoWidget(QWidget* parent):QOpenGLWidget(parent)
{
	XVideoThread::getInstance()->start();
	startTimer(20);
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent * ev)
{
	static QImage* image = nullptr;
	//先保存一份原先窗口的大小
	static int swidth = 0;
	static int sheight = 0;
	if (swidth != width() || sheight != height())
	{
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
	}

	if (image == nullptr)
	{
		//用于存放解码后的像素空间
		uchar* buf = new uchar[width()*height() * 4];
		image = new QImage(buf, width(), height(), QImage::Format::Format_ARGB32);
	}

	XFFempeg::getInstance()->ToRGB((char*)image->bits(), width(), height());
	
	QPainter painter;
	painter.begin(this);

	painter.drawImage(QPoint(0, 0), *image);

	painter.end();
}

void VideoWidget::timerEvent(QTimerEvent * ev)
{
	this->update();
}
