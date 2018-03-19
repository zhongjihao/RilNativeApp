//
// Created by SPREADTRUM\jihao.zhong on 17-6-20.
//

#ifndef CHATFRAME_CSENDPACKETHELPER_H
#define CHATFRAME_CSENDPACKETHELPER_H

//数据包辅助类
class CSendPacketHelper
{
    //变量定义
protected:
   unsigned short	m_wDataSize;					//数据大小
   unsigned short	m_wBufferSize;					//缓冲大小
   unsigned char*   m_pcbBuffer;					//缓冲指针
    //函数定义
public:
    //构造函数
    CSendPacketHelper(void* pcbBuffer, short wBufferSize);
    //析构函数
    virtual ~CSendPacketHelper() {}
    //功能函数
public:
    //清理数据
    void CleanData();
    //获取数据大小
    unsigned short GetDataSize();
    //获取缓冲
    void* GetDataBuffer();
    //插入数据
    bool AddPacket(const void* pData, short wDataSize, short wDataType);
};

#endif //CHATFRAME_CSENDPACKETHELPER_H
