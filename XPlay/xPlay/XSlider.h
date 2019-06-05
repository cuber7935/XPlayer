#ifndef _XSLIDEF_H_
#define _XSLIDEF_H_
#include <QSlider>

class XSlider : public QSlider
{
	Q_OBJECT
public:
	XSlider(QWidget* widget = nullptr);
	~XSlider();

	void mousePressEvent(QMouseEvent* ev)override;
};

#endif