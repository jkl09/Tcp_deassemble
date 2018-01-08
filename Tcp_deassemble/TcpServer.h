#pragma once

#include <Windows.h>
#include "common.h"


class TCPServer
{
public:
    TCPServer();
    virtual ~TCPServer();

public:
    void run();

    void acceptClient();

    void closeClient();

    bool SendData(unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize);

private:
    SOCKET      mServerSocket;  ///< 服务器套接字句柄
    sockaddr_in mServerAddr;    ///< 服务器地址

    SOCKET      mAcceptSocket;  ///< 接受的客户端套接字句柄
    sockaddr_in mAcceptAddr;    ///< 接收的客户端地址

    char        m_cbSendBuf[NET_PACKET_SIZE];
};
