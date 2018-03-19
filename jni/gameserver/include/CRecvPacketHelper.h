//
// Created by SPREADTRUM\jihao.zhong on 17-6-20.
//

#ifndef CHATFRAME_CRECVPACKETHELPER_H
#define CHATFRAME_CRECVPACKETHELPER_H

#include "../include/GameData.h"

//数据包辅助类
class CRecvPacketHelper
{
protected:
    //变量定义
    unsigned short		        m_wDataPos;						//数据点
    unsigned short		        m_wDataSize;					//数据大小
    unsigned char*	            m_pcbBuffer;					//缓冲指针
private:
    CRecvPacketHelper(const CRecvPacketHelper&);
    CRecvPacketHelper& operator=(const CRecvPacketHelper&);
public:
    //构造函数
    CRecvPacketHelper(void* pcbBuffer, short wDataSize);
    virtual ~CRecvPacketHelper(){}

public:
    //功能函数
    //获取数据
    void* GetData(tagDataDescribe& DataDescribe);
};

#endif //CHATFRAME_CRECVPACKETHELPER_H
