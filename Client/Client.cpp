#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
#include<WinSock2.h>
#include<Windows.h>
#else 
#include<unistd.h>
#include<arpa/inet.h>	
#define SOCKET int 
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#endif
#include <iostream>
#include<thread>


#pragma comment(lib,"ws2_32.lib")
using namespace std;

int main()
{

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Init error\n";
        return 0;
    }
    if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wHighVersion) != 2)
    {
        cout << "version error\n";
        WSACleanup();
        return 0;
    }
#endif
    sockaddr_in _sin{};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(6000);
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else 
    _sin.sin_addr.s_addr = inet_addr("192.168.134.1");
#endif
    SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);

    if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr)))
    {
        cout << "connect error\n";
        exit(0);
    }

    while (true)
    {
        char reBuf[1024] = {}, sendBuf[1024] = { "client is over\n" };
        //cin >> sendBuf;
        int nSend = send(_sock, sendBuf, 1024, 0);
        if (nSend < 0) {
            cout << "send error\n";
            return 0;
        }
        int nRecv = recv(_sock, reBuf, 1024, 0);
        if (nRecv >= 0)
            cout << "sever:" << reBuf << endl;
        else {
            cout << "recv error\n";
            return 0;
        }
    }
#ifdef _WIN32
    closesocket(_sock);
    WSACleanup();
#else 
    close(_sock);
#endif

    return 0;
}




