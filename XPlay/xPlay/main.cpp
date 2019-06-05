#include "xPlay.h"
#include <QtWidgets/QApplication>
#include <QAudioOutPut>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	xPlay w;
	w.show();
	return a.exec();
}
