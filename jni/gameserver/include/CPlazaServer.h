/*************************************************************************
    > File Name: CPlazaServer.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Fri 30 Jun 2017 03:34:28 PM CST
 ************************************************************************/

#ifndef PLAZA_SERVER_H
#define PLAZA_SERVER_H

#include "./TcpSocketSink.h"
#include "./TCPSocketEnging.h"
#include "./IClientUserManager.h"

//大厅服务器
class CPlazaServer : public TcpSocketSink
{
private:
	bool m_running;
	char m_gameConfigIniPath[256];
	static CPlazaServer* m_pInstance;
	TCPSocketEnging* m_pSocketEnging;
	IClientUserManager* m_pUserManager;
	static unsigned long m_UserIdIndex;
private:
	CPlazaServer();
	CPlazaServer(const CPlazaServer&);
	CPlazaServer& operator=(const CPlazaServer&);
	~CPlazaServer();
	//消息处理
    bool OnEventTCPSocketRead(int fd,CMD_Command Command, void* pData, unsigned short wDataSize);
	//登录消息处理
	bool OnSocketMainLogon(int fd,CMD_Command Command, void* pData, unsigned short wDataSize);
private:
	//线程函数
	void run();
	static void* threadFunc(void* d);
public:
	static CPlazaServer* getInstance();
	//启动大厅服务器
	bool StartPlazaServer(const char* serIp,int port);
	//退出大厅服务器
	void ExitPlazaServer();
	//保存游戏配置文件路径
	void SetGameConfigIni(const char* path);
	IClientUserManager* getPlazaUserManagerPtr()const;
};

#endif

