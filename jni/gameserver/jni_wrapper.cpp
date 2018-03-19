/*************************************************************************
    > File Name: jni_wrapper.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 11:03:49 AM CST
 *************************************************************************/


#define LOG_NDEBUG 0
#define LOG_TAG "Game-Jni"

#include <jni.h>
#include <utils/Log.h>
#include "./include/CPlazaServer.h"
#include "./include/CRoomServer.h"

static CRoomServer* g_pRoomServer1 = NULL;
static CRoomServer* g_pRoomServer2 = NULL;
static CRoomServer* g_pRoomServer3 = NULL;
static CRoomServer* g_pRoomServer4 = NULL;

//定义java中类全名
static const char* classPathName = "com/android/gameserver/GameJni";

void JNI_OnGamePlazaStart(JNIEnv *env,jclass jcls __unused)
{
	ALOGD("%s: ====Game=========1==",__FUNCTION__);
	jclass clazz = env ->FindClass(classPathName);
	if(clazz == NULL) {
		return;
	}
	jfieldID fidIp = env ->GetStaticFieldID(clazz, "PlazaServerIP", "Ljava/lang/String;");
	jfieldID fidPort = env ->GetStaticFieldID(clazz,"PlazaServerPort","I");
	if (fidIp == NULL || fidPort == NULL) {
		ALOGE("%s: ====Game=======2====",__FUNCTION__);
		return;
	}
    jstring jplazaIp = (jstring)env ->GetStaticObjectField(clazz, fidIp);
    const char* plazaIp = env ->GetStringUTFChars(jplazaIp, NULL);
    jint jplazaPort = (jint)env ->GetStaticIntField(clazz,fidPort);
    
	ALOGD("%s: ===Game===3===java pass PlazaServerIP: %s, PlazaServerPort: %d",__FUNCTION__,plazaIp,jplazaPort);
	CPlazaServer::getInstance() ->StartPlazaServer(plazaIp,(int)jplazaPort);
	env ->ReleaseStringUTFChars(jplazaIp, plazaIp);
	env ->DeleteLocalRef(clazz);
	
	ALOGD("%s: ===Game====4=====",__FUNCTION__);
}

void JNI_OnGameConfigIni(JNIEnv *env,jclass jcls __unused,jstring jpath)
{
	const char* path = env ->GetStringUTFChars(jpath, NULL);
	ALOGD("%s: ===Game===java pass configIni: %s",__FUNCTION__,path);
	CPlazaServer::getInstance()->SetGameConfigIni(path);
	env ->ReleaseStringUTFChars(jpath, path);
}


void JNI_OnGameRoom1Start(JNIEnv *env,jclass jcls __unused,jstring jip,jint jport,jint jserverId,jint jserverType)
{
	const char* ip = env ->GetStringUTFChars(jip, NULL);
	ALOGD("%s: ===Game===Room1====java pass room ipaddr: %s, port: %d,  serverId: %d   serverType: %d",__FUNCTION__,ip,(int)jport,(int)jserverId,(int)jserverType);
    g_pRoomServer1 = new CRoomServer((int)jserverId,(int)jserverType);
	g_pRoomServer1 ->StartRoomServer(ip,(int)jport);

	env ->ReleaseStringUTFChars(jip, ip);
}

void JNI_OnGameRoom2Start(JNIEnv *env,jclass jcls __unused,jstring jip,jint jport,jint jserverId,jint jserverType)
{
	const char* ip = env ->GetStringUTFChars(jip, NULL);
	ALOGD("%s: ===Game===Room2==java pass room ipaddr: %s, port: %d,  serverId: %d   serverType: %d",__FUNCTION__,ip,(int)jport,(int)jserverId,(int)jserverType);
    g_pRoomServer2 = new CRoomServer((int)jserverId,(int)jserverType);
	g_pRoomServer2 ->StartRoomServer(ip,(int)jport);

	env ->ReleaseStringUTFChars(jip, ip);
}

void JNI_OnGameRoom3Start(JNIEnv *env,jclass jcls __unused,jstring jip,jint jport,jint jserverId,jint jserverType)
{
	const char* ip = env ->GetStringUTFChars(jip, NULL);
	ALOGD("%s: ===Game=====Room3===java pass room ipaddr: %s, port: %d,  serverId: %d   serverType: %d",__FUNCTION__,ip,(int)jport,(int)jserverId,(int)jserverType);
    g_pRoomServer3 = new CRoomServer((int)jserverId,(int)jserverType);
	g_pRoomServer3 ->StartRoomServer(ip,(int)jport);

	env ->ReleaseStringUTFChars(jip, ip);
}

void JNI_OnGameRoom4Start(JNIEnv *env,jclass jcls __unused,jstring jip,jint jport,jint jserverId,jint jserverType)
{
	const char* ip = env ->GetStringUTFChars(jip, NULL);
	ALOGD("%s: ===Game====Room4====java pass room ipaddr: %s, port: %d,  serverId: %d   serverType: %d",__FUNCTION__,ip,(int)jport,(int)jserverId,(int)jserverType);
    g_pRoomServer4 = new CRoomServer((int)jserverId,(int)jserverType);
	g_pRoomServer4 ->StartRoomServer(ip,(int)jport);

	env ->ReleaseStringUTFChars(jip, ip);
}

//JAVA函数和C++函数映射关系表
static JNINativeMethod gMethods[] = {
	{ "OnGamePlazaStart","()V",(void*)JNI_OnGamePlazaStart},
	{ "setGameConfigIni","(Ljava/lang/String;)V",(void*)JNI_OnGameConfigIni},
	{ "OnGameRoom1Start","(Ljava/lang/String;III)V",(void*)JNI_OnGameRoom1Start},
	{ "OnGameRoom2Start","(Ljava/lang/String;III)V",(void*)JNI_OnGameRoom2Start},
	{ "OnGameRoom3Start","(Ljava/lang/String;III)V",(void*)JNI_OnGameRoom3Start},
	{ "OnGameRoom4Start","(Ljava/lang/String;III)V",(void*)JNI_OnGameRoom4Start},
};


typedef union{
	JNIEnv* env;
	void* venv;
}UnionJNIEnvToVoid;


static jint registerNativeMethods(JNIEnv* env,const char* className,JNINativeMethod* gMethods,int numMethods)
{
	jclass clazz;
	clazz = env ->FindClass(className);

	ALOGD("%s: ===Game=======1==",__FUNCTION__);
	if(clazz == NULL){
		return JNI_FALSE;
	}
	
	ALOGD("%s: ===Game======2====",__FUNCTION__);
	if(env->RegisterNatives(clazz,gMethods,numMethods) < 0){
		env ->DeleteLocalRef(clazz);
		return JNI_FALSE;
	}

	ALOGD("%s: ==Game======3===",__FUNCTION__);
	env ->DeleteLocalRef(clazz);
	return JNI_TRUE;
}


static jint registerNatives(JNIEnv* env,const char* classPathName,JNINativeMethod *gMethods,int methodsNum)
{
	ALOGD("%s: ===Game======1====",__FUNCTION__);
	if(!registerNativeMethods(env,classPathName,gMethods,methodsNum)){
		return JNI_FALSE;
	}
	ALOGD("%s: ===Game======2====",__FUNCTION__);
	return JNI_TRUE;
}

//JNI库加载时自动调用该函数
jint JNI_OnLoad(JavaVM* vm, void* reserved __unused)
{
	UnionJNIEnvToVoid uenv;
	JNIEnv* env = NULL;

	ALOGD("%s: ==Game========1===",__FUNCTION__);
	//获得JNI版本
	if(vm->GetEnv((void**)&uenv.venv,JNI_VERSION_1_4) != JNI_OK){
		return -1;
	}

	ALOGD("%s: ===Game=======2====",__FUNCTION__);
	env = uenv.env;
	//注册java函数
	if(registerNatives(env,classPathName,gMethods,sizeof(gMethods)/sizeof(gMethods[0])) != JNI_TRUE){
		return -1;
	}

	ALOGD("%s: ===Game=======3===",__FUNCTION__);
	return JNI_VERSION_1_4;
}

