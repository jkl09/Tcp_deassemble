
#include "TcpServer.h"
#include <iostream>

TCPServer::TCPServer()
 :mServerSocket(INVALID_SOCKET)
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        std::cout << "WSAStartup fail!, error: " << GetLastError() << std::endl;
        return;
    }

    mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (mServerSocket == INVALID_SOCKET)
    {
        std::cout << "create socket fail!, error: " <<GetLastError() <<std::endl;
        return;
    }


    mServerAddr.sin_family      = AF_INET;
    mServerAddr.sin_addr.s_addr = INADDR_ANY;
    mServerAddr.sin_port        = htons((u_short)SERVER_PORT);


    if ( ::bind(mServerSocket, (sockaddr*)&mServerAddr, sizeof(mServerAddr)) == SOCKET_ERROR)
    {
        std::cout << "bind fail!" << std::endl;
        return;
    }

    if ( ::listen(mServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "listen fail!" << std::endl;
        return;
    }

    std::cout << "start tcp server success!" << std::endl;
}

TCPServer::~TCPServer()
{
    ::closesocket(mServerSocket);
    std::cout << "close tcp server success!" << std::endl;
}

void TCPServer::run()
{

    acceptClient();

    int nCount = 0;
    for (;;)
    {
        if (mAcceptSocket == INVALID_SOCKET)
        {
            std::cout << "client close connection！" << std::endl;
            break;
        }

        // send data
        NetPacket_Test1 msg;
        msg.nIndex = nCount;
        msg.age=23;
        strncpy(msg.arrMessage, "BeiJing", sizeof(msg.arrMessage) );
        strncpy(msg.name, "Baidu", sizeof(msg.name) );
        strncpy(msg.sex, "Male", sizeof(msg.sex) );

        bool bRet = SendData(NET_TEST1, (const char*)&msg, sizeof(msg));//强制类型转换为字符串类型
        if (bRet)
        {
            std::cout << "send success！" << std::endl;
        }
        else
        {
            std::cout << "send fail！" << std::endl;
            break;
        }
        //sleep 2s, show it
        Sleep(1000 * 2);
        ++nCount;
        if (nCount >= NET_PACKET_NUMS)
        {
            std::cout << "server already send defined nums NetPacket_T, return" << std::endl;
            break;
        }
    }
    std::cout << "send NetPacket_T nums: " << nCount << std::endl;
}

void TCPServer::closeClient()
{

    if (mAcceptSocket == INVALID_SOCKET) return;


    ::closesocket(mAcceptSocket);
    std::cout << "client socket is closed!" << std::endl;
}

void TCPServer::acceptClient()
{
    // accept is block
    int nAcceptAddrLen = sizeof(mAcceptAddr);
    mAcceptSocket = ::accept(mServerSocket, (struct sockaddr*)&mAcceptAddr, &nAcceptAddrLen);
    std::cout << "accept client IP:" << inet_ntoa(mAcceptAddr.sin_addr) << std::endl;
}

bool TCPServer::SendData( unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize )
{
    NetPacketHeader* pHead = (NetPacketHeader*) m_cbSendBuf;
    pHead->wOpcode = nOpcode;

    // 数据封包
    if ( (nDataSize > 0) && (pDataBuffer != 0) )
    {
        CopyMemory(pHead+1, pDataBuffer, nDataSize);
    }


    const unsigned short nSendSize = nDataSize + sizeof(NetPacketHeader);//包的大小事发送数据的大小加上包头大小
    pHead->wDataSize = nSendSize;//包大小
    int ret = ::send(mAcceptSocket, m_cbSendBuf, nSendSize, 0);
    return (ret > 0) ? true : false;
}