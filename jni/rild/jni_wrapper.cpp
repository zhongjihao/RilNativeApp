/*************************************************************************
    > File Name: jni_wrapper.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 03 Apr 2017 11:03:49 AM CST
 *************************************************************************/


#define LOG_NDEBUG 0
#define LOG_TAG "Rild-Jni"

#include <jni.h>
#include "./libril/include/RilonRequestThread.h"
#include "./libril/include/RilonUnsolicitedResponseThread.h"

static RilonRequestThread             *p_th     = NULL;
static RilonUnsolicitedResponseThread *p_unsol  = NULL;
static JavaVM                         *g_vm     = NULL;
static jclass                          g_cls    = NULL;
static jobject                         g_obj    = NULL;

//定义java中类全名
static const char* classPathName = "com/android/ril/RilJni";

void JNI_OnRilStart(JNIEnv *env,jclass jcls __unused,jobject jobj)
{
	ALOGD("%s: ====Rild=========1==",__FUNCTION__);
    if(g_cls == NULL){
		jclass tmp = env ->GetObjectClass(jobj); //可以在子线程中使用
		g_cls = (jclass)env->NewGlobalRef(tmp);
	    g_obj = env ->NewGlobalRef(jobj);	
	}
	if(p_th == NULL){
		p_th = new RilonRequestThread;
		jclass clazz = env ->FindClass(classPathName);
		if(clazz == NULL) {
			return;
		}
		jfieldID fid = env ->GetStaticFieldID(clazz, "RilSocketName", "Ljava/lang/String;");
		if (fid == NULL) {
	        ALOGE("%s: ====Rild=======2====",__FUNCTION__);
			return;
		}
        jstring jsocket_name = (jstring)env ->GetStaticObjectField(clazz, fid);
        const char* socket_name = env ->GetStringUTFChars(jsocket_name, NULL);
        try{
			ALOGD("%s: ===Rild===java pass socket_name: %s",__FUNCTION__,socket_name);
			p_th ->init(socket_name);
			env ->ReleaseStringUTFChars(jsocket_name, socket_name);
			env ->DeleteLocalRef(clazz);
		}
		catch(ServerException e){
            env ->ReleaseStringUTFChars(jsocket_name, socket_name);
			env ->DeleteLocalRef(clazz);
			delete p_th;
			p_th = NULL;
			return;
		}
	}
	ALOGD("%s: ===Rild====3===",__FUNCTION__);
	p_th ->start();

	if(p_unsol == NULL){
		p_unsol =  new RilonUnsolicitedResponseThread;
	}
	p_unsol ->start(); 
	ALOGD("%s: ===Rild====4=====",__FUNCTION__);
}

void JNI_NotifyThreadExit(void* th)
{
	jclass clazz = NULL;
	bool needsDetach = false;
	JNIEnv* env = NULL;
	if(p_th == (RilonRequestThread*)th){
		ALOGD("%s: ===Rild=======",__FUNCTION__);
		delete p_th;
		p_th = NULL;

		delete p_unsol;
		p_unsol = NULL;
	
        JNIEnv* getJNIEnv(bool* needsDetach);
		env = getJNIEnv(&needsDetach);
		if(env != NULL){
			jmethodID mid = env ->GetMethodID(g_cls,"RIL_onRequestThreadQuit","()V");
	        ALOGD("%s: ===Rild====mid: %p , g_cls: %p  , g_obj : %p",__FUNCTION__,mid, g_cls,g_obj);
			if(mid == NULL){
				ALOGE("%s: ===Rild====无法获取java方法RIL_onRequestThreadQuit====",__FUNCTION__);
				goto on_exit;
			}
			env ->CallVoidMethod(g_obj,mid);	
		}
	}

on_exit:
	env ->DeleteGlobalRef(g_obj);
	env ->DeleteGlobalRef(g_cls);

	if(needsDetach){
		g_vm ->DetachCurrentThread();
	}
	g_obj = NULL;
	g_cls = NULL;
}


JNIEnv* getJNIEnv(bool* needsDetach)
{
	JNIEnv* env = NULL;
	if(g_vm ->GetEnv((void**)&env,JNI_VERSION_1_4) != JNI_OK)
	{
		int status = g_vm ->AttachCurrentThread(&env,NULL);
		if(status < 0)
		{
			ALOGE("%s: ====Rild====failed to attach current thread=====",__FUNCTION__);
			return NULL;
		}
		*needsDetach = true;
	}
	ALOGD("%s: =====Rild====Successfull=======needsDetach: %d",__FUNCTION__,*needsDetach);
	return env;
}


//JAVA函数和C++函数映射关系表
static JNINativeMethod gMethods[] = {
	{ "OnRilStart","(Lcom/android/ril/RilCallback;)V",(void*)JNI_OnRilStart},
};


typedef union{
	JNIEnv* env;
	void* venv;
}UnionJNIEnvToVoid;


static jint registerNativeMethods(JNIEnv* env,const char* className,JNINativeMethod* gMethods,int numMethods)
{
	jclass clazz;
	clazz = env ->FindClass(className);

	ALOGD("%s: ===Rild=======1==",__FUNCTION__);
	if(clazz == NULL){
		return JNI_FALSE;
	}
	
	ALOGD("%s: ===Rild======2====",__FUNCTION__);
	if(env->RegisterNatives(clazz,gMethods,numMethods) < 0){
		env ->DeleteLocalRef(clazz);
		return JNI_FALSE;
	}

	ALOGD("%s: ==Rild======3===",__FUNCTION__);
	env ->DeleteLocalRef(clazz);
	return JNI_TRUE;
}


static jint registerNatives(JNIEnv* env,const char* classPathName,JNINativeMethod *gMethods,int methodsNum)
{
	ALOGD("%s: ===Rild======1====",__FUNCTION__);
	if(!registerNativeMethods(env,classPathName,gMethods,methodsNum)){
		return JNI_FALSE;
	}
	ALOGD("%s: ===Rild======2====",__FUNCTION__);
	return JNI_TRUE;
}

//JNI库加载时自动调用该函数
jint JNI_OnLoad(JavaVM* vm, void* reserved __unused)
{
	g_vm = vm;
	UnionJNIEnvToVoid uenv;
	JNIEnv* env = NULL;

	ALOGD("%s: ==Rild========1===",__FUNCTION__);
	//获得JNI版本
	if(vm->GetEnv((void**)&uenv.venv,JNI_VERSION_1_4) != JNI_OK){
		return -1;
	}

	ALOGD("%s: ===Rild=======2====",__FUNCTION__);
	env = uenv.env;
	//注册java函数
	if(registerNatives(env,classPathName,gMethods,sizeof(gMethods)/sizeof(gMethods[0])) != JNI_TRUE){
		return -1;
	}

	ALOGD("%s: ===Rild=======3===",__FUNCTION__);
	return JNI_VERSION_1_4;
}

