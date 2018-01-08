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
    SOCKET      mServerSocket;  ///< �������׽��־��
    sockaddr_in mServerAddr;    ///< ��������ַ

    SOCKET      mAcceptSocket;  ///< ���ܵĿͻ����׽��־��
    sockaddr_in mAcceptAddr;    ///< ���յĿͻ��˵�ַ

    char        m_cbSendBuf[NET_PACKET_SIZE];
};
