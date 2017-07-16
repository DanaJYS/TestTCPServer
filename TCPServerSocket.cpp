#include "TCPServerSocket.h"
#include "TCPCustomSocket.h"

CTCPServerSocket::CTCPServerSocket()
{
	//���������߳��˳��¼����
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

	//������¼�����
	fd_set fdRead;
	int ret;
	TIMEVAL aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 1;
	while (true)
	{
		//�յ��˳��¼��������߳�
		if (WaitForSingleObject(pServerSocket->m_exitServerThreadEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		FD_ZERO(&fdRead);
		FD_SET(pServerSocket->m_ServerSocket, &fdRead);

		//�����¼�
		ret = select(0, &fdRead, NULL, NULL, &aTime);

		if (ret == SOCKET_ERROR)
		{
			//����������socket�����¼�
			int iErrorCode = WSAGetLastError();
			if (pServerSocket->OnServerError)
			{
				pServerSocket->OnServerError(pServerSocket->m_pOwner, pServerSocket, iErrorCode);
			}
			//�رշ������׽���
			//closesocket(pServerSocket->m_ServerSocket);
			pServerSocket->Close();
			break;
		}

		if (ret>0)
		{
			if (FD_ISSET(pServerSocket->m_ServerSocket, &fdRead))
			{
				//������ͻ������ӵ��׽���
				SOCKADDR_IN clientaddr;
				int namelen = sizeof(clientaddr);

				CTCPCustomSocket* pClientSocket = new CTCPCustomSocket();
				pClientSocket->m_socket = accept(pServerSocket->m_ServerSocket, (struct sockaddr*)&clientaddr, &namelen);

				//���յ���ͻ�������
				if (pClientSocket->m_socket)
				{
					strcpy(pClientSocket->m_RemoteHost, inet_ntoa(clientaddr.sin_addr));
					//pClientSocket->m_RemoteHost = inet_ntoa(clientaddr.sin_addr);
					pClientSocket->m_RemotePort = ntohs(clientaddr.sin_port);

					//�򿪿ͻ���ʵ�������߳�,����ɹ��򽫽�����뵽�ͻ��˴洢�б�,�������ͻ��˽��������¼�
					if(pClientSocket->Open(pServerSocket))
					{
						//��ӵ������ӿͻ����б���
						ActiveClientList.push_back(pClientSocket);

						//������ͻ��˽��������¼�
						if (pServerSocket->OnClientConnect)
						{
							//״̬����ʾ�����Ҫ��������ͻ���
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

void CTCPServerSocket::RemoteClient(void* pClient)			//��ĳ���ͻ����Ƴ������ӿͻ��˴洢�б�
{
	CTCPCustomSocket* ptempClient = (CTCPCustomSocket*)pClient;

	beginpos = ActiveClientList.begin();
	int ClientNum = ActiveClientList.size();				//��ȡ�����ӿͻ��˴洢�б�Ĵ�С

	for (int i = 0; i<ClientNum; i++)						//������������ָ�벻�ڴ洢�б��У��򲻻����κδ���
	{
		if (ActiveClientList[i] == ptempClient)
		{
			//ptempClient->Close();							//�ڵ��øú���֮ǰ���Ѿ��ر���socket�������ڴ˲����ظ��ر�
			delete ptempClient;
			ActiveClientList[i] = NULL;
			ActiveClientList.erase(beginpos+i);				//�˴�һ��Ҫɾ����������close�л��ظ��ر�socket��delete
			break;
		}
	}
}

int CTCPServerSocket::Open(void* pOwner, const char* ServerIPAddr, int serverPort)
{
	strcpy(m_LocalIPAddr, ServerIPAddr);			//��������IP��ַ
	m_LocalPort = serverPort;						//�������Ķ˿ں�
	m_pOwner = pOwner;

	WSADATA wsa;

	//��ʼ��socket��Դ
	if (WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{
		return -1;			//��ʼ��socket��Դʧ��
	}

	//���������׽���
	if ((m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		return -2;			//�����׽��ִ���ʧ��
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory((char*)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_LocalPort);
	serverAddr.sin_addr.s_addr = inet_addr(m_LocalIPAddr);

	//�󶨼����׽���
	if (bind(m_ServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0)
	{
		return -3;				//�󶨼����׽���ʧ��
	}

	//�����׽��ֿ�ʼ����
	if (listen(m_ServerSocket, 8)!=0)
	{
		return -4;
	}

	//���ü����׽���ͨѶģʽΪ�첽ģʽ
	DWORD u1 = 1;
	ioctlsocket(m_ServerSocket, FIONBIO, &u1);

	ResetEvent(m_exitServerThreadEvent);
	//����ͨѶ�̣߳����߳���ȴ��ͻ��˽���
	m_serverThreadHandle = CreateThread(NULL, 0, TCPServerFunc, this, 0, NULL);
	if (m_serverThreadHandle == NULL)
	{
		closesocket(m_ServerSocket);
		return -5;
	}
	return 1;
}

//�ر�TCP����
bool CTCPServerSocket::Close()
{
	//���������߳�
	SetEvent(m_exitServerThreadEvent);

	if (WaitForSingleObject(/*m_exitServerThreadEvent*/m_serverThreadHandle, 1000) == WAIT_TIMEOUT)
	{
		//ǿ�ƹر��߳�
		TerminateThread(m_serverThreadHandle, 0);
	}

	m_serverThreadHandle = NULL;

	beginpos = ActiveClientList.begin();
	int ClientNum = ActiveClientList.size();

	//�ر������пͻ��˵�����				//�ô��Ժ��ö���ʵ��
	for (int i = ClientNum-1; i>=0; i--)
	{
// 		(CTCPCustomSocket*)ActiveClientList[ActiveClientListPos]->Close();
// 		delete (CTCPCustomSocket*)ActiveClientList[ActiveClientListPos];
		CTCPCustomSocket* tempCustom = (CTCPCustomSocket*)ActiveClientList[i];
		tempCustom->Close();
		delete tempCustom;
		ActiveClientList.erase(beginpos+i);	
	}
	
	//�ر�socket���ͷ���Դ
	int err = closesocket(m_ServerSocket);
	if (err == SOCKET_ERROR)
	{
		return false;
	}
	
	//�ͷ�socket��Դ
	WSACleanup();
	return true;
}

//��ĳ���ͻ��˷�������
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
