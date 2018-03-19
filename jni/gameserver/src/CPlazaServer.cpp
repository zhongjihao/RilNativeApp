/*************************************************************************
    > File Name: CPlazaServer.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Wed 12 Jul 2017 02:34:52 PM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CPlazaServer"

#include "../include/CPlazaServer.h"
#include "../include/CIniParse.h"
#include "../include/CClientUserManager.h"
#include "../include/CSendPacketHelper.h"
#include "../include/CRecvPacketHelper.h"
#include <pthread.h>
#include <list>
#include <assert.h>
#include <utils/Log.h>

CPlazaServer* CPlazaServer::m_pInstance = new CPlazaServer;

unsigned long CPlazaServer::m_UserIdIndex = 10010;

CPlazaServer::CPlazaServer() : m_running(false)
{
	memset(m_gameConfigIniPath,0,sizeof(m_gameConfigIniPath));
	m_pSocketEnging = new TCPSocketEnging;
	m_pUserManager = new CClientUserManager;
}

CPlazaServer::~CPlazaServer()
{
	m_pSocketEnging->Close();
	delete m_pSocketEnging;
	delete m_pUserManager;
}

CPlazaServer* CPlazaServer::getInstance()
{
	return m_pInstance;
}

bool CPlazaServer::StartPlazaServer(const char* serIp,int port)
{
	m_pSocketEnging ->initSocket(serIp,port);
    if(!m_running)
	{
		ALOGD("%s: =====Game=======",__FUNCTION__);
		pthread_t tid;
		int ret = pthread_create(&tid,NULL,threadFunc,this);
		if(ret == 0 && tid != -1){
			m_running = true;
		}
	}
	return m_running;
}

void CPlazaServer::SetGameConfigIni(const char* path)
{
	memcpy(m_gameConfigIniPath,path,strlen(path));
}

IClientUserManager* CPlazaServer::getPlazaUserManagerPtr()const
{
	return m_pUserManager;
}

void* CPlazaServer::threadFunc(void *d)
{
	CPlazaServer* th = static_cast<CPlazaServer*>(d);
	th ->run();
	return NULL;
}

bool CPlazaServer::OnEventTCPSocketRead(int fd,CMD_Command Command, void* pData, unsigned short wDataSize)
{
	ALOGD("%s: =====Game=========fd: %d,  wMainCmdID: %d, wSubCmdID: %d, wDataSize: %d",__FUNCTION__,fd,Command.wMainCmdID,Command.wSubCmdID,wDataSize);
	 switch (Command.wMainCmdID)
	 {
		 case MDM_GP_LOGON:           //登录消息
		 {
			 return OnSocketMainLogon(fd,Command,pData,wDataSize);
		 }
	 }
	 return true;
}

bool CPlazaServer::OnSocketMainLogon(int fd,CMD_Command Command, void* pData, unsigned short wDataSize)
{
	  assert(Command.wMainCmdID == MDM_GP_LOGON);
	  ALOGD("%s: =====Game===1=======fd: %d,  wMainCmdID: %d, wSubCmdID: %d, wDataSize: %d",__FUNCTION__,fd,Command.wMainCmdID,Command.wSubCmdID,wDataSize);
	  switch (Command.wSubCmdID)
	  {
		   case SUB_GP_LOGON_ACCOUNTS:                 //账号登陆
		   {
			   LoginAccount* pAcc = (LoginAccount*)pData;

			   ALOGD("%s: =====Game===2=====dwPlazaVersion: %d, szAccounts: %s, szPassWord: %s, dwUserId: %lu",__FUNCTION__,pAcc->dwPlazaVersion,pAcc->szAccounts,pAcc->szPassWord,pAcc->dwUserId);
			   tagClientSerial* pTagSerial = (tagClientSerial*)(pAcc+1);
			   ALOGD("%s: ====Game====3===dwSystemVer: %lu, dwComputerID[0]: %lu,dwComputerID[1]: %lu, dwComputerID[2]: %lu",__FUNCTION__,pTagSerial->dwSystemVer,pTagSerial->dwComputerID[0],
					                                      pTagSerial->dwComputerID[1],pTagSerial->dwComputerID[2]);
              
			   //查找用户
			   IClientUserItem* p_ClientItem = m_pUserManager ->SearchUserByUserID(pAcc->dwUserId);
			   if(p_ClientItem)
			   {
				   //登陆成功消息
				   CMD_GP_LogonSuccess logSucc;
			       logSucc.wFaceID = p_ClientItem ->GetUserData()->wFaceID;
			       logSucc.cbGender = p_ClientItem ->GetUserData() ->cbGender;
			       logSucc.cbMember = p_ClientItem ->GetUserData() ->cbMemberOrder;
			       logSucc.dwUserID = p_ClientItem ->GetUserData() ->dwUserID;
			       logSucc.dwGameID = p_ClientItem ->GetUserData() ->dwGameID;
			       logSucc.dwExperience = p_ClientItem ->GetUserData() ->lExperience;
			       logSucc.lLoveliness = p_ClientItem ->GetUserData() ->lLoveliness;
			       logSucc.lGoldCoin = p_ClientItem ->GetUserData() ->lGameGold;
			       logSucc.lScore = p_ClientItem ->GetUserData() ->lScore;
			       logSucc.dwCustomFaceVer = p_ClientItem ->GetUserData() ->dwCustomFaceVer;

			       char logSuccInfo[SOCKET_BUFFER];
			       memcpy(logSuccInfo,&logSucc,sizeof(logSucc));

				   CSendPacketHelper Packet(logSuccInfo+sizeof(logSucc),sizeof(logSuccInfo)-sizeof(logSucc));
				   //添加用户账号
                   Packet.AddPacket(pAcc->szAccounts,strlen(pAcc->szAccounts),DTP_USER_ACCOUNTS);
                   //添加用户密码
                   Packet.AddPacket(pAcc->szPassWord,strlen(pAcc->szPassWord),DTP_USER_PASS);
                   //添加结束标志
				   Packet.AddPacket(NULL,0,0);
                   
			       ALOGD("%s:====Game=====4====附加数据大小: %d, logSucc: %d, fd: %d",__FUNCTION__,Packet.GetDataSize(),sizeof(logSucc),fd);
			       //发送登陆成功消息
                   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,logSuccInfo,Packet.GetDataSize()+sizeof(logSucc),fd);

                  //加载房间信息配置文件
                  CIniParse iniParse;
				  iniParse.OpenFile(m_gameConfigIniPath,"r");

                  //获取游戏类型配置
				  if(iniParse.CountKeyElement("tagGameType") >0)
				  {
					  tagGameType gameType[iniParse.CountKeyElement("tagGameType")];
					  memset(gameType,0,sizeof(gameType));
					  for(int i=0;i<iniParse.CountKeyElement("tagGameType");i++)
					  {
						  strcpy(gameType[i].szTypeName,iniParse.GetStr(i,"tagGameType","szTypeName"));
				          gameType[i].wSortID = iniParse.GetInt(i,"tagGameType","wSortID ");
				          gameType[i].wTypeID = iniParse.GetInt(i,"tagGameType","wTypeID");	  
					  }
					  //发送游戏类型消息
				      m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,gameType,sizeof(gameType),fd);
				  }              
				 
				  //获取游戏种类配置
                  if(iniParse.CountKeyElement("tagGameKind") >0)
				  {
					  tagGameKind gameKind[iniParse.CountKeyElement("tagGameKind")];
				      memset(gameKind,0,sizeof(gameKind));
					  
					  for(int i=0;i<iniParse.CountKeyElement("tagGameKind");i++)
					  {
						  gameKind[i].wSortID = iniParse.GetInt(i,"tagGameKind","wSortID");
				          gameKind[i].wTypeID = iniParse.GetInt(i,"tagGameKind","wTypeID");
				          gameKind[i].wKindID = iniParse.GetInt(i,"tagGameKind","wKindID");
				          gameKind[i].dwMaxVersion = iniParse.GetInt(i,"tagGameKind","dwMaxVersion");
				          gameKind[i].dwOnLineCount = m_pUserManager ->GetOnLineCount();
				          strcpy(gameKind[i].szKindName,iniParse.GetStr(i,"tagGameKind","szKindName"));
				          strcpy(gameKind[i].szProcessName,iniParse.GetStr(i,"tagGameKind","szProcessName"));   
					  }
					  //发送游戏种类消息
				      m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,gameKind,sizeof(gameKind),fd);
				  }

				 
				  //获取游戏站点配置
                  if(iniParse.CountKeyElement("tagGameStation") > 0)
				  {
					  tagGameStation gameStation[iniParse.CountKeyElement("tagGameStation")];
				      memset(gameStation,0,sizeof(gameStation));

					  for(int i=0;i<iniParse.CountKeyElement("tagGameStation");i++)
					  {
						  gameStation[i].wSortID = iniParse.GetInt(i,"tagGameStation","wSortID");
				          gameStation[i].wKindID = iniParse.GetInt(i,"tagGameStation","wKindID");
				          gameStation[i].wJoinID = iniParse.GetInt(i,"tagGameStation","wJoinID");
				          gameStation[i].wStationID = iniParse.GetInt(i,"tagGameStation","wStationID");
				          strcpy(gameStation[i].szStationName,iniParse.GetStr(i,"tagGameStation","szStationName"));
					  }
					  
					  //发送游戏站点消息
                      m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_STATION,gameStation,sizeof(gameStation),fd);
				  } 
				  
				  CMD_GP_ListConfig listConfig;
				  listConfig.bShowOnLineCount = m_pUserManager ->GetOnLineCount();
				  //发送游戏列表配置消息
				  m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_CONFIG,&listConfig,sizeof(CMD_GP_ListConfig),fd);
				  
                  if(iniParse.CountKeyElement("tagGameServer") > 0)
				  {
				      tagGameServer gameServer[iniParse.CountKeyElement("tagGameServer")];
					  memset(gameServer,0,sizeof(gameServer));

					  for(int i=0;i<iniParse.CountKeyElement("tagGameServer");i++)
					  {
					      gameServer[i].wServerType = iniParse.GetInt(i,"tagGameServer","wServerType");
						  gameServer[i].wSortID = iniParse.GetInt(i,"tagGameServer","wSortID");
				          gameServer[i].wKindID = iniParse.GetInt(i,"tagGameServer","wKindID");
				          gameServer[i].wServerID = iniParse.GetInt(i,"tagGameServer","wServerID");
				          gameServer[i].wStationID = iniParse.GetInt(i,"tagGameServer","wStationID");
                          gameServer[i].wServerPort = iniParse.GetInt(i,"tagGameServer","wServerPort");
				          gameServer[i].dwServerAddr = inet_addr(iniParse.GetStr(i,"tagGameServer","dwServerAddr"));
				          gameServer[i].dwOnLineCount = 500;
                          strcpy(gameServer[i].szServerName,iniParse.GetStr(i,"tagGameServer","szServerName"));
					  }
					  
					  //发送游戏服务器房间消息
				      m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,gameServer,sizeof(gameServer),fd);
				  }
				  
				  //发送列表完成消息
                  m_pSocketEnging ->SendData(MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH,NULL,0,fd);
			   }
			   else
			   {
				   //登陆失败消息
				   CMD_GP_LogonError logonError;
				   logonError.lErrorCode = 1001;
                   sprintf(logonError.szErrorDescribe,"%s账号未注册,请先注册!",pAcc->szAccounts);
				   //发送登陆失败消息
				   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_LOGON_ERROR,&logonError,sizeof(logonError),fd);
			   }
			   //发送登陆完成消息
			   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_LOGON_FINISH,NULL,0,fd);

			   break;
		   }
		   case SUB_GP_REGISTER_ACCOUNTS:          //注册账号
		   {
			   tagUserData* p_userData = new tagUserData;
			   memset(p_userData,0,sizeof(tagUserData));
               
			   void* pDataBuffer = NULL;
			   tagDataDescribe DataDescribe;
			   void* pRegisterSuccess = pData;
			   
			   CRecvPacketHelper RecvPacket(pRegisterSuccess,wDataSize);
			   while(true)
			   {
				   pDataBuffer = RecvPacket.GetData(DataDescribe);
				   if(DataDescribe.wDataDescribe == 0) break;
				   
				   switch (DataDescribe.wDataDescribe)
				   {
					    case DTP_USER_ACCOUNTS:              //用户账号
						{
							assert(DataDescribe.wDataSize <= NAME_LEN);
							assert(pDataBuffer != NULL);
							assert(DataDescribe.wDataSize > 0);
							memcpy(p_userData->szName,pDataBuffer,DataDescribe.wDataSize);
							ALOGD("%s: ==Game=====账号: %s",__FUNCTION__,p_userData->szName);
							break;
						}
                        case DTP_USER_PASS:                 //用户密码
						{
							assert(pDataBuffer != NULL);
							assert(DataDescribe.wDataSize > 0);
							assert(DataDescribe.wDataSize <= PASS_LEN);
							memcpy(p_userData->szPassword,pDataBuffer,DataDescribe.wDataSize);
						    ALOGD("%s: ==Game=====密码: %s",__FUNCTION__,p_userData->szPassword);
							break;
						}
						case DTP_USER_PHONE:               //注册手机号
						{
							assert(pDataBuffer != NULL);
							assert(DataDescribe.wDataSize > 0);
							assert(DataDescribe.wDataSize < PHONE_LEN);
						    memcpy(p_userData->szPhone,pDataBuffer,DataDescribe.wDataSize);
							ALOGD("%s: ==Game=====手机号: %s",__FUNCTION__,p_userData->szPhone);
							break;
						}
                        default:
						    return false;
				   }
			   }

			   //查找用户
			   IClientUserItem* p_ClientItem = m_pUserManager ->SearchUserByPhone(p_userData->szPhone);
               if(p_ClientItem)
			   {
				   //已经注册
				   //发送注册失败消息
				   char msg[100];
				   sprintf(msg,"%s该号码已经注册",p_userData->szPhone);
                   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_REGISTER_FAIL,msg,strlen(msg),fd);
			   }
			   else
			   {
				   //未注册
				   p_userData->wFaceID = 100;
				   p_userData->dwCustomFaceVer = 200;
			       p_userData->dwUserID = m_UserIdIndex++;
			       p_userData->dwGroupID = m_UserIdIndex++;
			       p_userData->dwGameID = m_UserIdIndex++;
			       p_userData->dwUserRight = 1;
			       p_userData->lLoveliness = 20;
			       p_userData->dwMasterRight = 0;
			       p_userData->cbGender = GENDER_BOY;
			       p_userData->cbMemberOrder = '9';
			       p_userData->cbMasterOrder = '2';
			       p_userData->lInsureScore = 1000;
                   p_userData->lGameGold = 20000;
			       p_userData->lScore = 50000;
			       p_userData->lWinCount = 5;
			       p_userData->lLostCount = 10;
			       p_userData->lDrawCount = 2;
			       p_userData->lFleeCount = 3;
			       p_userData->lExperience = 50;
			       p_userData->wMatchRank = 3;
			       p_userData->lUsableScore = 21000;
			       p_userData->wTableID = INVALID_TABLE;
			       p_userData->wChairID = INVALID_CHAIR;
			       p_userData->cbUserStatus = US_NULL;
			       p_userData->cbIsOut = 'N';
				   
				   ALOGD("%s: =====Game===注册用户=====",__FUNCTION__);
                   p_ClientItem = m_pUserManager ->ActiveUserItem(p_userData);
                   CMD_GP_RegisterSuccess regInfo;
				   memset(&regInfo,0,sizeof(CMD_GP_RegisterSuccess));
			       regInfo.wFaceID = p_ClientItem ->GetUserData()->wFaceID;
                   regInfo.cbGender = p_ClientItem ->GetUserData() ->cbGender;
			       regInfo.cbMember = p_ClientItem ->GetUserData() ->cbMemberOrder;
			       regInfo.dwUserID = p_ClientItem ->GetUserData() ->dwUserID;
			       regInfo.dwGameID = p_ClientItem ->GetUserData() ->dwGameID;
                   regInfo.dwExperience = p_ClientItem ->GetUserData() ->lExperience;
			       regInfo.lLoveliness = p_ClientItem ->GetUserData() ->lLoveliness;
			       regInfo.lGoldCoin = p_ClientItem ->GetUserData() ->lGameGold;
			       regInfo.lScore = p_ClientItem ->GetUserData() ->lScore;
			       memcpy(regInfo.szAcccount,p_ClientItem->GetUserData()->szName,strlen(p_ClientItem->GetUserData()->szName));
			       memcpy(regInfo.szPassword,p_ClientItem->GetUserData()->szPassword,strlen(p_ClientItem->GetUserData()->szPassword));
			       memcpy(regInfo.szPhone,p_ClientItem->GetUserData()->szPhone,strlen(p_ClientItem->GetUserData()->szPhone));
                   
				   //发送注册成功消息
                   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_REGISTER_SUCCESS,&regInfo,sizeof(CMD_GP_RegisterSuccess),fd);
			   }
			   break;
		   }
		   case SUB_GP_UNREGISTER_ACCOUNTS:            //注销账号
		   {
               int* userId = (int*)pData;
			   ALOGD("%s: =====Game===注销用户ID： %d",__FUNCTION__,*userId);
			   IClientUserItem* p_clientUserItem = m_pUserManager ->SearchUserByUserID(*userId);
               CMD_GP_UnRegisterResponse response;
			   if(p_clientUserItem)
			   {
				   m_pUserManager ->DeleteUserItem(p_clientUserItem);
                   response.retCode = 0;
				   sprintf(response.szDescribe,"%s该账号注销成功",p_clientUserItem->GetUserData()->szName);
			   }
			   else
			   {
                   response.retCode = 1;
				   strcpy(response.szDescribe,"注销失败，该用户不存在，请先注册!");
			   }
			   //发送注销响应消息
			   m_pSocketEnging->SendData(MDM_GP_LOGON,SUB_GP_UNREGISTER_RESPONSE,&response,sizeof(response),fd);
			   break;
		   }
	  }
	  return true;
}

void CPlazaServer::run()
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
					close(*iter);
					iter = cliFds.erase(iter);
					continue;
				}
			}
			iter++;
		}
	}
	ALOGD("%s: ===Game===大厅服务器线程退出",__FUNCTION__);
	pthread_exit(NULL);
}

