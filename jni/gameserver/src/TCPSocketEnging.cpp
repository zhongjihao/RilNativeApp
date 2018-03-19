/*************************************************************************
    > File Name: TCPSocketEnging.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2017年06月22日 星期四 14时16分31秒
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-TCPSocketEnging"

#include "../include/TCPSocketEnging.h"
#include <signal.h>
#include <utils/Log.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/sockets.h>

pthread_mutex_t TCPSocketEnging::s_writeMutex = PTHREAD_MUTEX_INITIALIZER;

TCPSocketEnging::TCPSocketEnging() : m_serFd(-1),m_wRecvSize(0)
{
    memset(m_cbRecvBuf,0, sizeof(m_cbRecvBuf));
	m_ClientFds.clear();
//	signal(SIGALRM, OnTimer); 
}

bool TCPSocketEnging::initSocket(const char* serIP,int port)
{
   // m_serFd = socket(AF_INET,SOCK_STREAM,6);
    m_serFd = socket_loopback_server(port,SOCK_STREAM);
    if(m_serFd == -1)
    {
        ALOGE("%s: ====Game====建立socket失败========%s",__FUNCTION__,strerror(errno));
        return false;
    }
    ALOGD("%s: ===Game=====建立socket成功=======serIP: %s, port: %d",__FUNCTION__,serIP,port);
	/*
    struct sockaddr_in dr;
    bzero(&dr,sizeof(dr));
    dr.sin_family = AF_INET;
    dr.sin_port = htons(port);
    inet_aton(serIP,&dr.sin_addr);
    int ret = bind(m_serFd,(struct sockaddr*)&dr,sizeof(dr));
    if(ret == -1)
    {
        ALOGE("%s: ====Game======bind连接失败=======%s",__FUNCTION__,strerror(errno));
        close(m_serFd);
        m_serFd = -1;
        return false;
    }
    ALOGD("%s: ====Game===bind连接成功===========ip: %s,port: %d",__FUNCTION__,serIP,port);
	*/
    return true;
}

void TCPSocketEnging::OnAccept()
{
	int cliFd = ::accept(m_serFd,NULL,NULL);
	if(cliFd == -1)
	{
		ALOGE("%s: ===Game=====accept error: %s==========",__FUNCTION__,strerror(errno));
		close(m_serFd);
		m_serFd = -1;
		return;
	}

	ALOGD("%s:===Game=====accept ok!========cliFd: %d",__FUNCTION__,cliFd);
	m_ClientFds.push_back(cliFd); 
}


int TCPSocketEnging::GetSocketFd()
{
	return m_serFd;
}

std::list<int>& TCPSocketEnging::GetClientFds()
{
	return m_ClientFds;
}

int TCPSocketEnging::OnSendCompleted(const void *data, size_t dataSize, int fd) 
{
	int ret;
	pthread_mutex_t * writeMutexHook = &s_writeMutex;
    
	if (fd < 0) {
		return -1;
	}

	if (dataSize > SOCKET_BUFFER) {
		ALOGE("%s: ====Game====packet larger than %u (%u)",__FUNCTION__,SOCKET_BUFFER, (unsigned int )dataSize);
		return -1 ;
	}

    pthread_mutex_lock(writeMutexHook);

	ret = blockingWrite(fd, data, dataSize);

	if (ret < 0) {
		pthread_mutex_unlock(writeMutexHook);
		return ret;
	}

	pthread_mutex_unlock(writeMutexHook);

	return ret;
}

int TCPSocketEnging::blockingWrite(int fd, const void *buffer, size_t len) 
{
	size_t writeOffset = 0;
	const uint8_t *toWrite;

	toWrite = (const uint8_t *)buffer;

	while(writeOffset < len) 
	{
		ssize_t written;
		do {
			written = write(fd, toWrite + writeOffset,len - writeOffset);
		} while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));
		
		if (written >= 0) {
			writeOffset += written;
		} else { // written < 0
			ALOGE ("%s: ========Game=====unexpected error on write errno:%d", __FUNCTION__,errno);
		    close(fd);
			return -1;
		}
	}
	
	ALOGD("%s: ====Game======Request bytes written:%d", __FUNCTION__,writeOffset);
	return writeOffset;
}

int TCPSocketEnging::OnRecvCompleted(int fd,TcpSocketSink* m_pSocketSink)
{
	//接收请求数据长度
	uint32_t dataLength = 0;
    int ret = 0;
	ret = recv(fd,&dataLength,sizeof(dataLength),MSG_WAITALL);
	if(ret <=0)
	{
		ALOGE("%s:====Game===recv数据==%s",__FUNCTION__,(ret == -1) ? strerror(errno) : "有客户退出");
		return -1;
	}

    //接收数据
	m_wRecvSize = ntohl(dataLength);
	ALOGD("%s: ===Game====请求数据长度m_wRecvSize : %d,  dataLength: %d",__FUNCTION__,m_wRecvSize,dataLength);
	memset(m_cbRecvBuf,0,sizeof(m_cbRecvBuf));
	ret = recv(fd,m_cbRecvBuf,m_wRecvSize,MSG_WAITALL);
    if(ret == -1 || ret == 0)
    {
        ALOGE("%s: ===Game===recv数据===%s",__FUNCTION__,(ret == -1) ? strerror(errno) : "对端socket关闭");
        return -1;
    }

    //接收完成
    unsigned char cbBuffer[SOCKET_BUFFER];
    memset(cbBuffer,0,SOCKET_BUFFER);
    CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf;

    //处理数据
    try
    {
        while(m_wRecvSize >= sizeof(CMD_Head))
        {
            //效验数据
            unsigned short wPacketSize = pHead->CmdInfo.wPacketSize;
            if (wPacketSize > SOCKET_BUFFER) throw "数据包超长";
            if (wPacketSize < sizeof(CMD_Head)) throw "数据包非法";
            if (pHead->CmdInfo.cbVersion != SOCKET_VER) throw "数据包版本错误";
            if (m_wRecvSize < wPacketSize) break;

            memcpy(cbBuffer,m_cbRecvBuf,wPacketSize);

            //解释数据
            unsigned short wDataSize = wPacketSize - sizeof(CMD_Head);
            void * pDataBuffer = cbBuffer + sizeof(CMD_Head);
            CMD_Command Command = ((CMD_Head *)cbBuffer)->CommandInfo;
            ALOGD("===Game====1====主命令: %d   ,子命令: %d,  wPacketSize: %d, m_wRecvSize: %d",Command.wMainCmdID,Command.wSubCmdID,wPacketSize,m_wRecvSize);

            //删除缓存数据
            m_wRecvSize -= wPacketSize;
            memcpy(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);

            ALOGD("===Game====2===主命令: %d   ,子命令: %d, wPacketSize: %d, m_wRecvSize: %d, wDataSize: %d",Command.wMainCmdID,Command.wSubCmdID,wPacketSize,m_wRecvSize,wDataSize);
            //消息处理
            m_pSocketSink->OnEventTCPSocketRead(fd,Command,pDataBuffer,wDataSize);
        }
    }
    catch (const char* msg)
    {
        ALOGE("%s: ===Game====OnRecvCompleted===异常: %s",__FUNCTION__,msg);
		return -1;
    }
    catch(...)
    {
		return -1;
    }
    return 0;
}

int TCPSocketEnging::OnSocketNotifyRead(int fd,TcpSocketSink* m_pSocketSink)
{
    return OnRecvCompleted(fd,m_pSocketSink);
}

int TCPSocketEnging::SendData(short wMainCmdID, short wSubCmdID,void* pData, size_t wDataSize,int fd)
{
    if(wDataSize + sizeof(CMD_Head) > SOCKET_BUFFER)
    {
        ALOGE("%s: ==Game=====SendData===发送数据太大======",__FUNCTION__);
        return 0;
    }
    //构造数据
    char cbDataBuffer[SOCKET_BUFFER];
    memset(cbDataBuffer,0,SOCKET_BUFFER);
    CMD_Head  pHead;
    pHead.CmdInfo.cbVersion = SOCKET_VER;
    pHead.CmdInfo.cbCheckCode = 0;
    pHead.CmdInfo.wPacketSize = sizeof(CMD_Head)+wDataSize;
    pHead.CommandInfo.wMainCmdID = wMainCmdID;
    pHead.CommandInfo.wSubCmdID = wSubCmdID;

    memcpy(cbDataBuffer,&pHead, sizeof(CMD_Head));
    if(wDataSize > 0)
    {
        if(pData != NULL)
        {
            memcpy(cbDataBuffer + sizeof(CMD_Head),pData,wDataSize);
        }
    }
    
    ALOGE("%s: ==Game=====SendData===发送数据=====wMainCmdID: %d,  wSubCmdID: %d",__FUNCTION__,pHead.CommandInfo.wMainCmdID,pHead.CommandInfo.wSubCmdID);
    //发送数据
    return OnSendCompleted(cbDataBuffer, sizeof(CMD_Head)+wDataSize,fd);
}

/*
void TCPSocketEnging::OnTimer(int sig)  
{
	if(SIGALRM == sig)
	{
		//向客户端发送心跳包
        LOGD("%s: ====心跳包======",__FUNCTION__);
		for(std::list<int>::iterator iter = m_ClientFds.begin();iter != m_ClientFds.end();iter++)
		{
			SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET, NULL, 0,*iter);
		}
		alarm(60); 
	}
	return;  
}
*/

void TCPSocketEnging::Close()
{
    if(m_serFd != -1)
    {
        close(m_serFd);
        m_serFd = -1;
    }
}

