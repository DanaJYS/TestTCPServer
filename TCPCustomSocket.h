#pragma once
#include "TCPServerSocket.h"
#include "GlobalValue.h"

class CTCPCustomSocket
{
public:
	CTCPServerSocket*		m_TCPServerSocket;				//����TCP����˼���Socket
	char					m_RemoteHost[50];				//�ͻ���IP��ַ
	int						m_RemotePort;					//�ͻ��˶˿ں�

	SOCKET					m_socket;						//ͨѶsocket���

private:
	HANDLE					m_exitThreadEvent;				//ͨѶ�߳��˳��¼����
	HANDLE					m_tcpThreadHandle;				//ͨѶ�߳̾��

public:
	CTCPCustomSocket();							//��ʼ����Ա����
	~CTCPCustomSocket();						//

	bool		Open(CTCPServerSocket* pTCPServer);			//����ͨѶ�߳�
	bool		Close();									//�ر�ͨѶ�̣߳��ر�socket
	bool		SendData(const char* buf, DWORD dwBufLen);	//��ͻ��˷�������

	static DWORD WINAPI TCPCustomFunc(PVOID lparam);				//ͨѶ�̣߳�ֻҪ���ڶ�ȡ����
};