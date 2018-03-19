/*************************************************************************
    > File Name: Ril.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 05:49:32 PM CST
 ************************************************************************/

#ifndef ANDROID_RIL_H
#define ANDROID_RIL_H 1

#define RESPONSE_SOLICITED                  0
#define RESPONSE_UNSOLICITED                1

#define SUCCESS                              0 
#define RIL_ERRNO_INVALID_RESPONSE          -1
#define RIL_LOCAL_SERVER_SOCKET_ERROR       -2
#define RIL_LOCAL_SERVER_ACCEPT_ERROR       -3
#define RIL_LOCAL_SERVER_SEND_ERROR         -4
#define RIL_RESPONSE_EXCEED_LONG            -5
#define GENERIC_FAILURE                     -6


#define RIL_REQUEST_DIAL                    100
#define RIL_REQUEST_HANGUP                  101
#define RIL_REQUEST_UDUB                    102
#define RIL_REQUEST_ANSWER                  103
#define RIL_REQUEST_SEND_SMS                104

#define RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED   1000
#define RIL_UNSOL_RESPONSE_NEW_SMS              1001
#define RIL_UNSOL_RESPONSE_CONNECT              1002
#define RIL_UNSOL_CALL_RING                     1003
#define RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED  1004


// match with constant in RIL.java
#define MAX_COMMAND_BYTES (8 * 1024)

#include <list>
#include <binder/Parcel.h>
#include "ServerException.h"

using namespace android;

class Ril
{
private:
	int m_Fd;
	std::list<int> m_ClientFds;
	static pthread_mutex_t s_writeMutex;
	static Ril* s_Ril;
private:
	Ril();
	Ril(const Ril&);
	Ril& operator=(const Ril& ril);
public:
	~Ril();
	void initSocket(const char* socketname)throw(ServerException);
	void accept()throw(ServerException);
	void closeSocket();
	int getSocketFd()
	{
		return m_Fd;
	}
	list<int>& getClientFds()
	{
		return m_ClientFds;
	}
	static int responseInts(Parcel &p, void *response, size_t responselen);
	static int responseString(Parcel &p, void *response, size_t responselen);
	static int responseStrings(Parcel &p, void *response, size_t responselen);
	static char * strdupReadString(Parcel &p);
	static status_t readStringFromParcelInplace(Parcel &p, char *str, size_t maxLen);
	static void RIL_onRequestComplete(void *response, size_t responselen,int fd);
	static int RIL_onUnsolicitedResponse(int unsolResponse,const void* response,int fd);
	static int sendResponse (Parcel &p,int fd);
	static Ril* createRilInstance();
private:
	static void writeStringToParcel(Parcel &p, const char *s);
	static int blockingWrite(int fd, const void *buffer, size_t len);
	static int sendResponseRaw (const void *data, size_t dataSize, int fd);
};


#endif


