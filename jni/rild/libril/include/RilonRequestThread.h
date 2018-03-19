/*************************************************************************
   > File Name: RilonRequestThread.h
   > Author: zhongjihao
   > Mail: zhongjihao100@163.com 
   > Created Time: Mon 03 Apr 2017 09:29:15 AM CST
 *************************************************************************/



#ifndef RIL_ONREQUEST_THREAD_H
#define RIL_ONREQUEST_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <list>
#include <sys/socket.h>
#include <sys/select.h>
#include <binder/Parcel.h>
#include <string>
#include <arpa/inet.h>
#include "ServerException.h"
#include "Ril.h"

class RilonRequestThread
{
private:
    bool m_running;
    pthread_t m_tid;
private:
    void run();
    static void* threadFunc(void* d);
public:
	RilonRequestThread();
	~RilonRequestThread();
    void init(const char* socket_name)throw(ServerException);
    void start();
    void stop();
    pthread_t& getTid(){ return m_tid;}
};

extern void JNI_NotifyThreadExit(void* d);

#endif

