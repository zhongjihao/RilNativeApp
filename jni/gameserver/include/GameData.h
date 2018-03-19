//
// Created by SPREADTRUM\jihao.zhong on 17-6-6.
//

#ifndef CHATFRAME_GAMEDATA_H
#define CHATFRAME_GAMEDATA_H

#include "LoginData.h"

#define MAX_CHAIR_NORMAL				8

#define SOCKET_VER						0x66
#define SOCKET_BUFFER					8192
#define SOCKET_PACKET					(SOCKET_BUFFER-sizeof(CMD_Head)-2*sizeof(unsigned long))


#define MDM_KN_COMMAND					0
#define SUB_KN_DETECT_SOCKET			1


struct CMD_Info{
    unsigned char						cbVersion;
    unsigned char						cbCheckCode;
    unsigned short						wPacketSize;
};


struct CMD_Command{
    unsigned short						wMainCmdID;
    unsigned short						wSubCmdID;
};

//网络包头
struct CMD_Head{
    CMD_Info							CmdInfo;
    CMD_Command							CommandInfo;
};

struct CMD_Buffer{
    CMD_Head							Head;
    unsigned char						cbBuffer[SOCKET_PACKET];
};


struct CMD_KN_DetectSocket{
    unsigned long						dwSendTickCount;
    unsigned long						dwRecvTickCount;
};


#define IPC_PACKAGE						4096

struct IPC_Head{
    unsigned short						wVersion;
    unsigned short						wDataSize;
    unsigned short						wMainCmdID;
    unsigned short						wSubCmdID;
};


struct IPC_Buffer{
    IPC_Head							Head;
    unsigned char						cbBuffer[IPC_PACKAGE];
};

#define TYPE_LEN						32
#define KIND_LEN						32
#define STATION_LEN						32
#define SERVER_LEN						32
#define MODULE_LEN						32


#define GENDER_NULL						0
#define GENDER_BOY						1
#define GENDER_GIRL						2


#define GAME_GENRE_SCORE				0x0001                          //积分场
#define GAME_GENRE_GOLD					0x0002                          //金币场
#define GAME_GENRE_MATCH				0x0004                          //比赛场
#define GAME_GENRE_EDUCATE				0x0008

//游戏类型结构
struct tagGameType{
    unsigned short						wSortID;                        //排序号码
    unsigned short						wTypeID;                        //种类号码
    char								szTypeName[TYPE_LEN];          	//种类名字
};

//游戏名称结构
struct tagGameKind{
    unsigned short						wSortID;                       	//排序号码
    unsigned short						wTypeID;                        //类型号码
    unsigned short						wKindID;                        //名称号码
    unsigned long					    dwMaxVersion;                   //最新版本
    unsigned long						dwOnLineCount;                 	//在线数目
    char								szKindName[KIND_LEN];           //游戏名字
    char								szProcessName[MODULE_LEN];      //进程名字
};

//游戏站点结构
struct tagGameStation{
    unsigned short						wSortID;                       //排序号码
    unsigned short						wKindID;                       //名称号码
    unsigned short 						wJoinID;                       //挂接号码
    unsigned short						wStationID;                    //站点号码
    char								szStationName[STATION_LEN];    //站点名称
};

//游戏房间列表结构
struct tagGameServer{
    unsigned short                      wServerType;                  //房间类型
    unsigned short						wSortID;                      //排序号码
    unsigned short						wKindID;                      //名称号码
    unsigned short						wServerID;                    //房间号码
    unsigned short						wStationID;                   //站点号码
    unsigned short						wServerPort;                  //房间端口
    unsigned long						dwServerAddr;                 //房间地址
    unsigned long						dwOnLineCount;                //在线人数
    char								szServerName[SERVER_LEN];     //房间名称
};

//列表配置
struct CMD_GP_ListConfig {
    char bShowOnLineCount;				 //显示人数
};

struct tagLevelItem{
    long								lLevelScore;
    unsigned short						szLevelName[16];
};

//用户积分信息
struct tagUserScore{
    long								lScore;                    	//用户分数
    long								lGameGold;                  //游戏金币
    long								lInsureScore;               //存储金币
    long								lWinCount;                  //胜利盘数
    long								lLostCount;                 //失败盘数
    long								lDrawCount;                 //和局盘数
    long								lFleeCount;                 //断线数目
    long								lExperience;                //用户经验
};

//用户状态信息
struct tagUserStatus{
    unsigned short					    wTableID;            	    //桌子号码
    unsigned short						wChairID;                   //椅子位置
    unsigned char						cbUserStatus;               //用户状态
};

//用户基本信息结构
struct tagUserInfoHead {
    //用户属性
    short		    wFaceID;							//头像索引
    int			    dwUserID;							//用户 I D
    int			    dwGameID;							//游戏 I D
    int			    dwGroupID;							//社团索引
    int			    dwUserRight;						//用户等级
    int			    lLoveliness;						//用户魅力
    int			    dwMasterRight;						//管理权限

    //用户属性
    unsigned char	cbGender;							//用户性别
    unsigned char	cbMemberOrder;						//会员等级
    unsigned char	cbMasterOrder;						//管理等级

    //用户状态
    short		    wTableID;							//桌子号码
    short	        wChairID;							//椅子位置
    unsigned char	cbUserStatus;						//用户状态

    //用户积分
    tagUserScore	UserScoreInfo;                     //积分信息

    //扩展信息
    int			    dwCustomFaceVer;					//上传头像
};

//用户信息结构
struct tagUserData{
    //用户属性
    unsigned short						wFaceID;                              	//头像索引
    unsigned long						dwCustomFaceVer;                        //上传头像
    unsigned long						dwUserID;                               //用户 I D
    unsigned long						dwGroupID;                              //社团索引
    unsigned long						dwGameID;                               //游戏I D
    unsigned long						dwUserRight;                            //用户等级
    long								lLoveliness;                            //用户魅力
    unsigned long						dwMasterRight;                          //管理权限
    char								szName[NAME_LEN];                       //用户名字
    char								szPassword[PASS_LEN];                   //用户密码
    char								szPhone[PHONE_LEN];                     //用户手机号

    //用户属性
    char						        cbGender;                               //用户性别
    char						        cbMemberOrder;                          //会员等级
    char						        cbMasterOrder;                          //管理等级

    //用户积分
    long								lInsureScore;                          	//消费金币
    long								lGameGold;                              //游戏金币
    long								lScore;                                 //用户分数
    long								lWinCount;                              //胜利盘数
    long								lLostCount;                             //失败盘数
    long								lDrawCount;                             //和局盘数
    long								lFleeCount;                             //断线数目
    long								lExperience;                            //用户经验

    //用户状态
    unsigned short		                wMatchRank;							     //排名
    unsigned int			            lUsableScore;						     //可用积分
    unsigned short						wTableID;                                //桌子号码
    unsigned short						wChairID;                                //椅子位置
    char						        cbUserStatus;                            //用户状态
    unsigned char			            cbIsOut;							     //是否出局
};

//数据描述
struct tagDataDescribe{
    short wDataSize;                   //数据大小
    short wDataDescribe;               //数据类型
};

struct tagOptionBuffer{
    unsigned char						cbBufferLen;
    unsigned char						cbOptionBuf[32];
};

#endif //CHATFRAME_GAMEDATA_H
