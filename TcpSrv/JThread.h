#ifndef _JTHREAD_H_
#define _JTHREAD_H_

#include <Windows.h>

class JThread
{
public:
	JThread();
	virtual ~JThread();

	virtual void Run() = 0;    //运行函数为纯虚函数

	static unsigned __stdcall ThreadFun(void* p);  //线程函数
	bool Start();   //启动

	void Wait(DWORD timeout = INFINITE);  //阻塞
	void SetAutoDel(bool autoDel);

private:
	HANDLE hThread_;
	unsigned threadId_;
	bool autoDel_;
};

#endif // _JTHREAD_H_