
#include "TcpClient.h"
#include <iostream>


TCPClient::TCPClient()
{
    memset( m_cbRecvBuf, 0, sizeof(m_cbRecvBuf) );
    m_nRecvSize = 0;

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
        std::cout << "create socket fail!" << std::endl;
        return;
    }

    mServerAddr.sin_family      = AF_INET;
    mServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    mServerAddr.sin_port        = htons((u_short)SERVER_PORT);

    if ( ::connect(mServerSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr)))
    {
        ::closesocket(mServerSocket);
        std::cout << "connect server fail!" << std::endl;
        return;
    }
}

TCPClient::~TCPClient()
{
    ::closesocket(mServerSocket);
}

void TCPClient::run()
{
    int nCount = 0;
    for (;;)
    {

        int nRecvSize = ::recv(mServerSocket,
            m_cbRecvBuf+m_nRecvSize,
            sizeof(m_cbRecvBuf)-m_nRecvSize, 0);
        if (nRecvSize <= 0)
        {
            std::cout << "server close connection!" << std::endl;
            break;
        }

        // 保存已经接收数据的大小
        m_nRecvSize += nRecvSize;

        // 接收到的数据够不够一个包头的长度
        while (m_nRecvSize >= sizeof(NetPacketHeader))//已经收到一个完整的包，如果没用收到一个完整的包，此处循环不执行，继续下一轮循环
        {
            // recv define nums NetPacket, close
            if (nCount >= NET_PACKET_NUMS)
            {
                ::closesocket(mServerSocket);
                break;
            }

            // read package head
            NetPacketHeader* pHead = (NetPacketHeader*) (m_cbRecvBuf);
            const unsigned short nPacketSize = pHead->wDataSize;

            // 判断是否已接收到足够一个完整包的数据
            if (m_nRecvSize < nPacketSize)
            {
                // 还不够拼凑出一个完整包
                break;
            }

            // 拷贝到数据缓存
            CopyMemory(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

            // 从接收缓存移除
            MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
            m_nRecvSize -= nPacketSize;

            // 解密数据，以下省略一万字
            // ...

            // 分派数据包，让应用层进行逻辑处理
            pHead = (NetPacketHeader*) (m_cbDataBuf);
            const unsigned short nDataSize = nPacketSize - (unsigned short)sizeof(NetPacketHeader);
            OnNetMessage(pHead->wOpcode, m_cbDataBuf+sizeof(NetPacketHeader), nDataSize);

            ++nCount;
        }
    }

    std::cout << "recv NetPacket_T nums: " << nCount << std::endl;

    std::cout << "disconnect with server!" << std::endl;
}

bool TCPClient::OnNetMessage( const unsigned short& nOpcode,
                             const char* pDataBuffer, unsigned short nDataSize )
{
    switch (nOpcode)
    {
    case NET_TEST1:
        {
            NetPacket_Test1* pMsg = (NetPacket_Test1*) pDataBuffer;
            return OnNetPacket(pMsg);
        }
        break;

    default:
        {
            std::cout << "no package recv:" << nOpcode << std::endl;
            return false;
        }
        break;
    }
}

bool TCPClient::OnNetPacket( NetPacket_Test1* pMsg )
{
    std::cout << "index：" << pMsg->nIndex << "  string：" << pMsg->arrMessage <<" name:"<<pMsg->name<<" sex:"<<pMsg->sex<<" age:"<<pMsg->age<< std::endl;
    return true;
}