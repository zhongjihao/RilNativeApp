/*************************************************************************
    > File Name: CClientUserItem.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 01 Jul 2017 03:49:53 PM CST
 ************************************************************************/

#ifndef CCLIENT_USERITEM_H
#define CCLIENT_USERITEM_H

#include "IClientUserItem.h"

//用户信息类
class CClientUserItem : public IClientUserItem
{
private:
	//变量定义
	tagUserData*     m_UserData;                   //用户信息
    bool             m_bActive;                    //激活有效
private:
	//清理数据
	void CleanData();
public:
	//构造函数
	CClientUserItem();
	//拷贝构造
	CClientUserItem(const CClientUserItem&);
	//赋值运算符重载
	CClientUserItem& operator=(const CClientUserItem&);
	//析构函数
	~CClientUserItem();
public:
	bool IsActive();
    void SetActive(bool bActive);
    int GetUserID();
	const char* GetUserName();
	int GetUserPlayCount();
	tagUserData* GetUserData();
	void SetUserScore(tagUserScore* pUserScore);
	void SetUserStatus(tagUserStatus* pUserStatus);
};


#endif

