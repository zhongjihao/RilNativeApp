/*************************************************************************
    > File Name: CRoomServer.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Fri 09 Aug 2017 03:34:28 PM CST
 ************************************************************************/

#ifndef ROOM_SERVER_H
#define ROOM_SERVER_H

#include "./TcpSocketSink.h"
#include "./TCPSocketEnging.h"
#include "./IClientUserManager.h"
#include "./CTableFrame.h"

//游戏房间服务器
class CRoomServer : public TcpSocketSink
{
private:
	bool                  m_running;
	int                   m_serverId;         //房间号
	int                   m_serverType;       //房间类型
	TCPSocketEnging*      m_pSocketEnging;
	IClientUserManager*   m_pUserManager;
	CTableFrame*          m_TableFrame;       //桌子框架

private:
	CRoomServer(const CRoomServer&);
	CRoomServer& operator=(const CRoomServer&);

public:
	CRoomServer(int serverId,int serverType);
	~CRoomServer();
	//消息处理
    bool OnEventTCPSocketRead(int fd,CMD_Command Command, void* pData, unsigned short wDataSize);

private:
	//登录消息处理
	bool OnSocketMainLogon(int fd,CMD_Command Command, void* pData, unsigned short wDataSize);
	//用户消息处理
	bool OnSocketMainUser(int fd,CMD_Command Command, void* pData, unsigned short wDataSize);
	//心跳消息处理
	bool OnSocketMainHeartBeat(int fd,CMD_Command Command,void* pData,unsigned short wDataSize);
private:
	//线程函数
	void run();
	static void* threadFunc(void* d);
	//心跳包处理线程函数
	void heartBeatRun();
	static void* heartBeatFunc(void* d);
	//设置定时器
	static void setTimer(int seconds, int mseconds);
public:
	//启动房间服务器
	bool StartRoomServer(const char* serIp,int port);
	//退出房间服务器
	void ExitRoomServer();
};

#endif

