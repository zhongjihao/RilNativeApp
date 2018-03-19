//
// Created by SPREADTRUM\jihao.zhong on 17-6-20.
//

#include <string.h>
#include <assert.h>
#include "../include/CRecvPacketHelper.h"

//构造函数
CRecvPacketHelper::CRecvPacketHelper(void* pcbBuffer, short wDataSize)
{
    m_wDataPos  = 0;
    m_wDataSize = wDataSize;
    m_pcbBuffer = (unsigned char*)pcbBuffer;
}

//获取数据
void* CRecvPacketHelper::GetData(tagDataDescribe& DataDescribe)
{
    //效验数据
    if (m_wDataPos >= m_wDataSize)
    {
        assert(m_wDataPos==m_wDataSize);
        DataDescribe.wDataSize = 0;
        DataDescribe.wDataDescribe = 0;
        return NULL;
    }
    //获取数据
    assert((m_wDataPos+sizeof(tagDataDescribe))<=m_wDataSize);
    memcpy(&DataDescribe,m_pcbBuffer+m_wDataPos,sizeof(tagDataDescribe));
    assert((m_wDataPos+sizeof(tagDataDescribe)+DataDescribe.wDataSize)<=m_wDataSize);

    //效验数据
    if((m_wDataPos+sizeof(tagDataDescribe)+DataDescribe.wDataSize)>m_wDataSize)
    {
        DataDescribe.wDataSize = 0;
        DataDescribe.wDataDescribe = 0;
        return NULL;
    }

    //设置数据
    void * pData = NULL;
    if(DataDescribe.wDataSize > 0) pData = m_pcbBuffer + m_wDataPos +sizeof(tagDataDescribe);
    m_wDataPos += sizeof(tagDataDescribe) + DataDescribe.wDataSize;
    return pData;
}
