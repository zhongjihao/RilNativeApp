//
// Created by SPREADTRUM\jihao.zhong on 17-8-5.
//

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CTableView"

#include <assert.h>
#include <string.h>
#include <utils/Log.h>
#include "../include/CTableView.h"


tagTableState::tagTableState() : bPass(false),bPlaying(false),wTableID(INVALID_TABLE),wSitCount(0),wChairCount(MAX_CHAIR_NORMAL),dwTableMasterID(0)
{
    memset(szTableNote,0, sizeof(szTableNote));
    pIUserItem.clear();
    ALOGD("%s:======Game===",__FUNCTION__);
}

tagTableState::~tagTableState()
{
    ALOGD("%s:======Game===",__FUNCTION__);
    for(std::vector<IClientUserItem*>::iterator it  = pIUserItem.begin(); it != pIUserItem.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除该用户
        it = pIUserItem.erase(it);
    }
}

tagTableState::tagTableState(tagTableState& tableState)
{
    this->bPass = tableState.bPass;
    this->bPlaying = tableState.bPlaying;
    this->wTableID = tableState.wTableID;
    this->wSitCount = tableState.wSitCount;
    this->wChairCount = tableState.wChairCount;
    this->dwTableMasterID = tableState.dwTableMasterID;
    memcpy(this->szTableNote,tableState.szTableNote,strlen(tableState.szTableNote));
    for(unsigned int i = 0; i< pIUserItem.size();i++)
    {
        memcpy(this->pIUserItem[i]->GetUserData(),tableState.pIUserItem[i]->GetUserData(),sizeof(*(tableState.pIUserItem[i]->GetUserData())));
        this->pIUserItem[i]->SetActive(tableState.pIUserItem[i]->IsActive());
    }
}

tagTableState& tagTableState::operator=(const tagTableState& tableState)
{
    this->bPass = tableState.bPass;
    this->bPlaying = tableState.bPlaying;
    this->wTableID = tableState.wTableID;
    this->wSitCount = tableState.wSitCount;
    this->wChairCount = tableState.wChairCount;
    this->dwTableMasterID = tableState.dwTableMasterID;
    memcpy(this->szTableNote,tableState.szTableNote,strlen(tableState.szTableNote));
    for(unsigned int i = 0; i< pIUserItem.size();i++)
    {
        memcpy(this->pIUserItem[i]->GetUserData(),tableState.pIUserItem[i]->GetUserData(),sizeof(*(tableState.pIUserItem[i]->GetUserData())));
        this->pIUserItem[i]->SetActive(tableState.pIUserItem[i]->IsActive());
    }
    return *this;
}

CTableView::CTableView() : m_bClickDown(false),m_bFocusFrame(false),m_wHoverChairID(INVALID_CHAIR)
{
    ALOGD("%s:======Game===",__FUNCTION__);
    m_pTableState = new tagTableState;
}

CTableView::~CTableView()
{
    ALOGD("%s:======Game===",__FUNCTION__);
    if(m_pTableState)
        delete m_pTableState;
}

void CTableView::InitTableView(short wTableID, short wChairCount)
{
    ALOGD("%s:======Game===wTableID: %d, wChairCount: %d",__FUNCTION__,wTableID,wChairCount);
    m_pTableState->wTableID = wTableID;
    m_pTableState->wChairCount = wChairCount;
	for(int i=0;i<wChairCount;i++)
	{
		m_pTableState->pIUserItem.push_back(NULL);
	}
}

bool CTableView::SetUserInfo(short wChairID, IClientUserItem* pIUserItem)
{

    ALOGD("%s:======Game===wChairID: %d",__FUNCTION__,wChairID);
    if((m_pTableState->pIUserItem[wChairID]) == NULL)
    {
        if(pIUserItem != NULL)
        {
            m_pTableState->wSitCount++;
            m_pTableState->pIUserItem.push_back(pIUserItem);
        }
    }
    else if(pIUserItem == NULL)
    {
        if(m_pTableState->wSitCount <=0) return false;
        m_pTableState->wSitCount--;
        return DeleteUserItem(wChairID);
    }

    return true;
}

bool CTableView::DeleteUserItem(short wChairID)
{
    //查找用户
    IClientUserItem* pUserItemActive = NULL;
    for(std::vector<IClientUserItem*>::iterator it  = m_pTableState->pIUserItem.begin(); it != m_pTableState->pIUserItem.end();)
    {
        pUserItemActive = *it;
        assert(pUserItemActive != NULL);
        if(wChairID == pUserItemActive->GetUserData()->wChairID && m_pTableState->wTableID ==  pUserItemActive->GetUserData()->wTableID)
        {
            //释放空间
            delete *it;
            //从容器中删除该用户
            it = m_pTableState->pIUserItem.erase(it);
            return true;
        }
        it++;
    }
    return false;
}

void CTableView::SetMouseDown(bool bClickDown)
{
    if(m_bClickDown != bClickDown)
    {
        m_bClickDown = bClickDown;
    }
    return;
}

void CTableView::SetFocusFrame(bool bFocusFrame)
{
    if(m_bFocusFrame != bFocusFrame)
    {
        m_bFocusFrame = bFocusFrame;
    }
    return;
}

void CTableView::SetHoverChairID(short wHoverChairID)
{
    if(m_wHoverChairID != wHoverChairID)
    {
        m_wHoverChairID = wHoverChairID;
    }
    return;
}

void CTableView::SetPlayFlag(bool bPlaying)
{
    if(m_pTableState->bPlaying != bPlaying)
    {
        m_pTableState->bPlaying = bPlaying;
    }
    return;
}

void CTableView::SetPassFlag(bool bPass)
{
    if(m_pTableState->bPass != bPass)
    {
        m_pTableState->bPass = bPass;
    }
    return;
}

void CTableView::SetTableNote(const char* pszNote)
{
    memcpy(m_pTableState->szTableNote, pszNote, strlen(pszNote));
    return;
}

IClientUserItem* CTableView::GetUserInfo(unsigned short wChairID)const
{
    if(wChairID >= m_pTableState->pIUserItem.size()) return NULL;
    return m_pTableState->pIUserItem[wChairID];
}

short CTableView::GetNullChairCount()const
{
    return m_pTableState->wChairCount - m_pTableState->wSitCount;
}

short CTableView::GetNullChairID()const
{
    short wNullCount = m_pTableState->wChairCount - m_pTableState->wSitCount;
    short pwNullChairID = INVALID_CHAIR;  //空椅子编号
    if(wNullCount !=0 )
    {
        for(short i = 0;i < m_pTableState->wChairCount;i++)
        {
            if(m_pTableState->pIUserItem[i] == NULL)
            {
                pwNullChairID = i;
                break;
            }
        }
    }
    return pwNullChairID;
}

bool CTableView::QueryFocusFrame()const
{
    return m_bFocusFrame;
}

short CTableView::QueryHoverChairID()const
{
    return m_wHoverChairID;
}

bool CTableView::QueryPlayFlag()const
{
    return m_pTableState->bPlaying;
}

bool CTableView::QueryPassFlag()const
{
    return m_pTableState->bPass;
}

tagTableState* CTableView::GetTableAttrib()const
{
    return m_pTableState;
}

