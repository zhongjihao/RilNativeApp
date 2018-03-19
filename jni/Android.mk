LOCAL_PATH := $(call my-dir)

########### Build Ril  ################################################################
include $(CLEAR_VARS)
 
LOCAL_MODULE    	:= libril_native

LOCAL_SRC_FILES 	:= \
	./rild/jni_wrapper.cpp \
	./rild/libril/src/Ril.cpp \
	./rild/libril/src/RilonRequestThread.cpp \
	./rild/libril/src/RilonUnsolicitedResponseThread.cpp \
	./rild/libril/src/ServerException.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libbinder \
	libcutils

#LOCAL_LDLIBS := -llog


#LOCAL_LDLIBS := \
	-llog \
	-lutils \
	-lbinder \
	-lcutils



LOCAL_CFLAGS += -frtti -fexceptions -std=c++11

#LOCAL_SDK_VERSION := 8
#LOCAL_NDK_STL_VARIANT := c++_static

include $(BUILD_SHARED_LIBRARY)

########## Build gameserver  ##############################################
include $(CLEAR_VARS)
LOCAL_MODULE    := libgameserver
LOCAL_SRC_FILES := ./gameserver/src/CClientUserItem.cpp \
	               ./gameserver/src/CClientUserManager.cpp \
				   ./gameserver/src/CPlazaServer.cpp \
				   ./gameserver/src/CRecvPacketHelper.cpp \
				   ./gameserver/src/CSendPacketHelper.cpp \
				   ./gameserver/src/TCPSocketEnging.cpp \
				   ./gameserver/src/CIniParse.cpp   \
				   ./gameserver/src/CTableFrame.cpp \
				   ./gameserver/src/CTableView.cpp \
				   ./gameserver/src/CRoomServer.cpp \
				   ./gameserver/jni_wrapper.cpp

LOCAL_SHARED_LIBRARIES := liblog \
	                      libutils \
						  libcutils

LOCAL_CFLAGS += -frtti -fexceptions -std=c++11

include $(BUILD_SHARED_LIBRARY)



