/*************************************************************************
    > File Name: CRoomServer.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Wed 09 Aug 2017 02:34:52 PM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CRoomServer"

#include "../include/CRoomServer.h"
#include "../include/CClientUserManager.h"
#include "../include/CSendPacketHelper.h"
#include "../include/CRecvPacketHelper.h"
#include "../include/CPlazaServer.h"
#include <pthread.h>
#include <time.h> 
#include <sys/time.h>
#include <list>
#include <assert.h>
#include <utils/Log.h>

CRoomServer::CRoomServer(int serverID,int serverType) : m_running(false),m_serverId(serverID),m_serverType(serverType)
{
	m_pSocketEnging = new TCPSocketEnging;
	m_pUserManager = new CClientUserManager;
	m_TableFrame = new CTableFrame;
}

CRoomServer::~CRoomServer()
{
	m_pSocketEnging->Close();
	delete m_pSocketEnging;
	delete m_TableFrame;
	delete m_pUserManager;
}

bool CRoomServer::StartRoomServer(const char* serIp,int port)
{
	m_pSocketEnging ->initSocket(serIp,port);
    if(!m_running)
	{
		ALOGD("%s: =====Game=======",__FUNCTION__);
		pthread_t tid1,tid2;
		pthread_create(&tid2,NULL,heartBeatFunc,this);
		int ret = pthread_create(&tid1,NULL,threadFunc,this);
		if(ret == 0 && tid1 != -1){
			m_running = true;
		}
	}
	return m_running;
}

void* CRoomServer::threadFunc(void *d)
{
	CRoomServer* th = static_cast<CRoomServer*>(d);
	th ->run();
	return NULL;
}

void* CRoomServer::heartBeatFunc(void *d)
{
	CRoomServer* th = static_cast<CRoomServer*>(d);
	th ->heartBeatRun();
	return NULL;
}

void CRoomServer::setTimer(int seconds, int mseconds)  
{
	struct timeval temp;  
	temp.tv_sec = seconds;  
	temp.tv_usec = mseconds;
					
	select(0, NULL, NULL, NULL, &temp);
	ALOGD("%s: ==Game===定时%d 秒. %d 微秒向户发送心跳包",__FUNCTION__,seconds,mseconds);
}

bool CRoomServer::OnEventTCPSocketRead(int fd,CMD_Command Command, void* pData, unsigned short wDataSize)
{
	ALOGD("%s: =====Game=========fd: %d,  wMainCmdID: %d, wSubCmdID: %d, wDataSize: %d",__FUNCTION__,fd,Command.wMainCmdID,Command.wSubCmdID,wDataSize);
	 switch (Command.wMainCmdID)
	 {
		 case MDM_GR_LOGON:           //登录消息
		 {
			 return OnSocketMainLogon(fd,Command,pData,wDataSize);
		 }
		 case MDM_GR_USER:            //用户消息
		 {
			 return OnSocketMainUser(fd,Command,pData,wDataSize);
		 }
		 case MDM_KN_COMMAND:         //心跳包
		 {
			 return OnSocketMainHeartBeat(fd,Command,pData,wDataSize);
		 }
	 }
	 return true;
}

bool CRoomServer::OnSocketMainHeartBeat(int fd,CMD_Command Command,void* pData,unsigned short wDataSize)
{
	assert(Command.wMainCmdID == MDM_KN_COMMAND && Command.wSubCmdID == SUB_KN_DETECT_SOCKET);
	ALOGD("%s: =====Game===心跳包=======fd: %d,  wMainCmdID: %d, wSubCmdID: %d",__FUNCTION__,fd,Command.wMainCmdID,Command.wSubCmdID);
    return true; 
}

bool CRoomServer::OnSocketMainLogon(int fd,CMD_Command Command, void* pData, unsigned short wDataSize)
{
	  assert(Command.wMainCmdID == MDM_GR_LOGON);
	  ALOGD("%s: =====Game===1=======fd: %d,  wMainCmdID: %d, wSubCmdID: %d, wDataSize: %d",__FUNCTION__,fd,Command.wMainCmdID,Command.wSubCmdID,wDataSize);
	  switch (Command.wSubCmdID)
	  {
		   case SUB_GR_LOGON_USERID:                 //登陆请求
		   {
			   CMD_GR_LogonByUserID* pLogonUserId = (CMD_GR_LogonByUserID*)pData;
			   ALOGD("%s: =====Game===2=====dwPlazaVersion: %d, dwProcessVersion: %d, szPassWord: %s, dwUserId: %d",__FUNCTION__,
					   pLogonUserId->dwPlazaVersion,pLogonUserId->dwProcessVersion,pLogonUserId->szPassWord,pLogonUserId->dwUserID);

			   tagClientSerial* pTagSerial = (tagClientSerial*)(pLogonUserId+1);
			   ALOGD("%s: ====Game====3===dwSystemVer: %lu, dwComputerID[0]: %lu,dwComputerID[1]: %lu, dwComputerID[2]: %lu",__FUNCTION__,pTagSerial->dwSystemVer,pTagSerial->dwComputerID[0],
					                                      pTagSerial->dwComputerID[1],pTagSerial->dwComputerID[2]);
              
			   //从大厅服务器用户管理容器中查找指定的用户
			   IClientUserItem* p_PlazaUser = CPlazaServer::getInstance()->getPlazaUserManagerPtr()->SearchUserByUserID(pLogonUserId->dwUserID);

			   //查找用户
			   IClientUserItem* p_RoomUser = m_pUserManager ->SearchUserByUserID(pLogonUserId->dwUserID);
			   if(p_RoomUser == NULL && pLogonUserId->dwPlazaVersion == PLAZA_VERSION && 
					   pLogonUserId->dwProcessVersion == PROCESS_VERSION && !strcmp(p_PlazaUser->GetUserData()->szPassword,pLogonUserId->szPassWord))
			   {
                   //将该用户添加到房间用户管理列表中
				   m_pUserManager ->ActiveUserItem(p_PlazaUser);
			       //发送登陆成功消息
                   m_pSocketEnging->SendData(MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,NULL,0,fd);

				   //配置信息
				   CMD_GR_ColumnInfo columnInfo;
				   columnInfo.wColumnCount = 1;
				   
				   for(int i=0;i<columnInfo.wColumnCount;i++)
				   {
					   switch(m_serverType)
					   {
						   case GAME_GENRE_SCORE:
						   {
							   columnInfo.pColumnItem[i].cbDataDescribe = 'S';
				               strcpy(columnInfo.pColumnItem[i].szColumnName,"积分");
				               columnInfo.pColumnItem[i].wColumnWidth = 10;
				               columnInfo.pColumnItem[i].nFormat = 1;
							   break;
						   }
                           case GAME_GENRE_GOLD:
						   {
							   columnInfo.pColumnItem[i].cbDataDescribe = 'G';
				               strcpy(columnInfo.pColumnItem[i].szColumnName,"金币");
				               columnInfo.pColumnItem[i].wColumnWidth = 10;
				               columnInfo.pColumnItem[i].nFormat = 1;
							   break;
						   }
						   case GAME_GENRE_MATCH:
						   {
							   columnInfo.pColumnItem[i].cbDataDescribe = 'M';
				               strcpy(columnInfo.pColumnItem[i].szColumnName,"元宝");
				               columnInfo.pColumnItem[i].wColumnWidth = 10;
				               columnInfo.pColumnItem[i].nFormat = 1;
							   break;
						   }
					   }
				   }


				   ALOGD("%s: ====Game===1=====cbDataDescribe: %c, szColumnName: %s, wColumnWidth: %d, nFormat: %d   数据大小: %d",__FUNCTION__,
						   columnInfo.pColumnItem->cbDataDescribe,columnInfo.pColumnItem->szColumnName,columnInfo.pColumnItem->wColumnWidth,columnInfo.pColumnItem->nFormat,sizeof(columnInfo));
				   //发送列表消息
				   m_pSocketEnging ->SendData(MDM_GR_INFO,SUB_GR_COLUMN_INFO,&columnInfo,sizeof(columnInfo),fd);
                   
				   ALOGD("%s: ====Game===2====cbDataDescribe: %c, szColumnName: %s, wColumnWidth: %d, nFormat: %d",__FUNCTION__,
						   columnInfo.pColumnItem->cbDataDescribe,columnInfo.pColumnItem->szColumnName,columnInfo.pColumnItem->wColumnWidth,columnInfo.pColumnItem->nFormat);

                   CMD_GR_ServerInfo serverInfo;
				   serverInfo.wKindID = 100;
				   serverInfo.wTableCount = 3;
				   serverInfo.wChairCount = 8;
				   serverInfo.dwVideoAddr = inet_addr("127.0.0.1");
				   serverInfo.wGameGenre = 1;
				   serverInfo.cbHideUserInfo = 'H';

				   m_TableFrame->InitTableFrame(serverInfo.wTableCount,serverInfo.wChairCount,serverInfo.cbHideUserInfo);

                   //发送房间信息
				   m_pSocketEnging ->SendData(MDM_GR_INFO,SUB_GR_SERVER_INFO,&serverInfo,sizeof(serverInfo),fd);

				   //配置完成
				   m_pSocketEnging ->SendData(MDM_GR_INFO,SUB_GR_CONFIG_FINISH,NULL,0,fd);
                   
				   //发送房间消息
				   int onLineCount = m_pUserManager->GetOnLineCount();
	               std::list<int>& cliFds = m_pSocketEnging ->GetClientFds();
	               std::list<int>::iterator iter;
	               for(iter = cliFds.begin();iter != cliFds.end();iter++)
				   {
					   m_pSocketEnging ->SendData(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&onLineCount,sizeof(int),*iter);
				   }

                   //发送登陆完成消息
				   m_pSocketEnging ->SendData(MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0,fd);

				   //用户进入
				   tagUserInfoHead userHead;
				   memset(&userHead,0,sizeof(tagUserInfoHead));
                   userHead.wFaceID = p_PlazaUser->GetUserData()->wFaceID;
				   userHead.dwUserID = p_PlazaUser->GetUserData()->dwUserID;
				   userHead.dwGameID = p_PlazaUser->GetUserData()->dwGameID;
				   userHead.dwGroupID = p_PlazaUser->GetUserData()->dwGroupID;
				   userHead.dwUserRight = p_PlazaUser->GetUserData()->dwUserRight;
				   userHead.lLoveliness = p_PlazaUser->GetUserData()->lLoveliness;
				   userHead.dwMasterRight = p_PlazaUser->GetUserData()->dwMasterRight;
                   userHead.cbGender = p_PlazaUser->GetUserData()->cbGender;
				   userHead.cbMemberOrder = p_PlazaUser->GetUserData()->cbMemberOrder;
				   userHead.cbMasterOrder = p_PlazaUser->GetUserData()->cbMasterOrder;
				   userHead.wTableID = p_PlazaUser->GetUserData()->wTableID;
				   userHead.wChairID = p_PlazaUser->GetUserData()->wChairID;
				   userHead.cbUserStatus = p_PlazaUser->GetUserData()->cbUserStatus;
				   userHead.UserScoreInfo.lScore = p_PlazaUser->GetUserData()->lScore;
                   userHead.UserScoreInfo.lGameGold = p_PlazaUser->GetUserData()->lGameGold;
				   userHead.UserScoreInfo.lInsureScore = p_PlazaUser->GetUserData()->lInsureScore;
				   userHead.UserScoreInfo.lWinCount = p_PlazaUser->GetUserData()->lWinCount;
				   userHead.UserScoreInfo.lLostCount = p_PlazaUser->GetUserData()->lLostCount;
				   userHead.UserScoreInfo.lDrawCount = p_PlazaUser->GetUserData()->lDrawCount;
				   userHead.UserScoreInfo.lFleeCount = p_PlazaUser->GetUserData()->lFleeCount;
				   userHead.UserScoreInfo.lExperience = p_PlazaUser->GetUserData()->lExperience;
                   userHead.dwCustomFaceVer = p_PlazaUser->GetUserData()->dwCustomFaceVer;
                   
				   char userInfo[SOCKET_BUFFER];
				   memcpy(userInfo,&userHead,sizeof(userHead));
                   CSendPacketHelper Packet(userInfo+sizeof(userHead),sizeof(userInfo)-sizeof(userHead));
				   //添加用户账号
				   Packet.AddPacket(p_PlazaUser->GetUserData()->szName,strlen(p_PlazaUser->GetUserData()->szName),DTP_USER_ACCOUNTS);
				   //添加用户密码
				   Packet.AddPacket(p_PlazaUser->GetUserData()->szPassword,strlen(p_PlazaUser->GetUserData()->szPassword),DTP_USER_PASS);
				   //添加用户手机号
				   Packet.AddPacket(p_PlazaUser->GetUserData()->szPhone,strlen(p_PlazaUser->GetUserData()->szPhone),DTP_USER_PHONE);
				   //添加个性签名
				   Packet.AddPacket("用一瓶留一瓶",strlen("用一瓶留一瓶"),DTP_UNDER_WRITE);
                   //添加用户社团
				   Packet.AddPacket("劳动社",strlen("劳动社"),DTP_USER_GROUP_NAME);
				   //添加结束标志
				   Packet.AddPacket(NULL,0,0);
                   
	               for(iter = cliFds.begin();iter != cliFds.end();iter++)
				   {
					   //发送用户进入消息
                       m_pSocketEnging ->SendData(MDM_GR_USER,SUB_GR_USER_COME,userInfo,sizeof(userHead)+Packet.GetDataSize(),*iter);
				   }
			   }
			   else
			   {
				   //登陆失败消息
				   CMD_GR_LogonError logonError;
				   logonError.lErrorCode = 1002;
                   strcpy(logonError.szErrorDescribe,"大厅版本错误或进程版本错误或登陆密码错误");
				   //发送登陆失败消息
				   m_pSocketEnging->SendData(MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&logonError,sizeof(logonError),fd);
			   }
			   break;
		   }
	  }
	  return true;
}

bool CRoomServer::OnSocketMainUser(int fd,CMD_Command Command, void* pData, unsigned short wDataSize)
{
	assert(Command.wMainCmdID == MDM_GR_USER);

	CMD_GR_UserReq* pUserReq = (CMD_GR_UserReq*)pData;
    ALOGD("%s:===Game=====wTableID: %d,  wChairID: %d, dwUserID: %d",__FUNCTION__,pUserReq->wTableID,pUserReq->wChairID,pUserReq->dwUserID);
	CMD_GR_UserStatus userStatus;
    bool isClientExit = false;

	switch (Command.wSubCmdID)
	{
		case SUB_GR_USER_SIT_REQ:
		{
			IClientUserItem* p_user = m_pUserManager->SearchUserByUserID(pUserReq->dwUserID);

			if(m_TableFrame->SetUserInfo(pUserReq->wTableID,pUserReq->wChairID,p_user))
			{
				userStatus.dwUserID = pUserReq->dwUserID;
	            userStatus.wTableID = pUserReq->wTableID;
	            userStatus.wChairID = pUserReq->wChairID;
				userStatus.cbUserStatus = US_SIT;
			}
			break;
		}
		case SUB_GR_USER_STANDUP_REQ:
		{
            if(m_TableFrame->SetUserInfo(pUserReq->wTableID,pUserReq->wChairID,NULL))
			{
				userStatus.dwUserID = pUserReq->dwUserID;
	            userStatus.wTableID = INVALID_TABLE;
	            userStatus.wChairID = INVALID_CHAIR;
				userStatus.cbUserStatus = US_FREE;
			}
			break;
		}
		case SUB_GR_USER_LEFT_ROOM_REQ:
		{
			if(m_TableFrame->GetUserInfo(pUserReq->wTableID,pUserReq->wChairID))
			{
				m_TableFrame->SetUserInfo(pUserReq->wTableID,pUserReq->wChairID,NULL);
			}

		    IClientUserItem* pClientUserItem = m_pUserManager ->SearchUserByUserID(pUserReq->dwUserID);
			if(pClientUserItem)
			{
				m_pUserManager ->DeleteUserItem(pClientUserItem);
			}
            isClientExit = true;
			userStatus.dwUserID = pUserReq->dwUserID;
	        userStatus.wTableID = INVALID_TABLE;
	        userStatus.wChairID = INVALID_CHAIR;
			userStatus.cbUserStatus = US_NULL;
			break;
		}
	}

	std::list<int>& cliFds = m_pSocketEnging ->GetClientFds();
	std::list<int>::iterator iter;

	for(iter = cliFds.begin();iter != cliFds.end();iter++)
	{
		//发送用户状态消息
	    m_pSocketEnging ->SendData(MDM_GR_USER,SUB_GR_USER_STATUS,&userStatus,sizeof(CMD_GR_UserStatus),*iter);
	}
    
	CMD_GR_TableInfo tableInfo;
    memset(&tableInfo,0,sizeof(CMD_GR_TableInfo));
	tableInfo.wTableCount = m_TableFrame->GetTableCount();
	for(int i=0;i<tableInfo.wTableCount;i++)
	{
		(tableInfo.pTableStatus+i)->bTableLock = 0;
	    (tableInfo.pTableStatus+i)->bPlayStatus = 1;
	}

	for(iter = cliFds.begin();iter != cliFds.end();iter++)
	{
		//发送桌子信息
	    m_pSocketEnging ->SendData(MDM_GR_STATUS,SUB_GR_TABLE_INFO,&tableInfo,sizeof(CMD_GR_TableInfo),*iter);
	}
    
	CMD_GR_TableStatus tableStatus;
    tableStatus.wTableID = pUserReq->wTableID;
	tableStatus.bTableLock = 0;
	tableStatus.bPlayStatus = 1;
	//发送桌子状态	
	for(iter = cliFds.begin();iter != cliFds.end();iter++)
	{
		m_pSocketEnging ->SendData(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&tableStatus,sizeof(CMD_GR_TableStatus),*iter);
	}
	if(isClientExit)
	{
		for(iter = cliFds.begin();iter != cliFds.end();)
		{
			if(*iter == fd)
			{
				ALOGD("%s:====Game=======客户: %d 退出了",__FUNCTION__,*iter);
				close(*iter);
				iter = cliFds.erase(iter);
				return true;
			}
			iter++;
		}
	}

	return true;
}

void CRoomServer::heartBeatRun()
{
	while(m_running)
	{
	    std::list<int>& cliFds = m_pSocketEnging ->GetClientFds();
	    std::list<int>::iterator iter;
    
	    for(iter = cliFds.begin();iter != cliFds.end();iter++)
	    {
			ALOGD("%s: ===Game===发送心跳包====fd: %d",__FUNCTION__,*iter);
			//发送心跳消息
	        m_pSocketEnging ->SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,NULL,0,*iter);
	    }

		//模拟定时向客户端发送新短信
	    setTimer(120,0);
	}

	ALOGD("%s: ===Game===房间服务器心跳线程退出",__FUNCTION__);
	pthread_exit(NULL);
}

void CRoomServer::run()
{
	fd_set fds;
	int maxfd = 0;
	std::list<int>& cliFds = m_pSocketEnging ->GetClientFds();
	int count = cliFds.size();
	int r;
	std::list<int>::iterator iter;
	
	FD_ZERO(&fds);
	while(m_running)
	{
		FD_ZERO(&fds);
		maxfd = 0;
		FD_SET(m_pSocketEnging->GetSocketFd(),&fds);
		maxfd = maxfd>=m_pSocketEnging->GetSocketFd()?maxfd:m_pSocketEnging->GetSocketFd();
		for(iter = cliFds.begin();iter != cliFds.end();iter++)
		{
			FD_SET(*iter,&fds);
			maxfd = (maxfd>=*iter?maxfd:*iter);
		}
		ALOGD("%s: ===Game=====开始select===serFd: %d,   maxfd: %d   count: %d",__FUNCTION__,m_pSocketEnging->GetSocketFd(),maxfd,cliFds.size());

        r = select(maxfd+1,&fds,NULL,NULL,NULL);
		if(r == -1){
			ALOGE("%s: ====Game===select error : %s",__FUNCTION__,strerror(errno));
            break;
		}

        if(FD_ISSET(m_pSocketEnging->GetSocketFd(),&fds))
		{
			ALOGD("%s: =====Game====Native开始接受客户======",__FUNCTION__);
            m_pSocketEnging->OnAccept();
		}
        
        ALOGD("%s:====Game===开始处理客户请求===1====客户个数: %d",__FUNCTION__,cliFds.size());
		for(iter = cliFds.begin();iter != cliFds.end();)
		{
			ALOGD("%s:====Game===开始处理客户请求===2====客户个数: %d",__FUNCTION__,cliFds.size());
			if(FD_ISSET(*iter,&fds))
			{
				ALOGD("%s:====Game===开始处理客户请求===3====客户: %d 请求来了",__FUNCTION__,*iter);
				int ret = m_pSocketEnging ->OnSocketNotifyRead(*iter,this);
				if(ret == -1)
				{
				    ALOGD("%s:====Game=======客户: %d 退出了",__FUNCTION__,*iter);
					close(*iter);
					iter = cliFds.erase(iter);
					continue;
				}
			}
			iter++;
		}
	}
	ALOGD("%s: ===Game===房间服务器线程退出",__FUNCTION__);
	pthread_exit(NULL);
}

