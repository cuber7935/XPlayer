#pragma once

#include <QtWidgets/QWidget>
#include "ui_xPlay.h"

class xPlay : public QWidget
{
	Q_OBJECT

public:
	xPlay(QWidget *parent = Q_NULLPTR);
	~xPlay();

	void timerEvent(QTimerEvent* ev)override;
	//主窗口发生变化
	void resizeEvent(QResizeEvent* ev)override;

public slots:
	void open();
	void play();
	void sliderPress();
	void sliderRelease();

private:
	Ui::xPlayClass ui;
};
