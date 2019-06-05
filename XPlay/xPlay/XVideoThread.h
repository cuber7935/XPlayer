#ifndef _XVIDEO_THREAD_H_
#define _XVIDEO_THREAD_H_

#include <QThread>

class XVideoThread: public QThread
{
public:
	static XVideoThread* getInstance()
	{
		static XVideoThread xt;
		return &xt;
	}
	virtual void run()override;

	virtual ~XVideoThread();
	
private:
	XVideoThread();
};

#endif