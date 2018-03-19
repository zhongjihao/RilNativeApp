/*************************************************************************
    > File Name: RilonRequestThread.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 09:37:24 AM CST
 **************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Rild-onRequestThread"

#include "../include/RilonRequestThread.h"

using namespace android;

RilonRequestThread::RilonRequestThread():m_running(false),m_tid(-1)
{

}

RilonRequestThread::~RilonRequestThread()
{
	stop();
}

void RilonRequestThread::init(const char* socket_name)throw(ServerException)
{
    try{
		ALOGD("%s: ==Rild======socket_name: %s",__FUNCTION__,socket_name);
        Ril::createRilInstance()->initSocket(socket_name);
    }catch(ServerException e){
        throw e;
    }
}

void* RilonRequestThread::threadFunc(void *d)
{
    RilonRequestThread* th = static_cast<RilonRequestThread*>(d);
    th ->run();
	return NULL;
}

void RilonRequestThread::start()
{
	if(!m_running){
		ALOGD("%s: ===Rild=========",__FUNCTION__);
		int ret = pthread_create(&m_tid,NULL,threadFunc,this);
        if(ret == 0 && m_tid != -1){
			m_running = true;
		}
	} 
}

void RilonRequestThread::stop()
{
	ALOGD("%s: ==Rild=======",__FUNCTION__);
    m_running = false;
	m_tid = -1;
    Ril* ril = Ril::createRilInstance();
	if(ril)
		delete ril;
}

static void threadExitHandle(void *d)
{
	ALOGD("%s: =====Rild======",__FUNCTION__);
	RilonRequestThread* th = static_cast<RilonRequestThread*>(d);
	JNI_NotifyThreadExit(th);
}

void RilonRequestThread::run()
{
    fd_set fds;
    int maxfd = 0;
    Ril* ril = Ril::createRilInstance();
	list<int>& cliFds = ril ->getClientFds();
    int count = cliFds.size();
    int r;
    list<int>::iterator iter;
    
    FD_ZERO(&fds);
    pthread_cleanup_push(threadExitHandle,this);
    while(m_running)
    {
        FD_ZERO(&fds);
        maxfd = 0;
        FD_SET(ril->getSocketFd(),&fds);
        maxfd = maxfd>=ril->getSocketFd()?maxfd:ril->getSocketFd();
        for(iter = cliFds.begin();iter != cliFds.end();iter++)
        {
			FD_SET(*iter,&fds);
            maxfd = (maxfd>=*iter?maxfd:*iter);
        }
        ALOGD("%s: ===Rild=====开始select===serFd: %d,   maxfd: %d   count: %d",__FUNCTION__,ril->getSocketFd(),maxfd,cliFds.size());

        r = select(maxfd+1,&fds,NULL,NULL,NULL);
        if(r == -1){
            ALOGE("%s: ====Rild===select error : %s",__FUNCTION__,strerror(errno));
            break;
        }
        if(FD_ISSET(ril->getSocketFd(),&fds)){
            try{
                ALOGD("%s: =====Rild====Native开始接受客户======",__FUNCTION__);
                ril->accept();
			}catch(ServerException e){
				int errorCode = e.geterrorCode();
                ALOGE("%s: ======Rild====errorCode: %d,  errmsg: %s",__FUNCTION__,errorCode,e.what());
                break;  
            }
			for(iter = cliFds.begin();iter != cliFds.end();iter++)
			{
				ALOGD("%s:====Rild=====有客户连接上来了===cliFd: %d",__FUNCTION__,*iter);
				int ret = Ril::RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CONNECT,"客户你好，欢迎连上我，很高兴为您服务!",*iter);
			    switch(ret){
					case RIL_LOCAL_SERVER_SEND_ERROR:
					case GENERIC_FAILURE:
						ALOGE("%s:====Rild===写入数据出错=====",__FUNCTION__);
						close(*iter);
						cliFds.remove(*iter);
						break;
					case RIL_RESPONSE_EXCEED_LONG:
						Ril::RIL_onUnsolicitedResponse(RIL_RESPONSE_EXCEED_LONG,"响应数据包太大!",*iter);
						break;
				}
			}
			
			//测试线程推出后的流程
			//	break;
        }
        ALOGD("%s:====Rild===开始处理客户请求===1====客户个数: %d",__FUNCTION__,cliFds.size());
        for(iter = cliFds.begin();iter != cliFds.end();)
        {
			ALOGD("%s:====Rild===开始处理客户请求===2====客户个数: %d",__FUNCTION__,cliFds.size());
            if(FD_ISSET(*iter,&fds))
            {
			    ALOGD("%s:====Rild===开始处理客户请求===3====客户: %d 请求来了",__FUNCTION__,*iter);
				//接收请求数据长度
				uint32_t dataLength = 0;
				//char dataLength[4];
			    //memset(dataLength,0,sizeof(dataLength));
                r = recv(*iter,&dataLength,sizeof(dataLength),MSG_WAITALL);
                if(r == 0){
                    ALOGD("%s:====Rild===有客户退出",__FUNCTION__);
                    close(*iter);
                    iter = cliFds.erase(iter);
					continue;
                }
                if(r == -1){
                    ALOGE("%s: ==Rild===客户端网络故障: %s",__FUNCTION__,strerror(errno));
                    close(*iter);
                    iter = cliFds.erase(iter);
					continue;
                }
                if(r>0){
                    //接收客户端请求数据
					//int requestLen = dataLength[0]*256*256*256 + dataLength[1]*256*256 + dataLength[2]*256 + dataLength[3];
					//int requestLen = ((dataLength & 0xFF) << 24) | ((dataLength >> 8 & 0xFF) << 16) | ((dataLength >> 16 & 0xFF) << 8) | (dataLength >> 24);
					int requestLen = ntohl(dataLength);
                    ALOGD("%s: ===Rild====请求数据长度requestLen : %d,  dataLength: %d",__FUNCTION__,requestLen,dataLength);
					char data[requestLen];
					memset(data,0,sizeof(data));
					r = recv(*iter,data,requestLen,MSG_WAITALL);    
                    if(r == 0){
						ALOGD("%s: ===Rild====有客户退出",__FUNCTION__);
						close(*iter);
						iter = cliFds.erase(iter);
						continue;
					}
                    if(r == -1){
						ALOGE("%s: ===Rild===客户端网络故障: %s",__FUNCTION__,strerror(errno));
						close(*iter);
                        iter = cliFds.erase(iter);
						continue;
					}
                    if(r > 0){
						Ril::RIL_onRequestComplete(data,requestLen,*iter);
					}
                }
            }
			iter++;
        } 
    }
    ALOGD("%s: ===Rild===RilonRequestThread线程退出",__FUNCTION__);
	pthread_cleanup_pop(1);
    pthread_exit(NULL);
}


