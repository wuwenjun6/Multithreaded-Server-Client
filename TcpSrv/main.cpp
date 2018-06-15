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
			//����
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
				send(conn_, buf, strlen(buf), 0);  //�����ȥ
			}
			memset(buf, 0, sizeof buf);  //��0
		}
		closesocket(conn_);
	}



private:
	SOCKET conn_;
};




int main(void)
{
	WORD wVersionRequested;  //�����ֽ�,ָ��ʹ�õİ汾��
	WSADATA wsaData;     //WSADATA �ṹ�洢 Windows �׽��ֵ��÷��صĳ�ʼ����Ϣ��
	int err;

	//makeword(a,b)�ǽ�����byte�ͺϲ���һ��word�ͣ�һ���ڸ�8λ(b)��һ���ڵ�8λ(a) 
	wVersionRequested = MAKEWORD(2, 2);  //ָ���汾��

	err = WSAStartup(wVersionRequested, &wsaData);   //����Windows Socket
	if (err != 0) {
		return 1;
	}

	//���Э��ջ�İ�װ��Ϣ
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return 1;
	}

	//����ʹ������ķ���
	//if (wsaData.wVersion != wVersionRequested)
	//{
	//	//Winsock�汾��ƥ��
	//	WSACleanup();
	//	return 1;
	//}


	SOCKET listenfd;    //����socket
	listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  //TCPЭ�� 
	if (listenfd == INVALID_SOCKET)   //����ʧ��
	{
		cout << "error with code = " << WSAGetLastError() << endl;     //WSAGetLastError()��ȡ������
		exit(1);
	}

	sockaddr_in servaddr;   //��������ַ
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;   //��ַ��
  //servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //ָ��IP��ַ  4�ֽ������ֽ���ת��Ϊ�����ֽ���
	servaddr.sin_port = htons(8888);   //�˿ں�  2�ֽ������ֽ���ת��Ϊ�����ֽ���

	int ret;
	int opt = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));  //���õ�ַ�ظ�����
	if (ret == SOCKET_ERROR)
	{
		cout << "error with code = " << WSAGetLastError() << endl;
		exit(1);
	}


	//��
	ret = bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "error with code = " << WSAGetLastError() << endl;
		exit(1);
	}

	//����
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

		//��һ��ʮ���������ֽ���ת��Ϊ���ʮ����IP��ʽ���ַ���      2�ֽ������ֽ���ת��Ϊ�����ֽ���
		cout << inet_ntoa(peeraddr.sin_addr) << " " << ntohs(peeraddr.sin_port) << endl;
		ServiceThread * t = new ServiceThread(conn);
		t->SetAutoDel(true);
		t->Start();
	}

	WSACleanup();


	return 0;
}