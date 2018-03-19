/*************************************************************************
    > File Name: IClientUserItem.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 01 Jul 2017 03:27:08 PM CST
 ************************************************************************/

#ifndef ICLIENT_USERITEM_H
#define ICLIENT_USERITEM_H

#include "GameData.h"

//用户信息
class IClientUserItem
{
public:
	virtual ~IClientUserItem(){}
	//访问判断
	virtual bool IsActive() = 0;
	virtual void SetActive(bool bActive) = 0;
	//用户 I D
	virtual int GetUserID() = 0;

	//用户名字
	virtual const char* GetUserName() = 0;
	//游戏局数
	virtual int GetUserPlayCount() = 0;
	//获取用户
	virtual tagUserData* GetUserData() = 0;

	//设置积分
	virtual void SetUserScore(tagUserScore* pUserScore)  =  0;
	//设置状态
	virtual void SetUserStatus(tagUserStatus* pUserStatus) = 0;

};

#endif

