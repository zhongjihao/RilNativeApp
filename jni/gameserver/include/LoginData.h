//
// Created by SPREADTRUM\jihao.zhong on 17-6-19.
//

#ifndef CHATFRAME_LOGINDATA_H
#define CHATFRAME_LOGINDATA_H

/////////////////////大厅登陆指令////////////////////////////////////////////////////////////////////

#define  MDM_GP_LOGON                1		 //大厅登陆

#define  SUB_GP_LOGON_ACCOUNTS		 1		 //帐号登录
#define  SUB_GP_REGISTER_ACCOUNTS	 2		 //注册帐号
#define  SUB_GP_UNREGISTER_ACCOUNTS	 3		 //注销帐号
#define  SUB_GP_EXIT_PLAZA           4       //退出大厅

#define SUB_GP_LOGON_SUCCESS		 100		     //登陆成功
#define SUB_GP_LOGON_ERROR		     101		     //登陆失败
#define SUB_GP_LOGON_FINISH		     102		     //登陆完成
#define SUB_GP_REGISTER_SUCCESS		 103		     //注册成功
#define SUB_GP_REGISTER_FAIL         104             //注册失败
#define SUB_GP_UNREGISTER_RESPONSE   105             //注销响应

//游戏列表命令码
#define MDM_GP_SERVER_LIST		      2		     //列表信息

#define SUB_GP_LIST_TYPE		     100	     //类型列表
#define SUB_GP_LIST_KIND			 101	     //种类列表
#define SUB_GP_LIST_STATION		     102		 //站点列表
#define SUB_GP_LIST_SERVER	         103	     //房间列表
#define SUB_GP_LIST_FINISH		     104		 //发送完成
#define SUB_GP_LIST_CONFIG		     105		 //列表配置

//基础信息
#define	DTP_USER_ACCOUNTS	 1		//用户帐号
#define	DTP_USER_PASS		 2		//用户密码
#define	DTP_UNDER_WRITE		 3		//个性签名
#define	DTP_USER_PHONE		 4		//用户手机号

//扩展信息
#define	 DTP_USER_GROUP_NAME      301		//社团名字

#define NAME_LEN						32
#define PASS_LEN						33
#define PHONE_LEN						12
#define GROUP_LEN						32
#define UNDER_WRITE_LEN					32
#define STATION_PAGE_LEN                32

//事件类型
#define REGISTER_MSG					1
#define LOGON_MSG						2
#define UNREGISTER_MSG				    3
#define EXITPLAZA                       4
#define LOGON_ROOM                      5

//系统信息
#define DTP_COMPUTER_ID	   	   1000		//机器序列
#define DTP_STATION_PAGE  	   1001		//站点页面

#define PLAZA_VERSION          131073    //大厅版本
#define PROCESS_VERSION        34359738  //进程版本

//登陆成功
struct CMD_GP_LogonSuccess
{
    short						    wFaceID;						//头像索引
    unsigned char                   cbGender;						//用户性别
    unsigned char					cbMember;						//会员等级
    int								dwUserID;						//用户 I D
    int								dwGameID;						//游戏 I D
    int								dwExperience;					//用户经验

    int 							lLoveliness;					//用户魅力
    int 							lGoldCoin;						//用户金币
    int 							lScore;						    //用户积分

    //扩展信息
    int								dwCustomFaceVer;				//头像版本

};

//注册成功
struct CMD_GP_RegisterSuccess
{
    short						    wFaceID;						//头像索引
    unsigned char                   cbGender;						//用户性别
    unsigned char					cbMember;						//会员等级
    int								dwUserID;						//用户 I D
    int								dwGameID;						//游戏 I D
    int								dwExperience;					//用户经验

    int 							lLoveliness;					//用户魅力
    int 							lGoldCoin;						//用户金币
    int 							lScore;						    //用户积分

    char                            szAcccount[NAME_LEN];           //账号
    char                            szPassword[PASS_LEN];           //密码
    char                            szPhone[PHONE_LEN];             //手机号

};

//登陆失败
struct CMD_GP_LogonError
{
    int		lErrorCode;						    //错误代码
    char    szErrorDescribe[128];               //错误消息
};

//注销响应
struct CMD_GP_UnRegisterResponse
{
    int retCode;                               //注销响应码   0---成功   1-----失败
    char szDescribe[128];                      //响应描述
};

//账号信息
struct LoginAccount
{
    int		         dwPlazaVersion;
    unsigned  long   dwUserId;
    char             szAccounts[NAME_LEN];
    char             szPassWord[PASS_LEN];
};

//机器序列码
struct tagClientSerial
{
    unsigned long	 dwSystemVer;       //系统版本
    unsigned long	 dwComputerID[3];   //机器ID
};


//////////////////////////房间登陆指令////////////////////////////////////////////////////////////////////

#define   MDM_GR_LOGON			                   1						//房间登录

#define SUB_GR_LOGON_USERID		                   1						//I D 登录

#define SUB_GR_LOGON_SUCCESS                      100					    //登录成功
#define SUB_GR_LOGON_ERROR		                  101					    //登录失败
#define SUB_GR_LOGON_FINISH		                  102						//登录完成

//用户数据包定义
#define MDM_GR_USER				                   2						//用户信息
#define SUB_GR_USER_SIT_REQ		                   1				        //坐下请求
#define SUB_GR_USER_STANDUP_REQ	                   2						//起立请求
#define SUB_GR_USER_LEFT_ROOM_REQ                  3  					    //离开房间
#define SUB_GR_USER_COME		                  100					    //用户进入
#define SUB_GR_USER_STATUS		                  101					    //用户状态
#define SUB_GR_USER_SCORE		                  102					    //用户分数
#define SUB_GR_SIT_FAILED		                  103				        //坐下失败
#define SUB_GR_USER_RIGHT		                  104					    //用户权限
#define SUB_GR_MEMBER_ORDER		                  105					    //会员等级
#define SUB_GR_USER_CHAT		                  200				        //聊天消息
#define SUB_GR_USER_WISPER		                  201			            //私语消息
#define SUB_GR_USER_INVITE		                  300					    //邀请消息

//配置信息数据包
#define MDM_GR_INFO				                   3						//配置信息
#define SUB_GR_SERVER_INFO		                   100					    //房间配置
#define SUB_GR_COLUMN_INFO		                   101				        //列表配置
#define SUB_GR_CONFIG_FINISH	                   102				        //配置完成

//房间状态数据包
#define MDM_GR_STATUS			                    4						//状态信息
#define SUB_GR_TABLE_INFO		                   100					    //桌子信息
#define SUB_GR_TABLE_STATUS		                   101					    //桌子状态

//系统数据包
#define MDM_GR_SYSTEM			                   10						//系统信息
#define SUB_GR_MESSAGE			                   100				        //系统消息

//房间数据包
#define MDM_GR_SERVER_INFO		                   11					    //房间信息
#define SUB_GR_ONLINE_COUNT_INFO	               100					    //在线信息

//网络命令码
#define MDM_GF_GAME				                   100					    //游戏消息
#define MDM_GF_FRAME			                   101					    //框架消息
#define	MDM_GF_PRESENT			                   102						//礼物消息
#define MDM_GF_VIDEO			                   200						//视频通道

//参数定义
#define INVALID_CHAIR		                       0xFFFF				    //无效椅子
#define INVALID_TABLE	                           0xFFFF					//无效桌子
#define INVALID_USER_ID		                       0						//无效用户ID

//用户状态定义
#define US_NULL		                               0x00				         //没有状态
#define US_FREE		                               0x01						 //站立状态
#define US_SIT		                               0x02						 //坐下状态
#define US_READY	                               0x03						 //同意状态
#define US_LOOKON	                               0x04						 //旁观状态
#define US_PLAY		                               0x05						 //游戏状态
#define US_OFFLINE	                               0x06						 //断线状态

#define IPC_SUB_USER_STATUS	                        2		                 //用户状态
#define IPC_SUB_GAME_START	                        4		                 //游戏开始
#define IPC_SUB_GAME_FINISH	                        5		                 //游戏结束
#define IPC_MAIN_USER		                        3		                 //用户信息

//房间服务状态
#define ServiceStatus_Null               0					            //没有状态
#define ServiceStatus_RecvConfigInfo     1		                        //接收配置
#define ServiceStatus_RecvRoomInfo       2	                            //接收信息
#define ServiceStatus_Serviceing         3		                        //服务状态
#define ServiceStatus_NetShutDown        4	                            //网络中断

//房间ID登录
struct CMD_GR_LogonByUserID
{
    int				dwPlazaVersion;						        //广场版本
    int				dwProcessVersion;					        //进程版本
    int 			dwUserID;							        //用户ID
    char			szPassWord[33];			                    //登录密码
};

//列表项描述结构
struct tagColumnItem
{
    char			cbDataDescribe;						//字段类型
    char	        szColumnName[16];		            //列表名字
    short			wColumnWidth;						//列表宽度
    short 			nFormat;							//水平对齐方式,垂直全部居中
};

//列表描述信息
struct CMD_GR_ColumnInfo
{
    short			  wColumnCount;						             //列表数目
    tagColumnItem	  pColumnItem[5];                                //列表描述
};

//登录失败
struct CMD_GR_LogonError
{
    int 	 lErrorCode;							            //错误代码
	char	 szErrorDescribe[128];			                    //错误消息
};

//用户请求
struct CMD_GR_UserReq
{
    long						dwUserID;							//用户 ID
    short						wTableID;							//桌子位置
    short					    wChairID;							//椅子位置
};

//用户状态
struct CMD_GR_UserStatus
{
    int							dwUserID;							//用户 ID
    short						wTableID;							//桌子位置
    short					    wChairID;							//椅子位置
    unsigned char				cbUserStatus;						//用户状态
};

//用户状态
struct IPC_UserStatus
{
    int								dwUserID;						//用户ID
    short							wNetDelay;						//网络延时
    unsigned char					cbUserStatus;					//用户状态
};

//游戏房间信息
struct CMD_GR_ServerInfo
{
    short					    wKindID;							//类型 I D
    short						wTableCount;						//桌子数目
    short						wChairCount;						//椅子数目
    int 						dwVideoAddr;						//视频地址

    //扩展配置
    short						wGameGenre;							//游戏类型
    unsigned char			    cbHideUserInfo;						//隐藏信息
};

//桌子状态结构
struct tagTableStatus
{
    unsigned char			bTableLock;							//锁定状态
    unsigned char			bPlayStatus;						//游戏状态
};

//桌子状态数组
struct CMD_GR_TableInfo
{
    short							wTableCount;						  //桌子数目
    tagTableStatus				    pTableStatus[10];                     //状态数组
};

//桌子状态信息
struct CMD_GR_TableStatus
{
    short				            wTableID;							//桌子号码
    unsigned char					bTableLock;							//锁定状态
    unsigned char					bPlayStatus;						//游戏状态
};


#endif //CHATFRAME_LOGINDATA_H
