#pragma once

#include "GlobalValue.h"


//����ͻ������ӽ����¼�
typedef void(CALLBACK* ONCLIENTCONNECT) (void* pOwner, void* pSocket);			//�ڶ����������ڴ���ĳ���ͻ��˵�ʵ��
//����ͻ���socket�ر��¼�
typedef void(CALLBACK* ONCLIENTCLOSE) (void* pOwner, void* pSocket);
//����ͻ���socket�����¼�
typedef void(CALLBACK* ONCLIENTERROR) (void* pOwner, void* pSocket, int nErrorCode);
//��������socket�����¼�
typedef void(CALLBACK* ONSERVERERROR) (void* pOwner, void* pSocket, int nErrorCode);
//����ͻ������ݽ����¼�
typedef void(CALLBACK* ONCLIENTREAD) (void* pOwner, void* pSocket, const char* buf, DWORD dwBufLen);

class CTCPServerSocket
{
public:
	char				m_LocalIPAddr[50];					//���÷�����IP��ַ
	int					m_LocalPort;						//���÷������˿ں�

	void*				m_pOwner;							//��������

// 	CTCPCustomSocket*	ActiveClientList[10];				//�洢�����ӵĿͻ���ָ��
// 	int					ActiveClientListPos;				//�����ӿͻ���ָ������ĵ�ǰ����

private:
	SOCKET				m_ServerSocket;						//TCP�������socket
	HANDLE				m_serverThreadHandle;				//�����߳̾��
	HANDLE				m_exitServerThreadEvent;			//�����߳��˳��¼����

public:
	ONCLIENTCONNECT		OnClientConnect;
	ONCLIENTCLOSE		OnClientClose;
	ONCLIENTERROR		OnClientError;
	ONCLIENTREAD		OnClientRead;
	ONSERVERERROR		OnServerError;

public:
	CTCPServerSocket();						//
	~CTCPServerSocket();

	//��ʼ��socket��Դ�� ����tcp�����׽��֣� �������׽����뱾�ص�ַ�󶨣� ��ʼ������ ���������̵߳ȴ��ͻ��˽���
	int	Open(void* pOwner, const char* ServerIPAddr, int serverPort);				
	//�رռ����̣߳� �ر������пͻ��˵����ӣ� ��տͻ����б� �ͷ�socket��Դ
	bool Close();
	//����ĳ���ͻ��˵�SendData��������
	bool SendData(void* pCustomSocket, const char* buf, DWORD dwBufLen);
	//ɾ���ͻ��˶���
	void RemoteClient(void* pClient);

	//�����̺߳����������ͻ����׽����¼����пͻ��˽����������ͻ����б���������Ӧ�Ŀͻ����߳�
	static DWORD WINAPI TCPServerFunc(PVOID lparam);
};

