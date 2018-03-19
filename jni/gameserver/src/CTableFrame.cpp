//
// Created by SPREADTRUM\jihao.zhong on 17-8-5.
//

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CTableFrame"

#include <assert.h>
#include <utils/Log.h>
#include "../include/CTableFrame.h"

CTableFrame::CTableFrame()
{
    //状态变量
    m_wDownChairID = INVALID_CHAIR;
    m_wDownTableID = INVALID_TABLE;
    m_wFrameTableID = INVALID_TABLE;

    //配置变量
    m_wTableCount = 0;
    m_wChairCount = 0;
    m_bShowUser = false;
    m_bHideInfo = false;
    m_pTableViewArray.clear();
}

CTableFrame::~CTableFrame()
{
    ALOGD("%s:======Game===",__FUNCTION__);
    DestroyTableFrame();
}

bool CTableFrame::InitTableFrame(short wTableCount, short wChairCount, bool bHideInfo)
{
    ALOGD("%s:======Game===tableCount: %d, chairCount: %d",__FUNCTION__,wTableCount,wChairCount);
    //效验参数
    assert(wChairCount <= 100);

    //创建游戏桌子
    m_bHideInfo = bHideInfo;
    m_wTableCount = wTableCount;
    m_wChairCount = wChairCount;
    for(short i=0;i<m_wTableCount;i++)
    {
        CTableView* pTableView = new CTableView;
        pTableView->InitTableView(i,wChairCount);

        m_pTableViewArray.push_back(pTableView);
    }

    return true;
}

void CTableFrame::DestroyTableFrame()
{
    for(std::vector<CTableView*>::iterator it  = m_pTableViewArray.begin(); it != m_pTableViewArray.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除
        it = m_pTableViewArray.erase(it);
    }

    //状态变量
    m_wDownChairID = INVALID_CHAIR;
    m_wDownTableID = INVALID_TABLE;
    m_wFrameTableID = INVALID_TABLE;

    //配置变量
    m_wTableCount = 0;
    m_wChairCount = 0;
    m_bShowUser = false;

    return;
}

CTableView* CTableFrame::GetTableArrayPtr(short wTableID)const
{
    if((m_pTableViewArray.size() == 0)||(wTableID >= m_wTableCount)) return NULL;
    return m_pTableViewArray[wTableID];
}

bool CTableFrame::ShowUserInfo(bool bShowUser)
{
    m_bShowUser = bShowUser;
    return m_bShowUser;
}

short CTableFrame::GetTableCount()const
{
    return m_wTableCount;
}

short CTableFrame::GetChairCount()const
{
    return m_wChairCount;
}

bool CTableFrame::SetUserInfo(short wTableID, short wChairID, IClientUserItem* pIUserItem)
{
    ALOGD("%s:======Game===wTableID: %d,  wChairID: %d",__FUNCTION__,wTableID,wChairID);
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(pITableView == NULL) return false;

    return pITableView->SetUserInfo(wChairID,pIUserItem);
}

void CTableFrame::SetPlayFlag(short wTableID, bool bPlaying)
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(pITableView == NULL) return;
    pITableView->SetPlayFlag(bPlaying);
}

void CTableFrame::SetPassFlag(short wTableID, bool bPass)
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(pITableView == NULL) return;
    pITableView->SetPassFlag(bPass);
}

IClientUserItem* CTableFrame::GetUserInfo(short wTableID, short wChairID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return NULL;
    return pITableView ->GetUserInfo(wChairID);
}

short CTableFrame::GetNullChairCount(short wTableID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return 0;
    return pITableView ->GetNullChairCount();
}

short CTableFrame::GetNullChairID(short wTableID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return 0;
    return pITableView ->GetNullChairID();
}

bool CTableFrame::QueryPlayFlag(short wTableID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return false;
    return pITableView ->QueryPlayFlag();
}

bool CTableFrame::QueryPassFlag(short wTableID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return false;
    return pITableView ->QueryPassFlag();
}

const tagTableState* CTableFrame::GetTableAttrib(short wTableID)const
{
    CTableView* pITableView = GetTableArrayPtr(wTableID);
    if(!pITableView) return NULL;
    return pITableView ->GetTableAttrib();
}
