#include "XSlider.h"
#include <QMouseEvent>

XSlider::XSlider(QWidget* widget):QSlider(widget)
{
}

XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent * ev)
{
	int value = ((float)ev->pos().x() / (float)width())*(this->maximum() + 1);
	this->setValue(value);
	QSlider::mousePressEvent(ev);
}
