//Server
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include<process.h>
#include"JThread.h"
using namespace std;


class ServiceThread :public JThread
{
public:
	ServiceThread(SOCKET conn) :conn_(conn)
	{
		cout << "ServiceThread" << endl;

	}
	~ServiceThread()
	{
		cout << "~ServiceThread" << endl;

	}
	void Run()
	{
		
		char buf[1024] = { 0 };

		while (1)
		{
			//接收
			int ret = recv(conn_, buf, sizeof(buf), 0);
			if (ret == SOCKET_ERROR)
			{
				cout << "error with code = " << WSAGetLastError() << endl;
				exit(1);
			}
			if (ret == 0)
			{
				cout << "client close" << endl;
				break;
			}
			if (ret > 0)
			{
				cout << buf << endl;
				send(conn_, buf, strlen(buf), 0);  //回射回去
			}
			memset(buf, 0, sizeof buf);  //置0
		}
		closesocket(conn_);
	}



private:
	SOCKET conn_;
};




int main(void)
{
	WORD wVersionRequested;  //两个字节,指定使用的版本号
	WSADATA wsaData;     //WSADATA 结构存储 Windows 套接字调用返回的初始化信息对
	int err;

	//makeword(a,b)是将两个byte型合并成一个word型，一个在高8位(b)，一个在低8位(a) 
	wVersionRequested = MAKEWORD(2, 2);  //指定版本号

	err = WSAStartup(wVersionRequested, &wsaData);   //启动Windows Socket
	if (err != 0) {
		return 1;
	}

	//检查协议栈的安装信息
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return 1;
	}

	//或者使用下面的方法
	//if (wsaData.wVersion != wVersionRequested)
	//{
	//	//Winsock版本不匹配
	//	WSACleanup();
	//	return 1;
	//}


	SOCKET listenfd;    //创建socket
	listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  //TCP协议 
	if (listenfd == INVALID_SOCKET)   //创建失败
	{
		cout << "error with code = " << WSAGetLastError() << endl;     //WSAGetLastError()获取错误码
		exit(1);
	}

	sockaddr_in servaddr;   //服务器地址
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;   //地址族
  //servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //指定IP地址  4字节主机字节序转换为网络字节序
	servaddr.sin_port = htons(8888);   //端口号  2字节主机字节序转换为网络字节序

	int ret;
	int opt = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));  //设置地址重复利用
	if (ret == SOCKET_ERROR)
	{
		cout << "error with code = " << WSAGetLastError() << endl;
		exit(1);
	}


	//绑定
	ret = bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "error with code = " << WSAGetLastError() << endl;
		exit(1);
	}

	//监听
	ret = listen(listenfd, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		cout << "error with code = " << WSAGetLastError() << endl;
		exit(1);
	}

	SOCKET conn;
	sockaddr_in peeraddr;
	int peerlen;
	while (1)
	{
		peerlen = sizeof(peeraddr);
		conn = accept(listenfd, (sockaddr*)&peeraddr, &peerlen);
		if (conn == INVALID_SOCKET)
		{
			cout << "error with code = " << WSAGetLastError() << endl;
			exit(1);
		}

		//将一个十进制网络字节序转换为点分十进制IP格式的字符串      2字节网络字节序转换为主机字节序
		cout << inet_ntoa(peeraddr.sin_addr) << " " << ntohs(peeraddr.sin_port) << endl;
		ServiceThread * t = new ServiceThread(conn);
		t->SetAutoDel(true);
		t->Start();
	}

	WSACleanup();


	return 0;
}