/*************************************************************************
    > File Name: Ril.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 06:02:24 PM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Rild-Socket"

#include <cutils/sockets.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <sys/un.h>
#include <binder/Parcel.h>
#include <cutils/jstring.h>
#include <list>

#include "../include/Ril.h"

//using namespace android;

pthread_mutex_t Ril::s_writeMutex = PTHREAD_MUTEX_INITIALIZER;

Ril* Ril::s_Ril = new Ril;

Ril::Ril()
{
	m_Fd = -1;
	m_ClientFds.clear();
}

Ril::~Ril()
{
	std::list<int>::iterator iter;
	for(iter = m_ClientFds.begin();iter != m_ClientFds.end();iter++)
	{
		close(*iter);
	}
	m_ClientFds.clear();
	closeSocket();
	s_Ril = NULL;
}

Ril* Ril::createRilInstance()
{
	return s_Ril;
}

void Ril::initSocket(const char* socket_name) throw(ServerException)
{
   ALOGD("%s: ===Rild====Start to initSocket  %s", __FUNCTION__,socket_name);
   int ret = 0;
   //m_Fd = android_get_control_socket(socket_name);
   m_Fd = socket_local_server(socket_name,ANDROID_SOCKET_NAMESPACE_ABSTRACT,SOCK_STREAM);
   //struct sockaddr_un srv_addr;
   //socklen_t addrlen;
   //m_Fd = socket(AF_UNIX,SOCK_STREAM,0);
   if(m_Fd == -1)
	   throw ServerException(RIL_LOCAL_SERVER_SOCKET_ERROR,strerror(errno));
   ALOGD("%s: ===Rild====socket_local_server ok !====m_Fd: %d", __FUNCTION__,m_Fd);

   //srv_addr.sun_family = AF_UNIX;
   //strncpy(srv_addr.sun_path,socket_name,sizeof(srv_addr.sun_path)-1);
   //addrlen = sizeof(struct sockaddr);
   //ret = bind(m_Fd,(struct sockaddr*)&srv_addr,addrlen);
   //if(ret == -1){
   //   ALOGD("%s: ===Rild====bind Failed===%s", __FUNCTION__,strerror(errno));
   //   close(m_Fd);
   //   throw ServerException("Failed to bind on control socket %d: %s",m_Fd,strerror(errno));
   //} 
   //ALOGD("%s: ===Rild====bind ok====", __FUNCTION__);

   /*
   ret = listen(m_Fd, 10);
   if(ret == -1){
	   close(m_Fd);
	   throw ServerException("Failed to listen on control socket %d: %s",m_Fd,strerror(errno));
   }
   */
   ALOGD("%s: ====Rild===initSocket ok!======",__FUNCTION__);
}

void Ril::accept() throw(ServerException)
{
	int cliFd = 0;
	struct sockaddr_un cli_addr;
	memset(&cli_addr,0,sizeof(struct sockaddr_un));
    socklen_t clt_addr_len;
	clt_addr_len = sizeof(struct sockaddr_un);

	cliFd = ::accept(m_Fd,(struct sockaddr*)&cli_addr,&clt_addr_len);
	if(cliFd == -1)
		throw ServerException(RIL_LOCAL_SERVER_ACCEPT_ERROR,strerror(errno));

   ALOGD("%s:====Rild===accept ok!========cliFd: %d, cliAddr: %s",__FUNCTION__,cliFd,cli_addr.sun_path);
   m_ClientFds.push_back(cliFd);
}

void Ril::closeSocket()
{
	if(m_Fd != -1){
		close(m_Fd);
		m_Fd = -1;
	}
}

/** response is an int* pointing to an array of ints */
int Ril::responseInts(Parcel &p, void *response, size_t responselen) 
{
	int numInts;
	if (response == NULL && responselen != 0) {
		ALOGE("invalid response: NULL");
		return RIL_ERRNO_INVALID_RESPONSE;
	}
	
	if (responselen % sizeof(int) != 0) {
		ALOGE("responseInts: invalid response length %d expected multiple of %d\n",(int)responselen, (int)sizeof(int));
		return RIL_ERRNO_INVALID_RESPONSE;
	}
				
	int *p_int = (int *) response;
					
	numInts = responselen / sizeof(int);
	p.writeInt32 (numInts);
							
	/* each int*/
	for (int i = 0 ; i < numInts ; i++) {
		p.writeInt32(p_int[i]);
	}
	return 0;
}

int Ril::responseString(Parcel &p, void *response, size_t responselen __unused) 
{
	/* one string only */
	writeStringToParcel(p, (const char *)response);
	return 0;
}

int Ril::responseStrings(Parcel &p, void *response, size_t responselen) 
{
	int numStrings;
    if (response == NULL && responselen != 0) {
		ALOGE("invalid response: NULL");
		return RIL_ERRNO_INVALID_RESPONSE;
	}
	if (responselen % sizeof(char *) != 0) {
		ALOGE("responseStrings: invalid response length %d expected multiple of %d\n",(int)responselen, (int)sizeof(char *));
		return RIL_ERRNO_INVALID_RESPONSE;
	}
	
	if (response == NULL) {
		p.writeInt32 (0);
	} else {
		char **p_cur = (char **) response;
		numStrings = responselen / sizeof(char *);
		p.writeInt32 (numStrings);
		
		/* each string*/
		for (int i = 0 ; i < numStrings ; i++) {
			writeStringToParcel (p, p_cur[i]);
		}
	}
	return 0;
}

void Ril::writeStringToParcel(Parcel &p, const char *s) 
{
	char16_t *s16;
	size_t s16_len;
	s16 = strdup8to16(s, &s16_len);
	p.writeString16(s16, s16_len);
	free(s16);
}

char * Ril::strdupReadString(Parcel &p) 
{
	size_t stringlen;
	const char16_t *s16;
	s16 = p.readString16Inplace(&stringlen);
	
	return strndup16to8(s16, stringlen);
}

status_t Ril::readStringFromParcelInplace(Parcel &p, char *str, size_t maxLen) 
{
	size_t s16Len;
	const char16_t *s16;
	
	s16 = p.readString16Inplace(&s16Len);
	if (s16 == NULL) {
		return NO_MEMORY;
	}
	size_t strLen = strnlen16to8(s16, s16Len);
	if ((strLen + 1) > maxLen) {
		return NO_MEMORY;
	}
	
	if (strncpy16to8(str, s16, strLen) == NULL) {
		return NO_MEMORY;
	} else {
		return NO_ERROR;
	}
}

void Ril::RIL_onRequestComplete(void *response, size_t responselen,int fd)
{
    Parcel p,reply;
	p.setData((uint8_t *)response,responselen);
    int32_t requestId;
	int32_t serial;
	p.readInt32(&requestId);
	p.readInt32(&serial);
	ALOGD("%s: ==Rild=====请求指令: %d, 请求序列号: %d,  SocketId: %d",__FUNCTION__,requestId,serial,fd);
	switch(requestId)
	{
		case RIL_REQUEST_DIAL:
			{
				const char* phone = strdupReadString(p);
				int clirMode;
				p.readInt32(&clirMode);
				ALOGD("%s: ===Rild====电话号码: %s, clirMode: %d",__FUNCTION__,phone,clirMode);
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(serial);
				reply.writeInt32(SUCCESS);   //错误码
				char dialRep[100];
				memset(dialRep,0,sizeof(dialRep));
				sprintf(dialRep,"%s%s%s","正在呼叫",phone,"中...");
				reply.writeString16(String16(dialRep));
			}
			break;
		case RIL_REQUEST_HANGUP:
			{
				int hangup,gsmIndex;
				p.readInt32(&hangup);
				p.readInt32(&gsmIndex);
				ALOGD("%s: ===Rild===挂断电话: %d, gsmIndex: %d",__FUNCTION__,hangup,gsmIndex);
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(serial);
				reply.writeInt32(SUCCESS);   //错误码
				reply.writeString16(String16("挂断成功"));
			}
			break;
		case RIL_REQUEST_SEND_SMS:
			{
				int smsTag;
				p.readInt32(&smsTag);
				const char* smsc = strdupReadString(p);
				const char* sms_pdu = strdupReadString(p);
				ALOGD("%s: ====Rild===短信标签: %d, 短信中心号码: %s, 短信内容: %s",__FUNCTION__,smsTag,smsc,sms_pdu);
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(serial);
				reply.writeInt32(SUCCESS);   //错误码
                char ret[1024];
				memset(ret,0,sizeof(ret));
				sprintf(ret,"短信已送达到该号码: %s, 具体内容为: %s",smsc,sms_pdu);
				reply.writeString16(String16(ret));
			}
			break;
		case RIL_REQUEST_ANSWER:
			{
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(serial);
				reply.writeInt32(SUCCESS);   //错误码
                reply.writeString16(String16("接听成功"));
			}
			break;
		case RIL_REQUEST_UDUB:
			{
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(serial);
				reply.writeInt32(SUCCESS);   //错误码
                reply.writeString16(String16("对方拒绝接听"));
			}
			break;
		default:
			{
				ALOGE("%s: ===Rild===Unrecognized solicited request: %d",__FUNCTION__,requestId);
				reply.writeInt32(RESPONSE_SOLICITED);
				reply.writeInt32(RIL_ERRNO_INVALID_RESPONSE);
                reply.writeInt32(SUCCESS);   //错误码
			}
	}
	sendResponse(reply, fd);
}

int Ril::RIL_onUnsolicitedResponse(int unsolResponse, const void* response,int fd)
{
	Parcel reply;
	reply.writeInt32(RESPONSE_UNSOLICITED);
	reply.writeInt32(unsolResponse);

	ALOGD("%s: ==Rild=====响应指令: %d,  socketId: %d",__FUNCTION__,unsolResponse,fd);
	switch(unsolResponse)
	{
		case RIL_UNSOL_RESPONSE_CONNECT:
			{
				reply.writeString16(String16((const char*)response));
			}
			break;
		case RIL_UNSOL_RESPONSE_NEW_SMS:
			{
				reply.writeString16(String16((const char*)response));
			}
			break;
		case RIL_UNSOL_CALL_RING:
			{
				reply.writeString16(String16((const char*)response));
			}
			break;
		case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
			{

			}
			break;
		default:
			reply.writeString16(String16((const char*)response));
	}
	return sendResponse(reply,fd);
}

int Ril::sendResponse(Parcel &p,int fd) 
{
	return sendResponseRaw(p.data(), p.dataSize(),fd);
}

int Ril::sendResponseRaw(const void *data, size_t dataSize, int fd) 
{
	uint32_t header;
	int ret;
	pthread_mutex_t * writeMutexHook = &s_writeMutex;
    
	if (fd < 0) {
		return GENERIC_FAILURE;
	}

	if (dataSize > MAX_COMMAND_BYTES) {
		ALOGE("RIL: packet larger than %u (%u)",MAX_COMMAND_BYTES, (unsigned int )dataSize);
		return  RIL_RESPONSE_EXCEED_LONG;
	}

    pthread_mutex_lock(writeMutexHook);

	header = htonl(dataSize);
    ALOGD("%s: ====Rild===响应字节数: %u,   header: %u",__FUNCTION__,dataSize,header);
	ret = blockingWrite(fd, (void *)&header, sizeof(header));

	if (ret < 0) {
		pthread_mutex_unlock(writeMutexHook);
		return ret;
	}

	ret = blockingWrite(fd, data, dataSize);

	if (ret < 0) {
		pthread_mutex_unlock(writeMutexHook);
		return ret;
	}

	pthread_mutex_unlock(writeMutexHook);

	return SUCCESS;
}

int Ril::blockingWrite(int fd, const void *buffer, size_t len) 
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
			ALOGE ("RIL Response: unexpected error on write errno:%d", errno);
		//	close(fd);
			return RIL_LOCAL_SERVER_SEND_ERROR;
		}
	}
	
	ALOGD("%s: =====Rild=====Response bytes written:%d", __FUNCTION__,writeOffset);
	return SUCCESS;
}



