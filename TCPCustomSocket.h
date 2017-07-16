#pragma once
#include "TCPServerSocket.h"
#include "GlobalValue.h"

class CTCPCustomSocket
{
public:
	CTCPServerSocket*		m_TCPServerSocket;				//引用TCP服务端监听Socket
	char					m_RemoteHost[50];				//客户端IP地址
	int						m_RemotePort;					//客户端端口号

	SOCKET					m_socket;						//通讯socket句柄

private:
	HANDLE					m_exitThreadEvent;				//通讯线程退出事件句柄
	HANDLE					m_tcpThreadHandle;				//通讯线程句柄

public:
	CTCPCustomSocket();							//初始化成员变量
	~CTCPCustomSocket();						//

	bool		Open(CTCPServerSocket* pTCPServer);			//创建通讯线程
	bool		Close();									//关闭通讯线程，关闭socket
	bool		SendData(const char* buf, DWORD dwBufLen);	//向客户端发送数据

	static DWORD WINAPI TCPCustomFunc(PVOID lparam);				//通讯线程，只要用于读取数据
};