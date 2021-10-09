#include "XFtpServerCMD.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include<iostream>
#include<string.h>
using namespace std;
void EventCB(struct bufferevent* bev, short what, void* arg)
{
	XFtpServerCMD* cmd = (XFtpServerCMD*)arg;
	//如果对方网络断掉或者机器死机，有可能收不到BEV_EVENT_EOF的数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR" << endl;
		bufferevent_free(bev);
		delete cmd;
	}
}
//子线程 XThread event事件分发中被调用
static void ReadCB(bufferevent* bev, void* arg)
{
	XFtpServerCMD* CMD = (XFtpServerCMD*)arg;
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0)break;
		data[len] = '\0';
		cout << data << flush;
		//测试代码 要清理
		if (strstr(data, "quit"))
		{
			bufferevent_free(bev);
			delete CMD;
			break;
		}
	}
}

//初始化任务 运行在子线程中
bool XFtpServerCMD::Init()
{
	cout << "XFtpServerCMD::Init" << endl;
	//监听socket bufferevent
	//base socket
	bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);  //base在子线程里的Setup初始化了
	bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//添加超时 
	timeval rt = { 10,0 };
	bufferevent_set_timeouts(bev, &rt, 0);

	return true;
}