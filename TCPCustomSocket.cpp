#include "TCPCustomSocket.h"

//构造函数主要用于初始化类的成员变量
CTCPCustomSocket::CTCPCustomSocket()		
{
	m_TCPServerSocket = NULL;				//引用TCP服务端监听socket
	memset(m_RemoteHost, 0, 50);			//远程主机IP地址
	m_RemotePort = 0;						//远程主机端口号
	m_socket = 0;							//通讯socket句柄
}

//析构函数
CTCPCustomSocket::~CTCPCustomSocket()
{

}

//Open函数用于创建与某个socket的通讯线程，并设置socket为异步模式,获取服务端类的地址
bool CTCPCustomSocket::Open(CTCPServerSocket* pTCPServer)
{
	//创建线程退出事件，第二个参数为false则事件响应后自动重置，第三个参数设置为false则事件初始化为未触发
	m_exitThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//创建通讯线程
	m_tcpThreadHandle = CreateThread(NULL, 0, TCPCustomFunc, this, 0, NULL);
	//如果通讯线程未创建成功，则关闭客户端socket，客户端socket在服务端的监听线程中创建
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return false;
	}

	//设置通讯模式为异步模式
	DWORD u1 = 1;
	ioctlsocket(m_socket, FIONBIO, &u1);
	//获取服务端类实例指针
	m_TCPServerSocket = pTCPServer;
	return true;
}

//关闭socket，关闭线程
bool CTCPCustomSocket::Close()
{
	//发送通讯线程结束事件
	SetEvent(m_exitThreadEvent);

	//等待1秒钟，如果读线程没有退出，则强制退出
	if (WaitForSingleObject(/*m_exitThreadEvent*/m_tcpThreadHandle, 1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_tcpThreadHandle, 0);
	}
	m_tcpThreadHandle = NULL;
	CloseHandle(m_exitThreadEvent);		//关闭事件句柄

	//关闭socket，释放资源
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

//向客户端发送数据
bool CTCPCustomSocket::SendData(const char* buf, DWORD dwBufLen)
{
	int nBytes = 0;					//一次发送的字节数
	int nSendBytes = 0;				//当前总共发送的字节数

	while(nSendBytes<dwBufLen)
	{
		nBytes = send(m_socket, buf+nSendBytes, dwBufLen-nSendBytes, 0);

		if (nBytes == SOCKET_ERROR)
		{
			int iErrorCode = WSAGetLastError();
			//触发socket的error事件
			if (m_TCPServerSocket->OnClientError)
			{
				m_TCPServerSocket->OnClientError(m_TCPServerSocket->m_pOwner, this, iErrorCode);
			}
			//触发与服务器断开连接事件
			if (m_TCPServerSocket->OnClientClose)
			{
				m_TCPServerSocket->OnClientClose(m_TCPServerSocket->m_pOwner, this);
			}

			//调用Close()函数，关闭socket退出线程
			Close();
			//将其从客户端列表中移出
			m_TCPServerSocket->RemoteClient(this);

			return false;
		}

		nSendBytes = nSendBytes + nBytes;

		if (nSendBytes<dwBufLen)
		{
			Sleep(1000);
		}
	}
	return true;
}

//用于监听已连接的客户端socket通讯事件，主要用于从客户端读取数据
DWORD WINAPI CTCPCustomSocket::TCPCustomFunc(PVOID lparam)
{
	int ret;
	//获取CTCPCustomSocket类实例指针
	CTCPCustomSocket* pSocket = (CTCPCustomSocket*)lparam;

	//定义读事件集合
	fd_set fdRead;
	TIMEVAL aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;

	while (true)
	{
		//收到线程退出事件，结束线程
		if (WaitForSingleObject(pSocket->m_exitThreadEvent, 0) == WAIT_OBJECT_0)			//第二个参数设置为0，如果事件未触发，则函数直接返回不用等待
		{
			break;
		}

		//置空读事件集合
		FD_ZERO(&fdRead);
		//给pSocket设置读事件
		FD_SET(pSocket->m_socket, &fdRead);
		//调用select函数，判断是否有读事件发生
		ret = select(0, &fdRead, NULL, NULL, &aTime);

		if (ret == SOCKET_ERROR)
		{
			//触发错误事件
			if(pSocket->m_TCPServerSocket->OnClientError)
			{
				pSocket->m_TCPServerSocket->OnClientError(pSocket->m_TCPServerSocket->m_pOwner, pSocket, 1);		//状态显示，表格移除
			}
			//触发与客户端连接的socket关闭事件
			if (pSocket->m_TCPServerSocket->OnClientClose)
			{
				pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
			}
			//关闭客户端socket,结束线程,并将其从已连接客户端存储列表中移出
			closesocket(pSocket->m_socket);
			pSocket->m_TCPServerSocket->RemoteClient(pSocket);			//释放内存，移出存储列表
			break;
		}

		if (ret>0)
		{
			//判断是否是读事件
			if (FD_ISSET(pSocket->m_socket, &fdRead))
			{
				char recvBuf[1024];
				int recvLen;
				ZeroMemory(recvBuf, 1024);
				recvLen = recv(pSocket->m_socket, recvBuf, 1024, 0);

				if (recvLen == SOCKET_ERROR)
				{
					int nErrorCode = WSAGetLastError();
					//触发与客户端连接的socket错误事件
					if(pSocket->m_TCPServerSocket->OnClientError)
					{
						pSocket->m_TCPServerSocket->OnClientError(pSocket->m_TCPServerSocket->m_pOwner, pSocket, nErrorCode);
					}
					//触发与客户端连接的socket关闭事件
					if (pSocket->m_TCPServerSocket->OnClientClose)
					{
						pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
					}
					//关闭socket
					closesocket(pSocket->m_socket);
					//从已连接客户端列表中移除客户端，（是不是在每次关闭socket时都应该移除socket）
					pSocket->m_TCPServerSocket->RemoteClient(pSocket);			
					//结束线程
					break;
				}
				else if (recvLen == 0)			//标示连接已经从容关闭
				{
					//触发客户端断开连接事件
					if (pSocket->m_TCPServerSocket->OnClientClose)
					{
						pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
					}
					//关闭socket
					closesocket(pSocket->m_socket);
					//从已连接客户端列表中移除客户端，（是不是在每次关闭socket时都应该移除socket）
					pSocket->m_TCPServerSocket->RemoteClient(pSocket);
					
					//结束线程
					break;
				}
				else
				{
					//触发与客户端连接的读事件
					if (pSocket->m_TCPServerSocket->OnClientRead)
					{
						pSocket->m_TCPServerSocket->OnClientRead(pSocket->m_TCPServerSocket->m_pOwner, pSocket, recvBuf, recvLen);
					}
				}
			}
		}
	}
	return 0;
}

