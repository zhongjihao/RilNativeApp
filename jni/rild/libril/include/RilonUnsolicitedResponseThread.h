/*************************************************************************
    > File Name: RilonUnsolicitedResponseThread.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Tue 16 May 2017 03:48:55 PM CST
 ************************************************************************/

#ifndef RIL_ONUNSOLICITEDRESPONSE_H
#define RIL_ONUNSOLICITEDRESPONSE_H

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <list>
#include <sys/socket.h>
#include <sys/select.h>
#include <binder/Parcel.h>
#include <string>
#include <arpa/inet.h>
#include <time.h> 
#include <sys/time.h>
#include "ServerException.h"
#include "Ril.h"

class RilonUnsolicitedResponseThread
{
private:
	bool m_running;
	pthread_t m_tid;
private:
	void run();
	static void* threadFunc(void* d);
	static void setTimer(int seconds, int mseconds);
public:
	RilonUnsolicitedResponseThread();
	~RilonUnsolicitedResponseThread();
	void start();
	pthread_t& getTid(){
		return m_tid;
	}
};


#endif



