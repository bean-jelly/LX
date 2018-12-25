#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_ 
#include <windows.h>
#include <stdarg.h> 
#include "basictable\GameRoomMsg.h"

#define PLAY_COUNT						3						             // 游戏人数 

//游戏整体状态
#define  GS_GAME_LOOK_STATION       1           
#define  GS_GAME_PLAYING            2

//游戏状态定义
#define  GS_WAIT_SETGAME			0			    //等待东家设置状态
#define  GS_WAIT_ARGEE				1				//等待同意设置
#define  GS_SEND_CARD				20				//发牌状态
#define  GS_WAIT_BACK				21				//等待扣压底牌
#define  GS_PLAY_GAME				22				//游戏中状态
#define  GS_WAIT_NEXT				23				//等待下一盘开始 

//游戏数据定义
#define  ALL_CARD_COUNT             54             //最大牌数
#define  HAND_CARD_MAX				20             //最大牌数据
#define  OTHER_CARD_COUNT           17             //其他人发的牌的张数
#define  BACK_CARD_COUNT            3		       //底牌
#define	 ERROR_PLAYER_POS           0XFF           //玩家错误位置 
#define  HAND_CARD_PIINT_TYPE		18             //牌点数种类

//游戏中的小状态
enum  EnumNotifyFlag : BYTE
{
	STATUS_FLAG_GAME_NORMAL = 0,       //通常状态
	STATUS_FLAG_GAME_BEGIN = 1,		//开始
	STATUS_FLAG_SEND_CARD = 2,       //玩家发牌
	STATUS_FLAG_CALL_SCORE = 3,		//叫分
	STATUS_FLAG_ROB_NT = 4,		//抢地主
	STATUS_FLAG_ADD_DOUBLE = 5,		//加倍
	STATUS_FLAG_SHOW_CARD = 6,       //明牌
	STATUS_FLAG_NT_ADD_DOUBLE = 7,       //地主加倍
	STATUS_FLAG_OUT_CARD = 8,		//开始出牌
	STATUS_FLAG_GAME_FINISH = 9,       //游戏结束
	STATUS_FLAG_CHANGE_CARD = 10,		//换三张
	STATUS_FLAG_CUT_LIST = 11,      //切牌
	STATUS_FLAG_SHOW_FIRST = 12,    //翻首张
	OPERATE_FLAG_AUTO = 20       //托管操作
};


enum GAME_STATUS
{
	STATUS_FREE = 1,				//空闲状态
	STATUS_PLAY,					//游戏中状态(可扩展)
	STATUS_END						//结算状态
}; 

//玩家状态
enum USER_STATUS
{
	USER_SEE = 0,				//旁观
	USER_JOIN					//参与游戏
};
 
 
enum Game_Sprint_Type
{
	type_sprint_normal = 0,
	type_sprint_fan = 1,
	type_sprint_sucess = 2
};

//新增录像记录，从 30开始
#define	   RECORD_PRIVATE_INFO            30					//包间信息
#define	   RECORD_GAME_RULE				  31					//游戏规则
#define	   RECORD_USER_INFO				  32					//游戏规则
#define	   RECORD_NOTIFY_INFO			  33					//通知类消息
#define	   RECORD_SEND_CARD				  34					//发牌消息
#define	   RECORD_CHANGE_CARD			  35					//换三张消息
#define	   RECORD_DEAL_RES				  36					//玩家操作处理结果
#define	   RECORD_BACK_CARD				  37					//底牌消息
#define	   RECORD_OUT_CARD				  38					//出牌结果消息
#define	   RECORD_NEW_TRUN				  39					//新一轮消息
#define	   RECORD_AWARD_POINT			  40					//奖励分
#define	   RECORD_GAEM_END				  41					//游戏结算
#define	   RECORD_COIN_CHANGE			  42					//玩家金币变化
#define	   RECORD_SHOW_CARD				  43					//玩家明牌

//游戏定时器 1-50   
#define   ONE_SECOND_VALUE		  1000
///定时器定义
#define	   GAME_TIME_AUTO_LEFT	   19	  //自动处理不准备的玩家
#define    GAME_TIME_SEND_CARD     20     //发牌
#define    GAME_TIME_CALL_SCORE    21	  //叫分
#define    GAME_TIME_ROB_NT        22	  //抢地主
#define    GAME_TIME_ADD_DOUBLE    23     //加倍
#define    GAME_TIME_SHOW_CARD     24     //明牌
#define    GAME_TIME_OUT_CARD      25     //出牌
#define    GAME_TIME_NEW_TURN      26     //下一轮
#define    GAME_TIME_RESLUT        27     //结算
#define    GAME_TIME_CONNECT       28     //断线重连
#define    GAME_TIME_FREE_DESK     32     //玩家解散定时器
#define    GAME_TIME_READY         33     //游戏准备
#define    GAME_TIME_CHANGE_CARD   34     //换三张 
#define    GAME_TIME_CHANGE_RES    35	  //换牌结果
#define    GAME_TIME_CUT_LIST      36     //通知玩家切牌
#define    GAME_TIME_CUT_ANI       37     //通知玩家切牌
#define    GAME_TIME_SHOW_FIRST    38     //通知所有玩家展示首张牌
#define    GAME_TIME_READY_FOR_NEXT 39    //打完后下一局准备的解散倒计时
#define	   GAME_TIME_CLEAR_CUT_USER 40	  //清理掉线玩家
#define    GAME_TIME_NOTIFY_OUT_CARD 41	  //通知玩家出牌
//游戏消息 
//send data from server to  client
//S->C
#define   GAME_MSG_SEND_ALL_CARD			1010				//发送所有牌(一下发放全部)
#define   GAME_MSG_GAME_MULTIPLE			1020				//游戏倍数(抢地主后会加倍)
#define   GAME_MSG_BACK_CARD_EX				1030				//底牌数据
#define   GAME_MSG_NOTIFY_INFO              1040                //服务端通知客户端消息
#define   GAME_MSG_DEAL_RES                 1041                //服务端处理结果后发送给客户端
#define	  GAME_MSG_SHOW_CARD                1042                //服务端发送明牌消息
#define   GAME_MSG_CHANGE_CARD              1043                //换牌
#define   GAME_MSG_USER_AGREE               1044                //玩家同意
#define   GAME_MSG_USER_COIN_CHANGE         1045                //服务端通知客户端玩家金币变化（不包含结算时金币变化）
#define   GAME_MSG_USER_NEED_CHANGE_DESK	1046				//服务器发送需要换桌消息
#define   GAME_MSG_SEND_CARD_TO_ROBOT		1047				//服务器发送所有手牌给机器人

#define   GAME_MSG_OUT_CARD_RESULT			1050				//出牌結果
#define   GAME_MSG_NEW_TURN					1060				//新一轮开始
#define   GAME_MSG_AWARD_POINT				1070				//奖分(炸弹火箭)
#define   GAME_MSG_SET_GAEM_END				1080				//游戏结束


//recv data from client to  server 
#define   GAME_MSG_USER_OPER_INFO           2010                //玩家操作结果
#define   GAME_MSG_OUT_CARD					2020				//用户出牌 


#pragma pack(push,1)
//游戏逻辑中，需要的结构体
//基本配置
typedef struct sg_game_basic_config
{
	unsigned int					m_free_mode;							//解散模式  0:所有人同意  1：大部分人同意
	unsigned int					m_player_mode;							//玩家模式  0:满人 1：全部同意
	unsigned int					m_min_player;							//最少人数
	sg_game_basic_config()
	{
		memset(this, 0, sizeof(sg_game_basic_config));
	}

}game_basic_config;
 

//房间游戏配置-根据游戏不同而设置
typedef struct sg_game_config
{  
	int				iSetRoomCount;    //设置打多少盘
	int				iCostOpenCard;    //对应盘数消耗的房卡数量
	unsigned char	byLimitScore;     //是否限制炸弹倍数
	bool			bTakePair;        //是否可以带对
	int				baseCoinScore;	  //金币底分
	int				iminCoinAccess;	  //准入金币数
	unsigned char	byplaymode[10];   //游戏玩法0，(双王四个2必叫3分) 1，(--地主模式，1-->明牌；2-->加倍),2，是否换三张,3叫分抢地主模式(0,1^2分，1 1^3分 2，抢地主),4不洗牌（0，洗，1不洗）5，发牌张数6，切牌次数 7首叫(0：顺序首叫,1：赢家首叫,2：随机首叫)8,上台阶

	sg_game_config()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
	void InitConfig()
	{
		memset(this, 0, sizeof(sg_game_config));
	} 
}game_config;

 
//玩家信息
typedef struct sg_game_user_info
{
	unsigned int	user_id;				//玩家ID 
	unsigned char	chair_id;				//玩家在桌上的位置
	unsigned int	game_status;			//游戏状态 0：旁观 1:参与游戏
	bool			cut_status;				//断线状态 false：在线 true：断线
	bool			user_ready;				//玩家是否准备 false：未准备 true：准备
	long long		game_score;				//游戏积分(根据不同货币，赋予不同值)
	char			nickname[60];			//玩家昵称
	char			head_url[200];			//头像路径  
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = 255;
	}
	sg_game_user_info()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = 255;
	}
}game_user_info;


/********************************************************************************/
//游戏状态数据包	（ 等待其他玩家开始 ）
struct GSBaseInfo
{
	game_config		gameconfig;						//游戏配置
	BYTE			byStation;						//游戏状态
	BYTE			iBeginTime;						//开始准备时间
	BYTE			iThinkTime;						//出牌思考时间
	BYTE			iCallScoreTime;					//叫分计时
	BYTE			iAddDoubleTime;					//加时间
	BYTE			iBackCount;						//底牌数
	UINT			user_id[PLAY_COUNT];			//玩家id	
	long long		user_score[PLAY_COUNT];			//玩家分数
	bool			bArgee[PLAY_COUNT];				//用户同意	
	bool			user_cut[PLAY_COUNT];			//玩家状态
	UINT			cur_play_count;					//当前盘数	
	BYTE			my_station;						//自己的位置 
	UINT			out_count[PLAY_COUNT];			//超时 
	BYTE            bychangetime;                   //换三张时间
	BYTE            bycutlisttime;                  //切牌时间
	int				tax;							//台费
	BYTE            byBuyaoTime;					//要不起时间
	unsigned int    freeGametime;                   //解散包间时间
	bool            bfirstcount;                    //是否首局
};
//游戏状态数据包	（ 等待扣押底牌状态 ）
struct GameStation_PlayPre
{
	BYTE		byGameFlag;										//叫分标记
	BYTE		byCallScorePeople;								//当前叫分人
	BYTE		byCallScoreResult;								//所叫的分
	BYTE		byNtPeople;										//庄家位置
	int			iLeaveTime;										//重连定时器剩余时间
	BYTE		byDouble[PLAY_COUNT];							//加倍情况
	bool		bAuto[PLAY_COUNT];								//托管情况
	BYTE		byShowCard[PLAY_COUNT];							//明牌情况
	BYTE		byCallScore[PLAY_COUNT];						//几家叫分情况
	BYTE		byRobNT[PLAY_COUNT];							//抢地主
	BYTE		byUserCardCount[PLAY_COUNT];					//用户手上扑克数目
	BYTE		byUserCardList[PLAY_COUNT][HAND_CARD_MAX];		//用户手上的扑克(其他玩家可能明牌)
	BYTE        byBackCount;                                    //底牌数量
	BYTE        byBackCard[BACK_CARD_COUNT];                    //底牌数据
	BYTE        bychoosestate[PLAY_COUNT];						//选牌状态
	BYTE        bychoosecardlist[3];                            //选择的牌
	BYTE        byCurrentRobNt;                                 //当前叫地主玩家
	BYTE        bycutuser;                                      //切牌玩家
	BYTE        bycutnum;                                       //切牌数量
	int         ibase;                                          //基本倍数
	GSBaseInfo  gamebaseInfo;									//基本数据
};
//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_Playing
{
	bool		bIsLastCard;						//是否有上轮数据		
	BYTE		bIsPass;							//是否不出
	BYTE		cbRobNtMul;							//抢地主倍数
	BYTE		cbShowCardMul;						//明牌倍数
	BYTE		cbBkCardMul;						//底牌倍数
	BYTE		cbBombMul;							//炸弹个数
	int			iBase;								//当前倍数
	int 		iCallScoreResult;					//叫分结果
	int			iLeaveTime;							//重连定时器剩余时间	
	BYTE		byBombNum;							//炸弹个数
	BYTE		byNTPeople;							//庄家位置
	BYTE		byOutCardPeople;					//现在出牌用户
	BYTE		byFirstOutPeople;					//先出牌的用户
	BYTE		byBigOutPeople;						//先出牌的用户
	BYTE		byRobNT[PLAY_COUNT];				//抢地主
	BYTE		byDouble[PLAY_COUNT];				//加倍情况
	BYTE		byShowCard[PLAY_COUNT];				//明牌情况
	bool		bAuto[PLAY_COUNT];					//托管情况
	BYTE		byDeskCardCount[PLAY_COUNT];			 //桌面扑克的数目
	BYTE        byDeskCard[PLAY_COUNT][HAND_CARD_MAX]; //桌面上的扑克 
	BYTE		byLastCardCount[PLAY_COUNT];				//上轮扑克的数目
	BYTE		byLastOutCard[PLAY_COUNT][HAND_CARD_MAX];//上轮的扑克
	BYTE        byLastPass;                                //上一轮不要的情况
	BYTE		byUserCardCount[PLAY_COUNT];			     //用户手上扑克数目
	BYTE		byUserCardList[PLAY_COUNT][HAND_CARD_MAX];//用户手上的扑克
	BYTE        byBackCount;                                    //底牌数量
	BYTE        byBackCard[BACK_CARD_COUNT];                    //底牌数据
	GSBaseInfo  gamebaseInfo;						//基本数据	
};

 
//S-->C
//游戏消息结构体 
//游戏状态基本信息
typedef struct sc_game_base_info
{
	game_config			gameconfig;					//游戏配置 
	unsigned char		game_status;				//当前桌子状态
	unsigned char		cur_status;					//当前游戏阶段 
	unsigned char		my_station;					//自己的位置 
	sc_game_base_info()
	{ 
		game_status = USER_LOOK_STATE;
		cur_status = STATUS_FREE;
		my_station = 255; 
	}

}game_base_info;
//游戏准备阶段
typedef struct sc_game_status_ready
{ 
	game_base_info		base_info;
	game_user_info		user_info[PLAY_COUNT];						//玩家信息  
	sc_game_status_ready()
	{ 

	}

}game_status_ready;
    
//游戏开始阶段
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//基础信息
	game_user_info		user_info[PLAY_COUNT];						//玩家信息   
	unsigned int		msg_index;									//消息索引-用于区分消息次序
	unsigned int		player_num;									//参与游戏的人数 

	sc_game_status_play()
	{
		msg_index = 0;
		player_num = 0; 
	}
}game_status_play;
 
////通知玩家游戏结束
//typedef struct sc_notify_game_result
//{  
//	unsigned char			hand_card[PLAY_COUNT][HAND_CARD_MAX];		//玩家手牌
//	unsigned char			card_type[PLAY_COUNT];						//牌型
//	int						 win_score[PLAY_COUNT];						//输赢分
//	int						 total_score[PLAY_COUNT];					//玩家总分
//	 
//	sc_notify_game_result()
//	{
//		memset(this, 0, sizeof(sc_notify_game_result)); 
//	}
//}notify_game_result;
  
//通知玩家同意
typedef struct sc_user_agree
{
	unsigned int	user_id;			//玩家ID
	unsigned char   chair_id;			//玩家位置
	bool			bagree;				//是否同意
	sc_user_agree()
	{
		user_id = 0;
		chair_id = 255;
		bagree = false;
	} 
}user_agree;

//通知玩家同意
typedef struct sc_user_agree_i
{
	bool		    b_agree[PLAY_COUNT];			//玩家是否同意信息  
	unsigned char	bytime;							//玩家准备倒计时  0：不限制  
	sc_user_agree_i()
	{
		memset(b_agree, 0, sizeof(b_agree));
		bytime = 0;
	}
}user_agree_i;

//通知玩家金币变化（不包含小局金币结算变化）
typedef struct sc_user_money_change
{
	unsigned char   chair_id;						//玩家是否同意信息  PLAY_COUNT:所有玩家
	unsigned char   money_type;						//金币类型 0:金币  
	long long		cur_money[PLAY_COUNT];			//玩家货币当前值
	sc_user_money_change()
	{
		chair_id = 255;
		money_type = 255;
		memset(cur_money, 0, sizeof(cur_money));
	}
}user_money_change;

//用户出牌数据包 （发向服务器）
struct OutCardStruct
{
	BYTE        byDeskStation;                  //玩家位置
	BYTE		byCardCount;						//扑克数目
	BYTE		byCardList[HAND_CARD_MAX];		//扑克信息
};
//通知玩家消息
struct  UserOperStruct
{
	BYTE      byFlag;					 //动作标识	
	BYTE      byChairID;				 //玩家位置
	BYTE      byOperValue;               //操作值
	BYTE	  byFlagEx[3];              //附加标识(换三张的牌值)
};
//S-->C
//需要换桌
struct NeedChangeDeskStruct
{
	BYTE	byChairID;            //玩家位置
	bool	bisNeed;			  //是否需要换桌
};


//换牌
struct  ChangeCardStruct
{
	bool    bAntiCloseWise;       //是否逆时针    
	BYTE	byDeskPos;            //玩家位置
	BYTE	byChooseCardList[3];  //玩家选择的牌
	BYTE    byGetCardList[3];     //玩家得到的牌
};
//明牌
struct ShowCardStruct
{
	BYTE		bDeskStation;									//坐号
	BYTE		iCardList[HAND_CARD_MAX];						//扑克信息
	BYTE		iCardCount;										//扑克数目
	int			iValue;											//明牌值
	int			iBase;											//倍数
};
//通知玩家消息
struct  NotifyUserOperData
{
	BYTE      byChairID;				//玩家位置
	BYTE      byFlag;					//动作标识	
	BYTE      byChairIDEx;              //扩展值,开始，0，表示正常开始，255，表示无人叫分，重新开始
	BYTE	  byFlagEx[PLAY_COUNT];     //附加标识
};
//发送玩家数据
struct SendAllStruct
{
	BYTE		byFirstCallScore;								//谁明牌(谁最先叫分)
	BYTE        byCardValue;                                    //亮牌的牌值
	BYTE		byCardCount[PLAY_COUNT];						//发牌数量
	BYTE		byCardList[HAND_CARD_MAX];						//发牌队例	
};

//发送玩家牌数据
struct SendAllCardInfoStruct
{
	BYTE		byPreCardCount[HAND_CARD_MAX];						//上家牌数据
	BYTE		byPostCardCount[HAND_CARD_MAX];						//下级牌数据
	BYTE        byBackCard[BACK_CARD_COUNT];						//底牌三张
};
//客户端处理结果完了之后发送给客户端
struct DealUserOperRes
{
	BYTE      byFlag;					//动作标识	
	BYTE      byChairIDEx;              //扩展值
	BYTE	  byValueEx;				//附加标识
};
//底牌数据包
struct BackCardExStruct
{
	BYTE		iGiveBackPeople;				//底牌玩家
	BYTE		iBackCardCount;					//扑克数目
	BYTE		iBackCard[12];					//底牌数据
};
//用户出牌数据包 （发向客户端）
struct OutCardMsg
{
	BYTE		byCardCount;					//扑克数目
	BYTE		bDeskStation;					//当前出牌者	
	BYTE		iCardList[HAND_CARD_MAX];		//扑克信息
};
//新一轮
struct NewTurnStruct
{
	BYTE		bDeskStation;					//坐号
	BYTE		bReserve;						//保留
};
struct AwardPointStruct
{
	BYTE	bDeskStation;									//坐号
	BYTE    cbRobNtCnt;										//地主
	BYTE	iBombNum;										//炸弹个数
};

//游戏结束统计数据包
struct notify_game_result
{
	long long		iTurePoint[PLAY_COUNT];			//玩家得分
	int				iScoreMultiple[PLAY_COUNT];		//玩家倍数
	BYTE			iUpGradeStation;				//庄家位置
	BYTE			iCallScore;						//庄家叫分
	BYTE			iRobMul;						//抢地主亮牌倍数
	BYTE			iBombCount;						//炸弹加倍
	BYTE			iKingBomb;						//王炸
	int				iSpecialTaskMul;				//特殊任务
	int				iTotalMul;						//总倍数
	BYTE			bySprint;                       //春天类型
	bool			bFinishTask[PLAY_COUNT];		//是否完成任务
	BYTE			byWinPeople;					//是谁赢
	BYTE			iUserCardCount[PLAY_COUNT];		//用户手上扑克数目
	BYTE			iUserCard[PLAY_COUNT][HAND_CARD_MAX];		//用户手上的扑克
	bool			bLimit[PLAY_COUNT];				//得分是否因为输赢不足受限制
	int				iDouble[PLAY_COUNT];			//各家加倍情况
	int				iPower[PLAY_COUNT];				//各家输赢倍数
	unsigned int	reward_gift[PLAY_COUNT];					//奖励礼券	

	notify_game_result()
	{
		memset(this, 0, sizeof(notify_game_result));
	}
};
//C-->S
//消息结构体 
//发送玩家数据
 

#pragma pack(pop)

#endif