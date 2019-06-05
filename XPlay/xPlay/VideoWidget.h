#ifndef _VIDEOWIDGET_H_
#define _VIDEOWIDGET_H_

#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
	VideoWidget(QWidget* parent = nullptr);
	virtual ~VideoWidget();

	//窗口的绘制
	void paintEvent(QPaintEvent* ev)override;
	//定时器
	void timerEvent(QTimerEvent* ev)override;
};

#endif