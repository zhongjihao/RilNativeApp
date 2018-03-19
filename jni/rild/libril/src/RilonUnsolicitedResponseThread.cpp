/*************************************************************************
    > File Name: RilonUnsolicitedResponseThread.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Tue 16 May 2017 04:34:04 PM CST
 ************************************************************************/
#define LOG_NDEBUG 0
#define LOG_TAG "Rild-onUnsoliciteResponseThread"

#include "../include/RilonUnsolicitedResponseThread.h"

RilonUnsolicitedResponseThread::RilonUnsolicitedResponseThread():m_running(false),m_tid(-1)
{

}

RilonUnsolicitedResponseThread::~RilonUnsolicitedResponseThread()
{
	m_running = false;
	m_tid = -1;
	Ril* ril = Ril::createRilInstance();
	if(ril)
		delete ril;
}

void RilonUnsolicitedResponseThread::start()
{
	if(!m_running){
		ALOGD("%s: ===Rild=========",__FUNCTION__);
		int ret = pthread_create(&m_tid,NULL,threadFunc,this);
		if(ret == 0 && m_tid != -1){
			m_running = true;
		}
	}
}

void* RilonUnsolicitedResponseThread::threadFunc(void *d)
{
	RilonUnsolicitedResponseThread* th = static_cast<RilonUnsolicitedResponseThread*>(d);
	th ->run();
	return NULL;
}


void RilonUnsolicitedResponseThread::run()
{
	Ril* ril = Ril::createRilInstance();
	list<int>& cliFds = ril ->getClientFds();
	list<int>::iterator iter;
	while(m_running)
	{
		setTimer(30,100);
		//模拟来电
		for(iter = cliFds.begin();iter != cliFds.end();iter++)
		{
			ALOGD("%s:====Rild=====来电===cliFd: %d",__FUNCTION__,*iter);
			int ret = Ril::RIL_onUnsolicitedResponse(RIL_UNSOL_CALL_RING,"响铃中...",*iter);
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

		//模拟定时向客户端发送新短信
		setTimer(30,100);
		for(iter = cliFds.begin();iter != cliFds.end();iter++)
		{
			ALOGD("%s:====Rild=====开始向客户发送消息===cliFd: %d",__FUNCTION__,*iter);
			int ret = Ril::RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS,"客户你好，欢迎来到广东深圳市!",*iter);
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
	}
	ALOGD("%s: ===Rild===RilonUnsolicitedResponseThread线程退出",__FUNCTION__);
	pthread_exit(NULL);
}

void RilonUnsolicitedResponseThread::setTimer(int seconds, int mseconds)  
{
	struct timeval temp;  
	temp.tv_sec = seconds;  
	temp.tv_usec = mseconds;
	
	select(0, NULL, NULL, NULL, &temp);
	ALOGD("%s: ==Rild===Ril定时%d 秒. %d 微秒向户发送消息",__FUNCTION__,seconds,mseconds);
}

