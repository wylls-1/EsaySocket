#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include <iostream>
#include<string>
#include<string.h>
#include<vector>
#include"mysever.hpp"

#pragma comment(lib,"ws2_32.lib")
using namespace std;
vector<SOCKET> g_clients;

int process(SOCKET _csock)
{
    char reBuf[1024] = {}, sendBuf[1024] = { "sever is working\n" };
    int nRecv = recv(_csock, reBuf, 1024, 0);
    if (nRecv <= 0)
    {
        cout << "recv error\n";
        return -1;
    }
    cout << "socket : " << _csock << "::" << reBuf << endl;
    //cin >> sendBuf;
    int nSend = send(_csock, sendBuf, 1024, 0);
    if (nSend <= 0) {
        cout << "send error\n";
        return -1;
    }
    return 0;
}

int main()
{

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
    SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in _sin{};

    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(6000);
    _sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
    {
        cout << "bind error\n";
        exit(0);
    }
    if (SOCKET_ERROR == listen(_sock, 5)) {
        cout << "listen error\n";
        exit(0);
    }

    //select socket
    while (true)
    {
        fd_set fdRead, fdWrite, fdExp;
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock, &fdRead);
        FD_SET(_sock, &fdWrite);
        FD_SET(_sock, &fdExp);

        for (auto i = 0; i < g_clients.size(); i++)
        {
            FD_SET(g_clients[i], &fdRead);
        }
        timeval t{ 0,0 };
        int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
        if (ret < 0) {
            cout << "select error\n";
            return 0;
        }
        if (FD_ISSET(_sock, &fdRead)) {
            FD_CLR(_sock, &fdRead);
            sockaddr_in clientAddr{};
            int naddr = sizeof(sockaddr);
            SOCKET _csock = INVALID_SOCKET;
            _csock = accept(_sock, (sockaddr*)&clientAddr, &naddr);
            if (_csock == INVALID_SOCKET)
            {
                cout << "accept error\n";
                exit(0);
            }
            cout << _csock << "\t" << inet_ntoa(clientAddr.sin_addr) << endl;
            g_clients.push_back(_csock);
            process(_csock);
        }
        for (int i = 0; i < fdRead.fd_count; i++)
        {
            if (-1 == process(g_clients[i]))
            {
                auto it = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[i]);
                if (it != g_clients.end())
                    g_clients.erase(it);
            }
        }


    }
    for (int i = 0; i < g_clients.size(); i++)
        closesocket(g_clients[i]);
    closesocket(_sock);
    WSACleanup();
    return 0;
}


