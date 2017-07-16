#include "TCPCustomSocket.h"

//���캯����Ҫ���ڳ�ʼ����ĳ�Ա����
CTCPCustomSocket::CTCPCustomSocket()		
{
	m_TCPServerSocket = NULL;				//����TCP����˼���socket
	memset(m_RemoteHost, 0, 50);			//Զ������IP��ַ
	m_RemotePort = 0;						//Զ�������˿ں�
	m_socket = 0;							//ͨѶsocket���
}

//��������
CTCPCustomSocket::~CTCPCustomSocket()
{

}

//Open�������ڴ�����ĳ��socket��ͨѶ�̣߳�������socketΪ�첽ģʽ,��ȡ�������ĵ�ַ
bool CTCPCustomSocket::Open(CTCPServerSocket* pTCPServer)
{
	//�����߳��˳��¼����ڶ�������Ϊfalse���¼���Ӧ���Զ����ã���������������Ϊfalse���¼���ʼ��Ϊδ����
	m_exitThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//����ͨѶ�߳�
	m_tcpThreadHandle = CreateThread(NULL, 0, TCPCustomFunc, this, 0, NULL);
	//���ͨѶ�߳�δ�����ɹ�����رտͻ���socket���ͻ���socket�ڷ���˵ļ����߳��д���
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return false;
	}

	//����ͨѶģʽΪ�첽ģʽ
	DWORD u1 = 1;
	ioctlsocket(m_socket, FIONBIO, &u1);
	//��ȡ�������ʵ��ָ��
	m_TCPServerSocket = pTCPServer;
	return true;
}

//�ر�socket���ر��߳�
bool CTCPCustomSocket::Close()
{
	//����ͨѶ�߳̽����¼�
	SetEvent(m_exitThreadEvent);

	//�ȴ�1���ӣ�������߳�û���˳�����ǿ���˳�
	if (WaitForSingleObject(/*m_exitThreadEvent*/m_tcpThreadHandle, 1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_tcpThreadHandle, 0);
	}
	m_tcpThreadHandle = NULL;
	CloseHandle(m_exitThreadEvent);		//�ر��¼����

	//�ر�socket���ͷ���Դ
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

//��ͻ��˷�������
bool CTCPCustomSocket::SendData(const char* buf, DWORD dwBufLen)
{
	int nBytes = 0;					//һ�η��͵��ֽ���
	int nSendBytes = 0;				//��ǰ�ܹ����͵��ֽ���

	while(nSendBytes<dwBufLen)
	{
		nBytes = send(m_socket, buf+nSendBytes, dwBufLen-nSendBytes, 0);

		if (nBytes == SOCKET_ERROR)
		{
			int iErrorCode = WSAGetLastError();
			//����socket��error�¼�
			if (m_TCPServerSocket->OnClientError)
			{
				m_TCPServerSocket->OnClientError(m_TCPServerSocket->m_pOwner, this, iErrorCode);
			}
			//������������Ͽ������¼�
			if (m_TCPServerSocket->OnClientClose)
			{
				m_TCPServerSocket->OnClientClose(m_TCPServerSocket->m_pOwner, this);
			}

			//����Close()�������ر�socket�˳��߳�
			Close();
			//����ӿͻ����б����Ƴ�
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

//���ڼ��������ӵĿͻ���socketͨѶ�¼�����Ҫ���ڴӿͻ��˶�ȡ����
DWORD WINAPI CTCPCustomSocket::TCPCustomFunc(PVOID lparam)
{
	int ret;
	//��ȡCTCPCustomSocket��ʵ��ָ��
	CTCPCustomSocket* pSocket = (CTCPCustomSocket*)lparam;

	//������¼�����
	fd_set fdRead;
	TIMEVAL aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;

	while (true)
	{
		//�յ��߳��˳��¼��������߳�
		if (WaitForSingleObject(pSocket->m_exitThreadEvent, 0) == WAIT_OBJECT_0)			//�ڶ�����������Ϊ0������¼�δ����������ֱ�ӷ��ز��õȴ�
		{
			break;
		}

		//�ÿն��¼�����
		FD_ZERO(&fdRead);
		//��pSocket���ö��¼�
		FD_SET(pSocket->m_socket, &fdRead);
		//����select�������ж��Ƿ��ж��¼�����
		ret = select(0, &fdRead, NULL, NULL, &aTime);

		if (ret == SOCKET_ERROR)
		{
			//���������¼�
			if(pSocket->m_TCPServerSocket->OnClientError)
			{
				pSocket->m_TCPServerSocket->OnClientError(pSocket->m_TCPServerSocket->m_pOwner, pSocket, 1);		//״̬��ʾ������Ƴ�
			}
			//������ͻ������ӵ�socket�ر��¼�
			if (pSocket->m_TCPServerSocket->OnClientClose)
			{
				pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
			}
			//�رտͻ���socket,�����߳�,������������ӿͻ��˴洢�б����Ƴ�
			closesocket(pSocket->m_socket);
			pSocket->m_TCPServerSocket->RemoteClient(pSocket);			//�ͷ��ڴ棬�Ƴ��洢�б�
			break;
		}

		if (ret>0)
		{
			//�ж��Ƿ��Ƕ��¼�
			if (FD_ISSET(pSocket->m_socket, &fdRead))
			{
				char recvBuf[1024];
				int recvLen;
				ZeroMemory(recvBuf, 1024);
				recvLen = recv(pSocket->m_socket, recvBuf, 1024, 0);

				if (recvLen == SOCKET_ERROR)
				{
					int nErrorCode = WSAGetLastError();
					//������ͻ������ӵ�socket�����¼�
					if(pSocket->m_TCPServerSocket->OnClientError)
					{
						pSocket->m_TCPServerSocket->OnClientError(pSocket->m_TCPServerSocket->m_pOwner, pSocket, nErrorCode);
					}
					//������ͻ������ӵ�socket�ر��¼�
					if (pSocket->m_TCPServerSocket->OnClientClose)
					{
						pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
					}
					//�ر�socket
					closesocket(pSocket->m_socket);
					//�������ӿͻ����б����Ƴ��ͻ��ˣ����ǲ�����ÿ�ιر�socketʱ��Ӧ���Ƴ�socket��
					pSocket->m_TCPServerSocket->RemoteClient(pSocket);			
					//�����߳�
					break;
				}
				else if (recvLen == 0)			//��ʾ�����Ѿ����ݹر�
				{
					//�����ͻ��˶Ͽ������¼�
					if (pSocket->m_TCPServerSocket->OnClientClose)
					{
						pSocket->m_TCPServerSocket->OnClientClose(pSocket->m_TCPServerSocket->m_pOwner, pSocket);
					}
					//�ر�socket
					closesocket(pSocket->m_socket);
					//�������ӿͻ����б����Ƴ��ͻ��ˣ����ǲ�����ÿ�ιر�socketʱ��Ӧ���Ƴ�socket��
					pSocket->m_TCPServerSocket->RemoteClient(pSocket);
					
					//�����߳�
					break;
				}
				else
				{
					//������ͻ������ӵĶ��¼�
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

