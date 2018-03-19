/*************************************************************************
    > File Name: CClientUserItem.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 01 Jul 2017 04:13:53 PM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CClientUserItem"

#include <assert.h>
#include <string.h>
#include "../include/CClientUserItem.h"

CClientUserItem::CClientUserItem()
{
	m_UserData = new tagUserData;
	memset(m_UserData,0,sizeof(tagUserData));
	m_bActive = false;
}

CClientUserItem::CClientUserItem(const CClientUserItem& userItem)
{
	this->m_bActive = userItem.m_bActive;
    memcpy(this->m_UserData,&(userItem.m_UserData),sizeof(tagUserData));
}

CClientUserItem::~CClientUserItem()
{
	CleanData();
}

CClientUserItem& CClientUserItem::operator=(const CClientUserItem& userItem)
{
	this->m_bActive = userItem.m_bActive;
    memcpy(this->m_UserData,&(userItem.m_UserData),sizeof(tagUserData));
	return *this;
}

bool CClientUserItem::IsActive()
{
	return m_bActive;
}

void CClientUserItem::SetActive(bool bActive) 
{
	m_bActive = bActive;
}

int CClientUserItem::GetUserPlayCount()
{
	return m_UserData->lWinCount + m_UserData->lLostCount + m_UserData->lDrawCount + m_UserData->lFleeCount;
}

int CClientUserItem::GetUserID() 
{
	return m_UserData->dwUserID;
}

const char* CClientUserItem::GetUserName() 
{
	return m_UserData->szName;
}

tagUserData* CClientUserItem::GetUserData() 
{
	return m_UserData;
}

void CClientUserItem::SetUserScore(tagUserScore* pUserScore)
{
	//效验参数 
	assert(m_bActive == true);
	assert(pUserScore != NULL);

	//设置变量
	m_UserData->lScore = pUserScore->lScore;
	m_UserData->lWinCount = pUserScore->lWinCount;
	m_UserData->lLostCount = pUserScore->lLostCount;
	m_UserData->lDrawCount = pUserScore->lDrawCount;
	m_UserData->lFleeCount = pUserScore->lFleeCount;
	m_UserData->lExperience = pUserScore->lExperience;
	
	return;
}

void CClientUserItem::SetUserStatus(tagUserStatus* pUserStatus)
{
	//效验参数
	assert(m_bActive == true);
	assert(pUserStatus != NULL);

	//设置变量
	m_UserData->wTableID = pUserStatus->wTableID;
	m_UserData->wChairID = pUserStatus->wChairID;
	m_UserData->cbUserStatus = pUserStatus->cbUserStatus;

	return;
}

void CClientUserItem::CleanData()
{
	if(m_UserData)
		delete m_UserData;
}



