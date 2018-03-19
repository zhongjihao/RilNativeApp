//
// Created by SPREADTRUM\jihao.zhong on 17-8-5.
//

#ifndef CHATFRAME_TABLE_FRAME_H
#define CHATFRAME_TABLE_FRAME_H

#include <vector>
#include "CTableView.h"

//游戏桌子框架
class CTableFrame
{
private:
    //状态信息
    short					    m_wDownChairID;						//位置号码
    short						m_wDownTableID;						//按下号码
    short						m_wFrameTableID;					//边框号码

    //设置变量
    bool                        m_bShowUser;						//是否显示
    bool						m_bHideInfo;						//隐藏信息
    short						m_wTableCount;						//游戏桌数
    short						m_wChairCount;						//椅子数目
    std::vector<CTableView*>    m_pTableViewArray;                  //桌子数组

public:
    CTableFrame();
    ~CTableFrame();
    //配置函数
    bool InitTableFrame(short wTableCount, short wChairCount, bool bHideInfo);
    //销毁函数
    void DestroyTableFrame();
    //获取游戏桌
    CTableView* GetTableArrayPtr(short wTableID)const;
    //设置显示
    bool ShowUserInfo(bool bShowUser);
    //获取桌子数目
    short GetTableCount()const;
    //获取椅子数目
    short GetChairCount()const;
    //设置用户信息
    bool SetUserInfo(short wTableID, short wChairID, IClientUserItem* pIUserItem);
    //设置游戏标志
    void SetPlayFlag(short wTableID, bool bPlaying);
    //设置密码标志
    void SetPassFlag(short wTableID, bool bPass);
    //获取用户信息
    IClientUserItem* GetUserInfo(short wTableID, short wChairID)const;
    //获取空椅子数
    short GetNullChairCount(short wTableID)const;
    //获取空椅子编号
    short GetNullChairID(short wTableID)const;
    //查询游戏标志
    bool QueryPlayFlag(short wTableID)const;
    //查询密码标志
    bool QueryPassFlag(short wTableID)const;
    //获取桌子属性
    const tagTableState* GetTableAttrib(short wTableID)const;

};

#endif //CHATFRAME_TABLE_FRAME_H
