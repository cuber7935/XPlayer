#ifndef _VIDEOWIDGET_H_
#define _VIDEOWIDGET_H_

#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
	VideoWidget(QWidget* parent = nullptr);
	virtual ~VideoWidget();

	//���ڵĻ���
	void paintEvent(QPaintEvent* ev)override;
	//��ʱ��
	void timerEvent(QTimerEvent* ev)override;
};

#endif