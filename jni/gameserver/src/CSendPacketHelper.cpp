//
// Created by SPREADTRUM\jihao.zhong on 17-6-20.
//

#include "../include/CSendPacketHelper.h"
#include "../include/GameData.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

//构造函数
CSendPacketHelper::CSendPacketHelper(void* pcbBuffer, short wBufferSize)
{
    m_wDataSize = 0;
    m_wBufferSize = wBufferSize;
    m_pcbBuffer = (unsigned char*)pcbBuffer;
}

//清理数据
void CSendPacketHelper::CleanData()
{
    m_wDataSize = 0;
}

//获取数据大小
unsigned short CSendPacketHelper::GetDataSize()
{
    return m_wDataSize;
}

//获取缓冲
void* CSendPacketHelper::GetDataBuffer()
{
    return m_pcbBuffer;
}

//插入数据
bool CSendPacketHelper::AddPacket(const void* pData, short wDataSize, short wDataType)
{
    assert((wDataSize+sizeof(tagDataDescribe)+m_wDataSize)<=m_wBufferSize);
    if ((wDataSize+sizeof(tagDataDescribe)+m_wDataSize)>m_wBufferSize) return false;

    //插入数据
    assert(m_pcbBuffer != NULL);
    tagDataDescribe * pDataDescribe = (tagDataDescribe *)(m_pcbBuffer+m_wDataSize);
    pDataDescribe->wDataSize = wDataSize;
    pDataDescribe->wDataDescribe = wDataType;

    //插入数据
    if(wDataSize>0)
    {
        assert(pData!=NULL);
        memcpy(pDataDescribe+1,pData,wDataSize);
    }

    //设置数据
    m_wDataSize += sizeof(tagDataDescribe)+wDataSize;
    return true;
}

