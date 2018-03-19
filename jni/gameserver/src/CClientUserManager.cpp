/*************************************************************************
    > File Name: CClientUserManager.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 10 Jul 2017 11:08:05 AM CST
 ************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "Game-CClientUserManager"

#include <assert.h>
#include "../include/CClientUserManager.h"

using namespace std;

CClientUserManager::CClientUserManager()
{

}

CClientUserManager::~CClientUserManager()
{
	for(vector<CClientUserItem*>::iterator it  = m_UserItemActive.begin(); it != m_UserItemActive.end();)
	{
		//释放空间
	    delete *it;
	    //从容器中删除该用户
		it = m_UserItemActive.erase(it);
	}

	for(vector<CClientUserItem*>::iterator it  = m_UserItemStorage.begin(); it != m_UserItemStorage.end();)
	{
		//释放空间
	    delete *it;
	    //从容器中删除该用户
		it = m_UserItemStorage.erase(it);
	}
}

//释放函数
void CClientUserManager::Release()
{
	for(vector<CClientUserItem*>::iterator it  = m_UserItemActive.begin(); it != m_UserItemActive.end();)
	{
		//向存储数组中添加用户
		m_UserItemStorage.push_back(*it);
	    //释放用户
		it = m_UserItemActive.erase(it);
	}
}

//增加用户
IClientUserItem* CClientUserManager::ActiveUserItem(tagUserData* UserData)
{
	//寻找用户
	CClientUserItem*  pUserItem = NULL;
	int nStorageCount = m_UserItemStorage.size();
	if(nStorageCount>0)
	{
		pUserItem = m_UserItemStorage[nStorageCount-1];
		assert(pUserItem != NULL);
		m_UserItemStorage.pop_back();
	}
	else
		pUserItem = new CClientUserItem();
	//拷贝数据
	pUserItem ->SetActive(true);
	memcpy(pUserItem->GetUserData(),UserData,sizeof(tagUserData));
	m_UserItemActive.push_back(pUserItem);

	return pUserItem;
}

//删除用户
bool CClientUserManager::DeleteUserItem(IClientUserItem* pIUserItem)
{
	//查找用户
	CClientUserItem* pUserItemActive = NULL;	
	for(vector<CClientUserItem*>::iterator it  = m_UserItemActive.begin(); it != m_UserItemActive.end();)
	{
		pUserItemActive = *it;
		assert(pUserItemActive != NULL);
        if(pIUserItem->GetUserData()->dwUserID == pUserItemActive->GetUserData()->dwUserID)
		{
			//删除用户
			m_UserItemStorage.push_back(pUserItemActive);
			it = m_UserItemActive.erase(it);
			return true;
		}
		it++;
	}
	return false;
}

//更新用户
bool CClientUserManager::UpdateUserItem(IClientUserItem* pIUserItem)
{
	//查找用户
	CClientUserItem* pUserItemActive = NULL;
	for(vector<CClientUserItem*>::iterator it  = m_UserItemActive.begin(); it != m_UserItemActive.end();)
	{
		pUserItemActive = *it;
		assert(pUserItemActive != NULL);
		if(pIUserItem->GetUserData()->dwUserID == pUserItemActive->GetUserData()->dwUserID)
		{
			//删除用户
			delete pUserItemActive;
			it = m_UserItemActive.erase(it);
			m_UserItemActive.push_back(reinterpret_cast<CClientUserItem*>(pIUserItem));
			return true;
		}
		it++;
	}
	return false;
}

//添加用户
void CClientUserManager::ActiveUserItem(IClientUserItem* pIUserItem)
{
	m_UserItemActive.push_back(reinterpret_cast<CClientUserItem*>(pIUserItem));
}

//更新积分
bool CClientUserManager::UpdateUserItemScore(IClientUserItem* pIUserItem, tagUserScore* pUserScore)
{
	//效验参数
	if(pUserScore == NULL) return false;
	//设置数据
	pIUserItem->SetUserScore(pUserScore);

	return true;
}

//更新状态
bool CClientUserManager::UpdateUserItemStatus(IClientUserItem* pIUserItem, tagUserStatus* pUserStatus)
{
	//效验参数
	if(pUserStatus == NULL) return false;
	//设置数据
	pIUserItem->SetUserStatus(pUserStatus);
	return true;
}

//获取人数
int CClientUserManager::GetOnLineCount()
{
	return m_UserItemActive.size();
}

//枚举用户
IClientUserItem* CClientUserManager::EnumUserItem(unsigned short wEnumIndex)
{
	assert(wEnumIndex >=0);
	if(wEnumIndex < m_UserItemActive.size())
	{
		CClientUserItem*  pUserItem = m_UserItemActive[wEnumIndex];
		assert(pUserItem != NULL);
		assert(pUserItem->IsActive() == true); 
		return pUserItem;
	}
	return NULL;
}

//查找用户
IClientUserItem* CClientUserManager::SearchUserByUserID(unsigned int dwUserID)
{
	CClientUserItem*  pUserItem = NULL;
	for (unsigned int i=0;i<m_UserItemActive.size();i++)
	{
		pUserItem = m_UserItemActive[i];
		assert(pUserItem != NULL);
		assert(pUserItem->IsActive() == true);
		if(pUserItem->GetUserData()->dwUserID == dwUserID) return pUserItem;
	}
	return NULL;
}

//查找用户
IClientUserItem* CClientUserManager::SearchUserByGameID(unsigned int dwGameID)
{
	CClientUserItem*  pUserItem = NULL;
	for(unsigned int i=0;i<m_UserItemActive.size();i++)
	{
		pUserItem = m_UserItemActive[i];
		assert(pUserItem != NULL);
		assert(pUserItem->IsActive() == true);
		if(pUserItem->GetUserData()->dwGameID == dwGameID) return pUserItem;
	}
	return NULL;
}

//查找用户
IClientUserItem* CClientUserManager::SearchUserByPhone(const char* phone)
{
	if(!strlen(phone)) return NULL;
	CClientUserItem*  pUserItem = NULL;
	for(unsigned int i=0;i<m_UserItemActive.size();i++)
	{
		pUserItem = m_UserItemActive[i];
		assert(pUserItem != NULL);
		assert(pUserItem->IsActive() == true);
		if(strcmp(pUserItem->GetUserData()->szPhone,phone) == 0) return pUserItem;
	}
	return NULL;
}

