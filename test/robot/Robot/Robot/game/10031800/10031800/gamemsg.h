#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#define PLAY_COUNT						    180						     //游戏人数 
#define TIME_ONE_SECOND				        1000						 //定时器一秒
#define MAX_CARDS                           416                          //最大牌数
#define MAX_COUNT                           2                            //发牌最大数
#define BANKERLISTLENGTH                    20                           //最大上庄列表
#define NOSITLENGTH                         180                          //无座玩家列表长度
#define CHOUMA_COUNT						5							 //筹码个数
#define AREA_COUNT							3							 //下注区域个数
#define SEAT_COUNT							7							 //座位数
#define AREA_TOTAL_COUNT					3 * 5						 //区域个数 * 筹码个数
//下注错误码
#define SUCCESS                             0                            //成功
#define FAIL                                1                            //失败
#define NOTENOUGH                           2                            //钱不够
#define COINLIMIT                           3                            //超过限制
#define WRONGAREA                           4                            //区域限制
#define NONOTE                              5                            //没有下注
#define CONFIRMED							6							 //已确定下注
//新增游戏状态，从12开始
#define     GAME_STATUS_LOOK_STATE          12                           //开始状态
#define     GAME_STATUS_SHUFFLE             13                           //洗牌状态
#define     GAME_STATUS_SEND_CARD           14                           //发牌状态
#define     GAME_STATUS_USER_NOTE           15                           //下注状态
#define     GAME_STATUS_OPEN                16                           //开牌状态
#define     GAME_STATUS_RESULT              17                           //结算状态
//新增录像记录，从 0x30开始
//游戏定时器 1-50 
#define     TIMER_READY                     1                            //准备时间
#define		TIME_SHUFFLE					2							 //洗牌
#define		TIME_SEND_CARD					3							 //发牌
#define		TIME_USER_NOTE					4							 //玩家下注
#define		TIME_WAIT_NEX					5							 //等待下局开始
#define		TIME_CARD_OPEN     				6							 //开牌

//游戏消息
//server to  client 
#define STC_SEND_USER_AGREE				    1000						 //玩家同意消息
#define STC_SEND_SEND_CARD				    1001						 //发牌消息
#define STC_SEND_SHUFFLE				    1002						 //洗牌消息
#define STC_SEND_MAKER						1003						 //庄家信息
#define STC_SEND_USER_NOTE				    1004						 //下注状态消息
#define STC_SEND_OPEN_RES				    1005						 //开牌结果消息
#define STC_SEND_SIT_RESULT				    1006						 //坐下结果消息
#define STC_SEND_GIVEUP_SIT_RES				1007						 //站起结果消息
#define STC_SEND_NO_SIT_COUNT				1008						 //无座人数消息
#define STC_SEND_GET_BANKER_RESULT          1009                         //上庄结果消息
#define STC_SEND_GIVEUP_BANKER_RESULT       1010                         //下庄结果消息
#define STC_SEND_NOSITLIST_RESULT           1011                         //无座列表结果消息
#define STC_SEND_LUTU_RESULT                1012                         //路图结果消息
#define STC_SEND_USER_NOTE_RESULT			1013						 //下注结果消息
#define STC_SEND_USER_REPEATNOTE_RESULT		1014						 //重复下注结果消息
#define STC_SEND_USER_CONFIRMNOTE_RESULT	1015						 //确认下注结果消息
#define STC_SEND_USER_CANCELNOTE_RESULT	    1016						 //取消下注结果消息
#define STC_SEND_MAKER_LIST					1017						 //发送上庄列表
#define STC_SEND_OPEN_CARD					1018						 //开牌消息
//发送共用消息
#define STC_UPDATE_USER_INFO			    1101						 //更新玩家信息	 
#define CTS_GET_GAME_CONFIG				    1201						 //申请获取游戏配置
#define CTS_UPDATE_USER_INFO			    1202						 //申请更新自己信息

//client to server
#define CTS_RECV_USER_NOTE				    2000						//收到玩家下注消息
#define CTS_RECV_USER_SIT                   2002                        //玩家坐下
#define CTS_RECV_USER_GIVEUP_SIT            2003                        //玩家放弃坐下
#define CTS_RECV_USER_GET_BANKER            2004                        //上庄情况
#define CTS_RECV_USER_GIVEUP_BANKER         2005                        //下庄情况
#define CTS_RECV_NOSIT_USER                 2006                        //请求无座玩家信息消息
#define CTS_RECV_LUTU                       2007                        //请求路图结果
#define CTS_RECV_REPEAT_XIAZHU              2008                        //重复下注
#define CTS_RECV_CONFIRM_XIAZHU             2009                        //确认下注
#define CTS_RECV_CANCEL_XIAZHU              2010                        //取消下注
#define CTS_RECV_MAKER_LIST_REQEST			2011						//玩家请求上庄列表
#pragma pack(push,1)

//玩家状态
enum USER_STATUS
{
	USER_SEE = 0,				//旁观
	USER_JOIN,					//参与游戏
	USER_REST,                  //休息
};

//区域类型
enum AREA_TYPE
{
	AREA_LONG = 0,				//龙
	AREA_HE,					//和
	AREA_HU,					//虎
};
//座位类型
enum SEAT_TYPE
{	
	SEAT_MAKER = 0,				//庄家
	SEAT_RICHER,				//大富豪
	SEAT_LUCKY_STAR,			//幸运星
	SEAT_1,						//座位1
	SEAT_2,						//座位2
	SEAT_3,						//座位3
	SEAT_4,						//座位4
};
//S-->C
//游戏消息结构体 
//游戏状态基本信息

//游戏中玩家信息
typedef struct Stu_GameUserItem
{
	unsigned int	player_id;						//玩家ID
	BYTE            chairid;                        //座位id
	BYTE            bsex;                           //性别(1男2女)
	char			player_nickname[60];			//玩家昵称
	int				player_icon_id;					//系统头像id
	bool			look_status;					//是否是旁观状态
	bool            bplaygame;                      //是否在游戏中
	bool			cut_status;						//是否是断线状态
	long long		total_score;					//玩家总分
	long long       servercount;                    //服务费
	
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;

//游戏开始阶段
typedef struct sc_game_station
{
	BYTE                chairid;                                    //玩家id
	BYTE                iuser_sit[SEAT_COUNT];						//ui坐下玩家
	long long           areamoney[AREA_COUNT];						//区域下注总金额
	BYTE                areacard[2];		                        //手牌
	BYTE	            nowstatus;			                        //游戏状态
	long long           user_money;									//当前玩家金币
	BYTE	            nowtime;				                    //当前状态剩余时间
	long long           selfmoney[AREA_COUNT];						//玩家区域下注金额
	long long           luckymoney[AREA_COUNT];						//幸运星下注
	int					area_tatal_chouma[AREA_TOTAL_COUNT];		//区域总的筹码个数 龙0-4  和5-9  虎10-14
	int					area_user_chouma[AREA_TOTAL_COUNT];			//玩家区域下注的筹码数 龙0-4  和5-9  虎10-14
	int					wall_count;									//牌墙数量
	GameUserItem        useritem[SEAT_COUNT];						//游戏中玩家信息
	int					watch_times;					            //旁观次数
	int                 chips[CHOUMA_COUNT];                        //筹码值
	bool				bBetLastGame;								//上一局是否有下注
	sc_game_station()
	{
		memset(this, 0, sizeof(sc_game_station));
	}
}game_station;

typedef struct sc_user_agree
{
	unsigned int	userid;
	unsigned char  chair_id;
	bool			bagree;
	sc_user_agree()
	{
		userid = 0;
		chair_id = 255;
		bagree = false;
	}
}user_agree;

//庄家信息
struct Stu_Maker_Info
{
	BYTE			chairid;		//庄家位置
	int				userid;			//庄家id
	long long		usermoney;		//庄家金币
	int				wall_length;	//牌墙长度  剩余牌个数
	Stu_Maker_Info()
	{
		memset(this, 0, sizeof(Stu_Maker_Info));
		chairid = 255;
	}
};

struct Stu_Status
{
	BYTE  status;                   //发送状态
	BYTE  bytime;                   //当前状态时间
	Stu_Status()
	{
		memset(this, 0, sizeof(Stu_Status));
	}
};

struct Stu_UserXiaZhuResult
{
	BYTE		chairid;			    //座位号
	BYTE		areaid;					//区域号
	BYTE		xiazhuresult;           //下注结果 
	long long   xiazhumoney;			//下注金额
	long long   userAreaTotalBet;		//玩家该区域总下注
	long long	areaTotalBet;			//区域所有玩家的总下注
	long long	usermoney;				//下注玩家金币
	bool		bsit;                   //是否坐下玩家
	Stu_UserXiaZhuResult()
	{
		memset(this, 0, sizeof(Stu_UserXiaZhuResult));
		chairid = 255;
	}
};

//用户申请上庄结果
struct Stu_UserGetBanker_Result
{
	BYTE		chairid;				//座位号
	long long   bring_coin;				//上庄金币数
	BYTE		result;					//上庄结果
};

//掉线后被强制下庄的玩家
struct Stu_Cut_Banker
{
	unsigned int		userid;          //玩家id
	long long			timecount;       //掉线时间（时间到清0）
	Stu_Cut_Banker()
	{
		memset(this, 0, sizeof(Stu_Cut_Banker));
	}
};

//发牌阶段
struct Stu_Send_Card
{
	BYTE	status;                 //发送状态
	BYTE	bytime;                 //当前状态时间
	BYTE	openid[2];              //开牌区域
	BYTE	cards[2];               //牌信息0:龙，1:虎
	Stu_Send_Card()
	{
		memset(this, 0, sizeof(Stu_Send_Card));
	}
};

//玩家分数变化
struct Stu_ChangeMoney
{
	BYTE				chairid;										//座位号
	BYTE				status;											//游戏状态
	BYTE				winarea;										//开牌区域
	long long			win_score;										//玩家输赢分  包括本金 去除服务费
	long long           user_score;										//玩家金币
	long long			seat_win_score[SEAT_COUNT];						//上座玩家输赢分 包括庄家幸运星大富豪  包括本金 去除服务费
	long long           seat_user_score[SEAT_COUNT];					//座位上玩家金币
	bool				bFlyCoin;										//无座列表玩家是否需要飞金币
	int					rest_time;										//休息时间	
	bool				bBet;											//该局当前玩家是否有下注
	Stu_ChangeMoney()
	{
		memset(this, 0, sizeof(Stu_ChangeMoney));
		chairid = 255;
	}
};

//玩家上座结果
struct Stu_Sit_Result
{
	BYTE		chairid;			        //座位号
	BYTE		result;                     //更迭结果
	BYTE		sitid;						//椅子号 3-6
	int			userid;						//上座玩家id
	long long   restmoney;					//玩家金币
	int			nositnum;                   //无座玩家数
	Stu_Sit_Result()
	{
		memset(this, 0, sizeof(Stu_Sit_Result));
		chairid = 255;
	}
};

//玩家站起结果
struct Stu_UP_Result
{
	BYTE		chairid;			        //玩家位置
	BYTE		sitid;                      //椅子号 3-6
	BYTE		result;                     //更迭结果 0失败 1成功 2金币不足自动下座
	int			userid;						//下座玩家id
	int			nositnum;                   //无座玩家数
	Stu_UP_Result()
	{
		memset(this, 0, sizeof(Stu_UP_Result));
		chairid = 255;
	}
};

//请求无座玩家结果
struct Stu_NoSit_Userinfo_Result
{
	unsigned int    userid[NOSITLENGTH];		//玩家id
	long long		money[NOSITLENGTH];			//金币
	int				win_num[NOSITLENGTH];		//获胜局数
	Stu_NoSit_Userinfo_Result()
	{
		memset(this, 0, sizeof(Stu_NoSit_Userinfo_Result));
	}
};

//请求路图
struct Stu_LuTu_Result
{
	BYTE brecord[100];
	Stu_LuTu_Result()
	{
		memset(this, 255, sizeof(Stu_LuTu_Result));
	}
};

//玩家重复下注结果
struct Stu_Repeat_XiaZhu_Result
{
	BYTE			chairid;
	long long		area_total_bet[AREA_COUNT];							//区域所有玩家总下注
	long long		area_user_bet[AREA_COUNT];							//区域玩家下注
	int				area_user_chouma[AREA_TOTAL_COUNT];					//玩家各区域对应的下注筹码个数  龙0-4  和5-9  虎10-14
	long long		user_coin;											//玩家金币
	BYTE			result;												//0成功 1金币不足 2庄家不能下注
	bool			bsit;
	Stu_Repeat_XiaZhu_Result()
	{
		memset(this, 0, sizeof(Stu_Repeat_XiaZhu_Result));
	}
};

//玩家确认下注结果
struct Stu_Confirm_XiaZhu_Res
{
	BYTE			chairid;
	unsigned int    userid;
	bool			 result;
	Stu_Confirm_XiaZhu_Res()
	{
		chairid = 255;
		userid = 0; 
		result = false;
	}
};

//玩家取消下注结果
struct Stu_Cancel_XiaZhu_Res
{
	BYTE			chairid;
	unsigned int    userid;
	long long		area_total_bet[AREA_COUNT];					//取消下注后区域所有玩家的总下注
	int				area_user_chouma[AREA_TOTAL_COUNT];			//取消下注的玩家下注的对应的筹码个数
	long long		user_score;									//取消下注后的玩家身上金币
	bool			bsit;										//是否是上座玩家 
	BYTE			result;
	Stu_Cancel_XiaZhu_Res()
	{
		chairid = 255;
		userid = 0;
		result = 0;
	}
};
//C-->S
//消息结构体
//用户下注
struct Stu_UserXiaZhu
{
	BYTE   chairid;			       //座位号
	BYTE   areaid;                 //区域号
	INT    xiazhumoney;            //下注金额

	Stu_UserXiaZhu()
	{
		memset(this, 0, sizeof(Stu_UserXiaZhu));
		chairid = 255;
	}
};

//用户申请上庄
struct Stu_UserGetBanker
{
	BYTE		chairid;			        //座位号
	long long   bring_coin;			        //上庄金币数
	long long	maker_time;					//上庄时间
	Stu_UserGetBanker()
	{
		memset(this, 0, sizeof(Stu_UserGetBanker));
		chairid = 255;
	}
};

//发送庄家列表
struct Stu_Send_MakerLIst
{
	Stu_UserGetBanker	makerlist[BANKERLISTLENGTH];
	int					maker_num;					//庄家坐庄局数
};

//用户申请下庄
struct Stu_UserGiveupBanker
{
	int				userid;
	BYTE			chairid;
	BYTE            result;                    //下庄结果 0成功 1失败 2金币不足自动下庄
	Stu_UserGiveupBanker()
	{
		userid = 0;
		chairid = 255;
		result = 0;
	}
};
//用户申请开牌
struct Stu_UserOpen
{
	BYTE    chairid;			       //座位号
	BYTE    areaid;                    //区域id 龙：0， 虎：1
	Stu_UserOpen()
	{
		chairid = 255;
	}
};

//无座玩家总数
struct Stu_NoSit_Count
{
	BYTE     bycount;   //无座玩家总数
	Stu_NoSit_Count()
	{
		memset(this, 0, sizeof(Stu_NoSit_Count));
	}
};

//请求无座玩家
struct Stu_NoSit_Userinfo
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_NoSit_Userinfo()
	{
		chairid = 255;
		userid = 0;
	}
};

//请求路图
struct Stu_LuTu
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_LuTu()
	{
		chairid = 255;
		userid = 0;
	}
};

//玩家申请坐下
struct Stu_Sit
{
	BYTE    chairid;			       //座位号
	BYTE    sitid;                     //椅子号
	BYTE    result;                    //坐下结果
	Stu_Sit()
	{
		memset(this, 0, sizeof(Stu_Sit));
		chairid = 255;
	}
};

//玩家申请站起
struct Stu_GiveUp_Sit
{
	BYTE    chairid;
	BYTE    sitid;
	Stu_GiveUp_Sit()
	{
		memset(this, 0, sizeof(Stu_GiveUp_Sit));
		chairid = 255;
	}
};

//玩家重复下注
struct Stu_Repeat_XiaZhu
{
	BYTE			chairid;
	unsigned int    userid;
	bool    bnote;
	Stu_Repeat_XiaZhu()
	{
		chairid = 255;
		userid = 0;
		bnote = false;
	}
};

//玩家确认下注
struct Stu_Confirm_XiaZhu
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_Confirm_XiaZhu()
	{
		chairid = 255;
		userid = 0;
	}
};

//玩家取消下注
struct Stu_Cancel_XiaZhu
{
	BYTE			chairid;
	int				userid;
	Stu_Cancel_XiaZhu()
	{
		chairid = 255;
		userid = 0;
	}
};
//玩家请求上庄列表
struct Stu_View_MakerList
{
	BYTE			chair_id;
	Stu_View_MakerList()
	{
		chair_id = 255;
	}
};
#pragma pack(pop)

#endif