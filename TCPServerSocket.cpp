#include "TCPServerSocket.h"
#include "TCPCustomSocket.h"

CTCPServerSocket::CTCPServerSocket()
{
	//创建监听线程退出事件句柄
	m_exitServerThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	OnClientConnect = NULL;
	OnClientClose   = NULL;
	OnClientError   = NULL;
	OnClientRead    = NULL;
	OnServerError   = NULL;

	memset(m_LocalIPAddr, 0, 50);
	m_LocalPort = 0;
}

CTCPServerSocket::~CTCPServerSocket()
{
	CloseHandle(m_exitServerThreadEvent);
}

DWORD WINAPI CTCPServerSocket::TCPServerFunc(PVOID lparam)
{
	CTCPServerSocket* pServerSocket = (CTCPServerSocket*)lparam;

	//定义读事件集合
	fd_set fdRead;
	int ret;
	TIMEVAL aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 1;
	while (true)
	{
		//收到退出事件，结束线程
		if (WaitForSingleObject(pServerSocket->m_exitServerThreadEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		FD_ZERO(&fdRead);
		FD_SET(pServerSocket->m_ServerSocket, &fdRead);

		//监听事件
		ret = select(0, &fdRead, NULL, NULL, &aTime);

		if (ret == SOCKET_ERROR)
		{
			//触发服务器socket错误事件
			int iErrorCode = WSAGetLastError();
			if (pServerSocket->OnServerError)
			{
				pServerSocket->OnServerError(pServerSocket->m_pOwner, pServerSocket, iErrorCode);
			}
			//关闭服务器套接字
			//closesocket(pServerSocket->m_ServerSocket);
			pServerSocket->Close();
			break;
		}

		if (ret>0)
		{
			if (FD_ISSET(pServerSocket->m_ServerSocket, &fdRead))
			{
				//创建与客户端连接的套接字
				SOCKADDR_IN clientaddr;
				int namelen = sizeof(clientaddr);

				CTCPCustomSocket* pClientSocket = new CTCPCustomSocket();
				pClientSocket->m_socket = accept(pServerSocket->m_ServerSocket, (struct sockaddr*)&clientaddr, &namelen);

				//接收到与客户端连接
				if (pClientSocket->m_socket)
				{
					strcpy(pClientSocket->m_RemoteHost, inet_ntoa(clientaddr.sin_addr));
					//pClientSocket->m_RemoteHost = inet_ntoa(clientaddr.sin_addr);
					pClientSocket->m_RemotePort = ntohs(clientaddr.sin_port);

					//打开客户端实例服务线程,如果成功则将将其加入到客户端存储列表,并触发客户端建立链接事件
					if(pClientSocket->Open(pServerSocket))
					{
						//添加到已连接客户端列表中
						ActiveClientList.push_back(pClientSocket);

						//触发与客户端建立连接事件
						if (pServerSocket->OnClientConnect)
						{
							//状态栏显示，表格要添加新增客户端
							pServerSocket->OnClientConnect(pServerSocket->m_pOwner, pClientSocket);
						}
					}												
				}
				else
				{
					delete pClientSocket;
					pClientSocket = NULL;
				}
			}
		}
	}
	return 0;
}

void CTCPServerSocket::RemoteClient(void* pClient)			//将某个客户端移出已连接客户端存储列表
{
	CTCPCustomSocket* ptempClient = (CTCPCustomSocket*)pClient;

	beginpos = ActiveClientList.begin();
	int ClientNum = ActiveClientList.size();				//获取已连接客户端存储列表的大小

	for (int i = 0; i<ClientNum; i++)						//若果传过来的指针不在存储列表中，则不会做任何处理
	{
		if (ActiveClientList[i] == ptempClient)
		{
			//ptempClient->Close();							//在调用该函数之前就已经关闭了socket，所以在此不能重复关闭
			delete ptempClient;
			ActiveClientList[i] = NULL;
			ActiveClientList.erase(beginpos+i);				//此处一定要删除，否则在close中会重复关闭socket与delete
			break;
		}
	}
}

int CTCPServerSocket::Open(void* pOwner, const char* ServerIPAddr, int serverPort)
{
	strcpy(m_LocalIPAddr, ServerIPAddr);			//服务器的IP地址
	m_LocalPort = serverPort;						//服务器的端口号
	m_pOwner = pOwner;

	WSADATA wsa;

	//初始化socket资源
	if (WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{
		return -1;			//初始化socket资源失败
	}

	//创建监听套接字
	if ((m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		return -2;			//监听套接字创建失败
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory((char*)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_LocalPort);
	serverAddr.sin_addr.s_addr = inet_addr(m_LocalIPAddr);

	//绑定监听套接字
	if (bind(m_ServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0)
	{
		return -3;				//绑定监听套接字失败
	}

	//监听套接字开始监听
	if (listen(m_ServerSocket, 8)!=0)
	{
		return -4;
	}

	//设置监听套接字通讯模式为异步模式
	DWORD u1 = 1;
	ioctlsocket(m_ServerSocket, FIONBIO, &u1);

	ResetEvent(m_exitServerThreadEvent);
	//创建通讯线程，在线程里，等待客户端接入
	m_serverThreadHandle = CreateThread(NULL, 0, TCPServerFunc, this, 0, NULL);
	if (m_serverThreadHandle == NULL)
	{
		closesocket(m_ServerSocket);
		return -5;
	}
	return 1;
}

//关闭TCP服务
bool CTCPServerSocket::Close()
{
	//结束监听线程
	SetEvent(m_exitServerThreadEvent);

	if (WaitForSingleObject(/*m_exitServerThreadEvent*/m_serverThreadHandle, 1000) == WAIT_TIMEOUT)
	{
		//强制关闭线程
		TerminateThread(m_serverThreadHandle, 0);
	}

	m_serverThreadHandle = NULL;

	beginpos = ActiveClientList.begin();
	int ClientNum = ActiveClientList.size();

	//关闭与所有客户端的链接				//该处以后用队列实现
	for (int i = ClientNum-1; i>=0; i--)
	{
// 		(CTCPCustomSocket*)ActiveClientList[ActiveClientListPos]->Close();
// 		delete (CTCPCustomSocket*)ActiveClientList[ActiveClientListPos];
		CTCPCustomSocket* tempCustom = (CTCPCustomSocket*)ActiveClientList[i];
		tempCustom->Close();
		delete tempCustom;
		ActiveClientList.erase(beginpos+i);	
	}
	
	//关闭socket，释放资源
	int err = closesocket(m_ServerSocket);
	if (err == SOCKET_ERROR)
	{
		return false;
	}
	
	//释放socket资源
	WSACleanup();
	return true;
}

//给某个客户端发送数据
bool CTCPServerSocket::SendData(void* pCustom, const char* buf, DWORD dwBufLen)
{
	CTCPCustomSocket* pCustomS = (CTCPCustomSocket*)pCustom;
	if (pCustomS == NULL)
	{
		return false;
	}

	if (!(pCustomS->SendData(buf, dwBufLen)))
	{
		return false;
	}
	return true;
}
