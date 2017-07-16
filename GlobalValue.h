#pragma once

#include <WinSock.h>
#pragma comment(lib, "Ws2_32.lib")
#include <vector>

using namespace std;

extern vector<void*>	ActiveClientList;				//存储已连接的客户端指针
extern vector<void*>::iterator beginpos;				//指向已连接客户端存储列表的起始位置
