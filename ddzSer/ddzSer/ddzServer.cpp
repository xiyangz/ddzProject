// ddzSer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include <Ws2tcpip.h>
using namespace std;

struct sockData
{
	TCHAR ip[16];
	UINT port;
	SOCKET sock;
	HANDLE runHandle;
};

DWORD WINAPI loginProc(LPVOID lpv)
{
	sockData* sockdata = (sockData*)lpv;
	sLoginSock lsock(sockdata->sock,sockdata->ip,sockdata->port);
	lsock.OnReceive();
	return 0;
}

DWORD WINAPI runProc(LPVOID lpv)
{
	sockData* sockdata = (sockData*)lpv;
	if (sockdata->runHandle == NULL)
		Sleep(100);
	sRunSock rsock(sockdata->sock, sockdata->ip, sockdata->port,sockdata->runHandle);
	rsock.OnReceive();
	return 0;
}

DWORD WINAPI listenLoginProc(LPVOID lpv)
{
	SOCKET lsock = *(SOCKET*)lpv;
	sockaddr_in addrClient;
	int len = sizeof(sockaddr);
	SOCKET lssock = NULL;
	sockData sockData;
	TCHAR ip[16];
	while (true)
	{
		lssock = accept(lsock, (sockaddr*)&addrClient, &len);
		if (lssock != NULL)
		{//连接成功
			sockData.sock = lssock;
			sockData.port = ntohs(addrClient.sin_port);
			InetNtopW(AF_INET, &addrClient.sin_addr, sockData.ip, sizeof(sockData.ip));
			HANDLE hThread = CreateThread(NULL, 0, loginProc, &sockData, 0, NULL);
		}
		Sleep(30);
	}
	return 0;
}

DWORD WINAPI listenRunProc(LPVOID lpv)
{
	SOCKET ssock = *(SOCKET*)lpv;
	sockaddr_in addrClient;
	int len = sizeof(sockaddr);
	SOCKET rssock = NULL;
	sockData sockData;
	sockData.runHandle = NULL;
	TCHAR ip[16];
	while (true)
	{
		rssock = accept(ssock, (sockaddr*)&addrClient, &len);
		if (rssock != NULL)
		{//连接成功
			sockData.sock = rssock;
			sockData.port = ntohs(addrClient.sin_port);
			InetNtopW(AF_INET, &addrClient.sin_addr, sockData.ip, sizeof(sockData.ip));
			sockData.runHandle = CreateThread(NULL, 0, runProc, &sockData, 0, NULL);
			Sleep(30);
		}
	}
	return 0;
}

int main()
{
	WSADATA wd;
	int err = WSAStartup(MAKEWORD(2, 2), &wd);

	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		cout << "WSAStartup failed with error: " << err << endl;
		return 0;
	}

	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		cout << "没有找到Winsock.dll的有效版" << endl;
		WSACleanup();
		return 1;
	}
	else
		cout << "The Winsock 2.2 dll可以使用" << endl;

	
	//登录侦听套接字
	SOCKET lsock = INVALID_SOCKET;
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock == INVALID_SOCKET)
		cout << "lsocket调用socket函数失败,失败代码:" << WSAGetLastError() << endl;
	else
		cout << "lsocket调用socket函数成功" << endl;
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8881);
	int iResult = 0;
	iResult = bind(lsock, (sockaddr*)&addr, sizeof(sockaddr));
	if (iResult == SOCKET_ERROR) {
		cout << "绑定lsock失败,失败代码:" << WSAGetLastError() << endl;
		closesocket(lsock);
		WSACleanup();
		return 0;
	}
	else
		cout << "绑定lsock成功" << endl;
	listen(lsock, 5);


	//游戏侦听套接字
	SOCKET ssock = INVALID_SOCKET;
	ssock = socket(AF_INET, SOCK_STREAM, 0);
	if (ssock == INVALID_SOCKET)
		cout << "ssocket调用socket函数失败,失败代码:" << WSAGetLastError() << endl;
	else
		cout << "ssocket调用socket函数成功" << endl;
	sockaddr_in addr2;
	addr2.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr2.sin_family = AF_INET;
	addr2.sin_port = htons(8882);
	int iResult2 = 0;
	iResult2 = bind(ssock, (sockaddr*)&addr2, sizeof(sockaddr));
	if (iResult2 == SOCKET_ERROR) {
		cout << "绑定ssock失败,失败代码:" << WSAGetLastError() << endl;
		closesocket(ssock);
		WSACleanup();
		return 0;
	}
	else
		cout << "绑定ssock成功" << endl;
	listen(ssock, 5);
	

	HANDLE hThread1 = CreateThread(NULL, 0, listenLoginProc, &lsock, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, listenRunProc, &ssock, 0, NULL);

	
	char ch = 'N';
	do
	{
		cout << "退出服务器吗？(Y/N)" << endl;
		cin>>ch;
	} while ('y' != ch && 'Y' != ch);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
