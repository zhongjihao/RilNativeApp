/*************************************************************************
    > File Name: IClientUserManager.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Sat 01 Jul 2017 03:15:38 PM CST
 ************************************************************************/

#ifndef ICLIENT_USER_MANAGER_H
#define ICLIENT_USER_MANAGER_H

#include "IClientUserItem.h"

//用户管理接口
class IClientUserManager
{
public:
	virtual ~IClientUserManager(){}
	//增加用户
	virtual IClientUserItem* ActiveUserItem(tagUserData* UserData) = 0;
	//添加用户
	virtual void ActiveUserItem(IClientUserItem* pIUserItem) = 0;
	//删除用户
	virtual bool DeleteUserItem(IClientUserItem* pIUserItem) = 0;
	//更新用户
	virtual bool UpdateUserItem(IClientUserItem* pIUserItem) = 0;
	//更新积分
	virtual bool UpdateUserItemScore(IClientUserItem* pIUserItem, tagUserScore* pUserScore) = 0;
	//更新状态
	virtual bool UpdateUserItemStatus(IClientUserItem* pIUserItem, tagUserStatus* pUserStatus) = 0;
	//获取人数
	virtual int GetOnLineCount() = 0;
    //枚举用户
	virtual IClientUserItem* EnumUserItem(unsigned short wEnumIndex) = 0;
	//查找用户
	virtual IClientUserItem* SearchUserByUserID(unsigned int dwUserID) = 0;
    //查找用户
	virtual  IClientUserItem* SearchUserByPhone(const char* phone) = 0;
	//查找用户
	virtual IClientUserItem* SearchUserByGameID(unsigned int dwGameID) = 0;
	//释放函数
	virtual void Release() = 0;
};

#endif

