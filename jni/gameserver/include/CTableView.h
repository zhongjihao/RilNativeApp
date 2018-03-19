//
// Created by SPREADTRUM\jihao.zhong on 17-8-5.
//

#ifndef CHATFRAME_CTABLEVIEW_H
#define CHATFRAME_CTABLEVIEW_H


#include "CClientUserItem.h"
#include "IClientUserItem.h"
#include <string.h>
#include <vector>

//游戏桌子属性
struct tagTableState
{
    bool							    bPass;								//密码标志
    bool							    bPlaying;							//游戏标志
    short							    wTableID;							//桌子号码
    short							    wSitCount;							//在坐人数
    short							    wChairCount;						//椅子数目
    int								    dwTableMasterID;					//台主玩家
    char							    szTableNote[64];		            //桌子备注
    std::vector<IClientUserItem*>	    pIUserItem;	                        //用户信息

    tagTableState();
    ~tagTableState();
    tagTableState(tagTableState& tableState);
    tagTableState& operator=(const tagTableState& tableState);
};

//游戏桌基础类
class CTableView
{
private:
    //变量定义
    bool					    m_bClickDown;						        //是否按下
    bool					    m_bFocusFrame;						        //框架标志
    short					    m_wHoverChairID;					        //盘旋椅子
    struct tagTableState*	    m_pTableState;         	                    //桌子属性
public:
    CTableView();
    ~CTableView();
    //初始化函数
    void InitTableView(short wTableID, short wChairCount);
    //设置用户信息
    bool SetUserInfo(short wChairID, IClientUserItem* pIUserItem);
    //删除指定位置的玩家
    bool DeleteUserItem(short wChairID);
    //设置点击按下
    void SetMouseDown(bool bClickDown);
    //设置焦点框架
    void SetFocusFrame(bool bFocusFrame);
    //设置盘旋位置
    void SetHoverChairID(short wHoverChairID);
    //设置游戏标志
    void SetPlayFlag(bool bPlaying);
    //设置密码标志
    void SetPassFlag(bool bPass);
    //设置备注
    void SetTableNote(const char* pszNote);
    //获取用户信息
    IClientUserItem* GetUserInfo(unsigned short wChairID)const;
    //获取空椅子数
    short GetNullChairCount()const;
    //获取空椅子编号
    short GetNullChairID()const;
    //查询焦点框架
    bool QueryFocusFrame()const;
    //查询盘旋位置
    short QueryHoverChairID()const;
    //查询游戏标志
    bool QueryPlayFlag()const;
    //查询密码标志
    bool QueryPassFlag()const;
    //获取桌子属性
    tagTableState* GetTableAttrib()const;
};

#endif //CHATFRAME_CTABLEVIEW_H
