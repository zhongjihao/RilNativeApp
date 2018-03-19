/*************************************************************************
    > File Name: TCPSocketEnging.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2017年06月22日 星期四 14时16分31秒
 ************************************************************************/

#ifndef CHATFRAME_TCPSOCKETENGING_H
#define CHATFRAME_TCPSOCKETENGING_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <list>
#include "GameData.h"
#include "TcpSocketSink.h"


class TCPSocketEnging
{
private:
    int m_serFd;
	std::list<int> m_ClientFds;
    unsigned int m_wRecvSize;
    unsigned char m_cbRecvBuf[SOCKET_BUFFER];
	static pthread_mutex_t s_writeMutex;
private:
    int  OnSendCompleted(const void* data, size_t wSenddata,int fd);
    int OnRecvCompleted(int fd,TcpSocketSink* m_pSocketSink);
//	static void OnTimer(int sig);
	static int blockingWrite(int fd, const void *buffer, size_t len);
public:
    TCPSocketEnging();
	int GetSocketFd();
	std::list<int>& GetClientFds();
    bool initSocket(const char* serIP,int port);//初始化socket
	void OnAccept();    //接收客户连接
    int OnSocketNotifyRead(int fd,TcpSocketSink* m_pSocketSink);   //接收数据
    int SendData(short wMainCmdID, short wSubCmdID,void* pData, size_t wDataSize,int fd);    //发送数据
    void Close();    //关闭Socket
};

#endif //CHATFRAME_TCPSOCKETENGING_H
