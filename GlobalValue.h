#pragma once

#include <WinSock.h>
#pragma comment(lib, "Ws2_32.lib")
#include <vector>

using namespace std;

extern vector<void*>	ActiveClientList;				//�洢�����ӵĿͻ���ָ��
extern vector<void*>::iterator beginpos;				//ָ�������ӿͻ��˴洢�б����ʼλ��
