#pragma once

#include "GlobalValue.h"


//定义客户端连接建立事件
typedef void(CALLBACK* ONCLIENTCONNECT) (void* pOwner, void* pSocket);			//第二个参数用于传递某个客户端的实例
//定义客户端socket关闭事件
typedef void(CALLBACK* ONCLIENTCLOSE) (void* pOwner, void* pSocket);
//定义客户端socket错误事件
typedef void(CALLBACK* ONCLIENTERROR) (void* pOwner, void* pSocket, int nErrorCode);
//定义服务端socket错误事件
typedef void(CALLBACK* ONSERVERERROR) (void* pOwner, void* pSocket, int nErrorCode);
//定义客户端数据接收事件
typedef void(CALLBACK* ONCLIENTREAD) (void* pOwner, void* pSocket, const char* buf, DWORD dwBufLen);

class CTCPServerSocket
{
public:
	char				m_LocalIPAddr[50];					//设置服务器IP地址
	int					m_LocalPort;						//设置服务器端口号

	void*				m_pOwner;							//父对象句柄

// 	CTCPCustomSocket*	ActiveClientList[10];				//存储已连接的客户端指针
// 	int					ActiveClientListPos;				//已连接客户端指针数组的当前容量

private:
	SOCKET				m_ServerSocket;						//TCP服务监听socket
	HANDLE				m_serverThreadHandle;				//监听线程句柄
	HANDLE				m_exitServerThreadEvent;			//监听线程退出事件句柄

public:
	ONCLIENTCONNECT		OnClientConnect;
	ONCLIENTCLOSE		OnClientClose;
	ONCLIENTERROR		OnClientError;
	ONCLIENTREAD		OnClientRead;
	ONSERVERERROR		OnServerError;

public:
	CTCPServerSocket();						//
	~CTCPServerSocket();

	//初始化socket资源， 创建tcp监听套接字， 将监听套接字与本地地址绑定， 开始监听， 创建监听线程等待客户端接入
	int	Open(void* pOwner, const char* ServerIPAddr, int serverPort);				
	//关闭监听线程， 关闭与所有客户端的链接， 清空客户端列表， 释放socket资源
	bool Close();
	//调用某个客户端的SendData发送数据
	bool SendData(void* pCustomSocket, const char* buf, DWORD dwBufLen);
	//删除客户端对象
	void RemoteClient(void* pClient);

	//监听线程函数，监听客户端套接字事件，有客户端接入则将其加入客户端列表，并创建相应的客户端线程
	static DWORD WINAPI TCPServerFunc(PVOID lparam);
};

