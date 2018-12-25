#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_ 
#include <windows.h>
#include <stdarg.h> 
#include "mjdata.h"
#define NAME						    hzmj								 // 红中麻将 
//#define PLAY_COUNT						4						             // 游戏人数
#define MAX_ZHA_MA						68									//最大扎码数
#define PLAY_COUNT					8						             // 游戏人数
#define MAX_COUNT					5									//最大数目
//游戏定时器 1-50 (40-50 已用)
#define		TIME_SEND_CARD					1							//发牌
#define		TIME_SEND_ONE_CARD				2							//摸牌 
#define		TIME_FREE_DESK					3							//解散包间
#define		TIME_USER_OUT_1					4							//玩家1超时
#define		TIME_USER_OUT_2					5							//玩家2超时
#define		TIME_USER_OUT_3					6							//玩家3超时
#define		TIME_USER_OUT_4					7							//玩家4超时
#define		TIME_USER_CUT_INFO_1			8							//发送断线信息-
#define		TIME_USER_CUT_INFO_2			9							//发送断线信息-
#define		TIME_USER_CUT_INFO_3			10							//发送断线信息-
#define		TIME_USER_CUT_INFO_4			11							//发送断线信息-

//游戏消息
//server to  client
#define STC_SEND_MAKER_INFO				1000						//发送庄家信息
#define STC_SEND_DICE_INFO				1001						//发送骰子信息
#define STC_SEND_CARD_INFO				1002						//发送扑克信息
#define STC_SEND_OUT_CARD				1003						//玩家出牌
#define STC_SEND_GET_CARD				1004						//玩家摸牌
#define STC_SEND_BLOCK_INFO				1005						//发送拦牌消息
#define STC_SEND_BLOCK_RESULT			1006						//发送拦牌结果消息
#define STC_SEND_NOTIFY_OUT				1007						//通知玩家出牌 
#define STC_SEND_NOTIFY_WAIT			1008						//发送等待消息
#define STC_SEND_GAME_RESULT			1009						//发送游戏结算信息
#define STC_SEND_TING					1010						//发送听牌消息 
#define STC_SEND_USER_AGREE				1011						//发送玩家准备消息	
#define STC_SEND_GAME_RECORD			1012						//发送牌局信息
#define STC_SEND_TOTAL_RECORD			1013						//发送所有牌局			
#define STC_SEND_FAST_OUT				1014						//发送快速发牌
#define	STC_SEND_USER_FREE				1015						//发送玩家解散游戏消息
#define STC_SEND_FREE_RESUTL			1016						//解散结果
#define STC_SEND_GAME_INFO				1017						//游戏配置信息
#define STC_SEND_TIME_OUT				1018						//通知玩家超时
/***************************************【牌类】********************************************/
///状态消息定义
#define  GAME_MSG_STATUS_READY                 50           //准备状态消息
#define  GAME_MSG_STATUS_CALLBANKER            51           //抢庄状态消息
#define  GAME_MSG_STATUS_XIAZHU                52           //下注状态消息
#define  GAME_MSG_STATUS_OPEN                  53           //开牌状态消息
#define  GAME_MSG_STATUS_SET_NUM               54           //设置手牌数量消息
#define  GAME_MSG_STATUS_CALLBANKER_DGN        55           //斗公牛抢庄状态消息
#define  GAME_MSG_STATUS_KPQZ_SET_NUM          56           //看牌抢庄设置手牌数量消息
#define  GAME_MSG_STATUS_START_GAME            57           //游戏开始消息
#define  GAME_MSG_STATUS_WAIT_START            58           //游戏等待开始消息

///服务端消息
#define  GAME_MSG_SEND_CALLBANKER              61           //玩家抢庄消息
#define  GAME_MSG_SEND_BANKER                  62           //发送庄家消息
#define  GAME_MSG_SEND_USERCARD                63           //发送玩家牌数据消息
#define  GAME_MSG_SEND_OPENCARD                64           //玩家摊牌消息
#define  GAME_MSG_SEND_RESULT                  65           //游戏输赢结果消息
#define  GAME_MSG_SEND_XIAZHU                  66           //玩家下注消息
#define  GAME_MSG_SEND_XIAZHURESULT            67           //玩家下注消息结果
#define  GAME_MSG_SEND_OPENCARDALONE           68           //玩家单独摊牌消息
#define  GAME_MSG_SEND_USER_JOIN               69           //其他玩家加入游戏
#define  GAME_MSG_SEND_USER_LEFT               70           //其他玩家离开游戏
#define  GAME_MSG_SEND_GAME_RULE			   71           //发送规则	
#define	 GAME_MSG_SEND_FAST_OUT			       72			//发送快速发牌
#define  GAME_MSG_SINGLE_INFO                  73           //小局记录
#define  GAME_MSG_TOTLE_INFO                   74           //总局记录
#define  GAME_MSG_CUOPAI_INFO                  75           //搓牌结果消息
#define  GAME_MSG_COINS_NOT_ENOUTH             76           //金币不足消息
#define  GAME_MSG_SEND_GAME_OVER               77           //发送游戏结束消息

///客户端消息
#define	 GAME_MSG_RECV_CALLBANKER               80          //玩家抢庄消息
#define  GAME_MSG_RECV_XIAZHU                   81          //玩家下注消息
#define  GAME_MSG_RECV_OPENCARD                 82          //玩家摊牌消息
#define  GAME_MSG_SET_CARD_COUNT				83          //玩家设置几副牌 
#define  GAME_MSG_USER_AGREE                    84          //玩家同意开始
#define  GAME_MSG_REQUEST_RECORD                85          //请求小局记录
#define  GAME_MSG_USER_READY_INFO               86          //所有玩家的准备状态
#define  GAME_MSG_USER_ROBNT_INFO               87          //抢庄情况
#define  GAME_MSG_USER_FALL_INFO                88          //下庄情况
#define  GAME_MSG_KPQZ_SET_CARD_COUNT			89          //看牌抢庄自由选牌消息
#define  GAME_MSG_KPQZ_CUOPAI		        	90          //搓牌消息
#define  GAME_MSG_KPQZ_LEFT  		        	91          //主动离开消息

/***********************************************************************************/




//发送共用消息
#define STC_UPDATE_USER_INFO			1101						//更新玩家信息		

#define CTS_GET_GAME_CONFIG				1201						//申请获取游戏配置
#define CTS_UPDATE_USER_INFO			1202						//申请更新自己信息



//client to server
#define CTS_RECV_PLAY_DICE				2000						//收到庄家打骰消息
#define CTS_RECV_OUT_CARD				2001						//收到玩家出牌
#define CTS_RECV_BLOCK_OPERATE			2002						//收到玩家拦牌操作
#define CTS_RECV_SET_NEXT_WALL			2003						//设置下一张牌墙
#define CTS_RECV_USER_AGREE				2004						//收到玩家准备消息
#define CTS_RECV_USER_FREE_GAME			2005						//收到玩家解散包间消息

#pragma pack(push,1)

//拦牌信息
typedef struct sc_game_block_data
{
	unsigned char	block_type;			//拦牌类型 1:吃 2 碰 4 杠
	unsigned char	block_sub_type;		//拦牌子类型  1:暗杠 2 明杠 4 补杠
	unsigned char	blocked_card;		//被拦的牌
	unsigned char	blocked_user;		//被拦的人
	unsigned char	block_data[4];		//拦牌数据
	sc_game_block_data()
	{
		memset(this, 0, sizeof(game_block_data));
	}

}game_block_data;

//玩家信息
typedef struct sc_game_user_info
{
	game_block_data	block_info[BLOCK_MAX];			//玩家拦牌 
	unsigned int	hand_card_count;				//玩家手牌数
	unsigned int	out_card_count;					//玩家出牌数	
	unsigned int	block_count;					//玩家拦牌数	
	unsigned char	hand_card[HAND_CARD_COUNT];		//玩家手牌
	unsigned char	out_card[MAX_OUT_CARD];			//玩家出牌
	unsigned char	   draw_card;					//玩家摸的牌
	sc_game_user_info()
	{
		memset(this, 0, sizeof(game_user_info));
	}
}game_user_info;

//更新玩家信息 
typedef struct stu_update_user_info
{
	unsigned char	chair_id;						//玩家 1
	game_block_data	block_info[BLOCK_MAX];			//玩家拦牌  40
	unsigned int	hand_card_count;				//玩家手牌数 4
	unsigned int	out_card_count;					//玩家出牌数 4	 
	unsigned int	block_count;					//玩家拦牌数 4
	unsigned char	hand_card[HAND_CARD_COUNT];		//玩家手牌 14
	unsigned char	out_card[MAX_OUT_CARD];			//玩家出牌 40
	unsigned char	   draw_card;					//玩家摸的牌 1

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}

}update_user_info;


//房间游戏配置
typedef struct stu_game_config
{
	int				room_card;				//消耗房卡数
	bool			hu_qxd;					//是否允许胡七对
	int				hongaward_ma;			//红中加码数
	int				award_ma;				//扎码数
	int				luck_mode;				//游戏模式 0:正常 1：翻码 2:一马全中
	bool			all_ma;					//四红中全码 
	stu_game_config()
	{
		room_card = 0;
		hu_qxd = false;
		hongaward_ma = 0;
		award_ma = 0;
		luck_mode = 0; 
		all_ma = false;
	}
}game_config;

//S-->C
//游戏消息结构体 
//游戏状态基本信息
typedef struct sc_game_base_info
{
	unsigned char		game_status;				//游戏状态 1
	unsigned int		user_id[PLAY_COUNT];		//玩家id 16 
	long long			user_score[PLAY_COUNT];		//玩家分数 32
	bool				user_cut[PLAY_COUNT];		//玩家状态 4
	unsigned int		total_play_count;			//总局数	4
	unsigned int		cur_play_count;				//已开局   4
	bool				bOpenFree;					//是否开启解散 1
	unsigned int		free_apply_id;				//申请者id   4
	unsigned char		free_apply_station;			//申请者位置 1
	bool				bUserChose[PLAY_COUNT];		//玩家是否操作 4
	unsigned int		free_remain_time;			//剩余时间 4
	unsigned char		mj_status;					//麻将阶段状态 1
	unsigned char		banker_station;				//庄家位置 1
	unsigned int		fast_count[PLAY_COUNT];		//快速出牌次数 16
	unsigned int		out_count[PLAY_COUNT];		//超时次数		16
	unsigned char		my_station;					//自己的位置	1
	unsigned char		private_station;			//房主位置		1
	unsigned int		private_id;					//房主位置		4
	unsigned int		free_time;					//解散包间时间
	game_config			gameconfig;					//游戏配置		18
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));  
		free_apply_station = 255; 
		banker_station = 255;
		my_station = 255;
		private_station = 255;
	}

}game_base_info;
 
//游戏准备阶段
typedef struct sc_game_status_ready
{ 
	game_base_info		base_info;
	bool				user_ready[PLAY_COUNT];
	sc_game_status_ready()
	{
		memset(user_ready, 0, sizeof(user_ready));
	}

}game_status_ready;
//快速出牌消息
typedef struct sc_fast_out_info
{
	unsigned char			chair_id;		//玩家位置
	unsigned int			add_fast_count;		//增加次数
	unsigned int			fast_all_count;	//快速次数总数
	sc_fast_out_info()
	{
		memset(this, 0, sizeof(sc_fast_out_info));
	}
}fast_out_info;
//提示玩家超时
typedef struct sc_user_time_out
{
	unsigned char			chair_id;		//玩家位置 
	unsigned int			out_all_count;	//快速次数总数
	sc_user_time_out()
	{
		memset(this, 0, sizeof(sc_user_time_out));
	}
}user_time_out;


//游戏开始阶段
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//基础信息
	unsigned char		dice[2];									//骰子
	unsigned char		cur_operate_station;						//当前操作玩家
	unsigned char		cur_gams_stats;								//当前游戏状态
	unsigned char		cur_block_level;							//当前拦牌等级
	unsigned char		cur_check_user;								//当前被拦的人
	unsigned char		cur_check_card;								//当前被拦的牌
	bool				bdraw_stats;								//是否是摸牌 

	unsigned int		wall_count;									//牌墙数量
	game_user_info		user_info[PLAY_COUNT];						//玩家信息 
	unsigned int		gang_all_count;								//杠的总数
	bool				ting;										//是否听
	unsigned char		ting_list[NORMAL_CARD_NUM + 1];				//听的牌
	unsigned char		ting_count[NORMAL_CARD_NUM + 1];			//听牌的个数

	unsigned char	    block_info[BLOCK_MAX][5];					//可以拦的牌  
	unsigned int		block_index;								//拦牌索引
	 
	sc_game_status_play()
	{
		memset(this, 0, sizeof(sc_game_status_play));
		cur_operate_station = 255;
	}
}game_status_play;


//定庄消息
typedef struct sc_maker_info
{
	unsigned char   chair_id;				//庄家位置
	sc_maker_info()
	{
		chair_id = 255;
	}
}maker_info;

//骰子消息
typedef struct sc_dice_info
{
	unsigned char		dice[2];			//骰子信息
	sc_dice_info()
	{
		memset(dice, 255, sizeof(dice));
	}
}dice_info;

//发牌消息
typedef struct sc_hand_card_info
{
	unsigned char	hand_card[PLAY_COUNT][HAND_CARD_COUNT];			//玩家手牌
	unsigned char	card_count[PLAY_COUNT];							//手牌个数
	sc_hand_card_info()
	{
		memset(hand_card, 0, sizeof(hand_card));
		memset(card_count, 0, sizeof(card_count));
	}
}hand_card_info;
//玩家出牌
typedef struct sc_user_out_card_result
{
	unsigned char   chair_id;				//玩家位置
	unsigned char   card_value;				//牌值
	sc_user_out_card_result()
	{
		chair_id = 255;
		card_value = 0x00;
	}
}user_out_card_result;
//玩家摸牌
typedef struct sc_user_touch_card
{
	unsigned char   chair_id;				//玩家位置
	unsigned char   card_value;				//牌值
	bool			bGang;					//发送是杠牌摸牌
	sc_user_touch_card()
	{ 
		chair_id = 255;
		card_value = 0x00;
		bGang = false;
	}
}user_touch_card;
 
//玩家拦牌消息
typedef struct sc_notify_block_info
{ 
	unsigned char   card_value;							//牌值
	bool			out;								//出牌/摸牌
	unsigned char   block_level;						//玩家拦牌等级
	unsigned char	checked_id;							//被拦牌的人 
	unsigned char   block_info[BLOCK_MAX][5];			//可以拦的牌  
	unsigned int	block_index;						//拦牌索引
	sc_notify_block_info()
	{
		block_index = 0;
		checked_id = 255;
		out = false;
		card_value = 0x00; 
		block_level = 0x00;  
		memset(block_info, 0, sizeof(block_info)); 
	}
}notify_block_info;

//通知玩家等待拦牌操作消息
typedef struct sc_notify_wait_block_operate
{
	unsigned char   card_value;							//牌值
	bool			out;								//出牌/摸牌
	unsigned char	block_level[PLAY_COUNT];			//玩家拦牌等级
	unsigned char	checked_id;							//被拦牌的人
	sc_notify_wait_block_operate()
	{
		out = false;
		card_value = 0x00;
		checked_id = 255;
		memset(block_level, 0, sizeof(block_level)); 
	}
}notify_wait_block_operate;

//通知玩家拦牌操作结果
typedef struct sc_notify_block_reslut
{
	unsigned char   chair_id;				//玩家位置 
	unsigned char	opreate_type;			//操作类型 0 放弃 0x01 吃  0x02 碰 0x04 杠 0x08 听 0x10 胡
	unsigned char   card_value[4];			//牌值 0-3 选择的拦牌数据
	unsigned char	checked_id;				//被拦牌的人
	unsigned char	block_subtype;			//拦牌类型
	sc_notify_block_reslut()
	{
		block_subtype = 0;
		chair_id = 255;
		checked_id = 255;
		opreate_type = 0x00;
		memset(card_value, 0, sizeof(card_value));
	}

}notify_block_reslut;

//通知玩家出牌
typedef struct sc_notify_out_card
{ 
	unsigned char   chair_id;				//玩家位置
	sc_notify_out_card()
	{
		chair_id = 255;
	}
}notify_out_card;

//通知玩家游戏结束
typedef struct sc_notify_game_result
{ 
	int						 bWinType;									//胡牌方式 0：荒庄   1：自摸 2：放炮  3：抢杠 4:杠开
	bool					 bWin[PLAY_COUNT];							//胡牌玩家
	unsigned char			 hand_card[PLAY_COUNT][HAND_CARD_COUNT];	//玩家手牌
	unsigned char			 win_card;									//胡的牌
	unsigned char			 dianpao_id;								//放炮的玩家
	int						 gang_score[PLAY_COUNT];					//玩家杠分
	int						 fan_score[PLAY_COUNT];						//玩家胡牌番数
	unsigned char			 luck_card[MAX_ZHA_MA];						//扎码牌值 
	long long				 total_score[PLAY_COUNT];					//玩家总分
	bool					 bAllKing;									//是否是全码
	unsigned int			 fast_count[PLAY_COUNT];					//快速次数
	unsigned int			 out_count[PLAY_COUNT];						//超时次数
	unsigned int			 reward_gift[PLAY_COUNT];					//奖励礼券
	unsigned int			 user_luck_count[PLAY_COUNT];				//玩家中码个数
	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
	}
}notify_game_result;

//听牌消息
typedef struct sc_notify_ting
{
	unsigned char	ting_list[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];
	unsigned char	ting_list_count[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];
	unsigned int    win_card_count[HAND_CARD_COUNT];	//胡牌个数
	unsigned int	max_win_count;						//最大胡牌个数
	sc_notify_ting()
	{
		memset(this, 0, sizeof(sc_notify_ting));
	}


}notify_ting;

//每一局的游戏记录
typedef struct sc_single_game_record
{
	unsigned int		index;			//序号
	unsigned char		win_type;		//赢牌模式
	unsigned char		dianpaoer;		//点炮的玩家
	unsigned char		win_card;		//胡的牌
	long long			finish_time;	//结束时间
	bool				win[PLAY_COUNT];//赢的玩家
	int					win_score[PLAY_COUNT];//番数
	int					gang_score[PLAY_COUNT];//杠分 
	int					luck_count[PLAY_COUNT];//中码张数
	int					mgang_count[PLAY_COUNT];//明杠次数
	int					agang_count[PLAY_COUNT];//暗杠次数
	int					bgang_count[PLAY_COUNT];//补杠次数
	int					fgang_count[PLAY_COUNT];//放杠次数
	unsigned int		fast_count[PLAY_COUNT];	//快速次数
	unsigned int		out_count[PLAY_COUNT];//超时次数
	unsigned int		reward_gift[PLAY_COUNT];//奖励礼券
	sc_single_game_record()
	{
		memset(this, 0, sizeof(sc_single_game_record));
		dianpaoer = 255;
	}

}single_game_record;
//总游戏记录
typedef struct sc_total_record_game_info
{
	unsigned int	play_count;					//总局数
	unsigned int	win_count[PLAY_COUNT];		//胡牌次数
	unsigned int	qiang_win_count[PLAY_COUNT];//抢杠胡次数
	unsigned int	ming_gang[PLAY_COUNT];		//明杠次数
	unsigned int	an_gang[PLAY_COUNT];		//暗杠次数
	unsigned int	bu_gang[PLAY_COUNT];		//补杠次数
	unsigned int	luck_count[PLAY_COUNT];		//中码次数
	int				fan_score[PLAY_COUNT];		//玩家得分
	int				gang_score[PLAY_COUNT];		//杠分
	unsigned int	fast_count[PLAY_COUNT];		//快速次数
	unsigned int	out_count[PLAY_COUNT];		//超时次数
	unsigned int	reward_gift[PLAY_COUNT];	//奖励礼券
	long long		finish_time;				//结束时间

	sc_total_record_game_info()
	{
		memset(this, 0, sizeof(sc_total_record_game_info));
	}
}total_record_game_info;

//听牌信息
typedef struct sc_user_ting_info
{
	bool		ting;
	unsigned char ting_list[NORMAL_CARD_NUM+1];
	unsigned char ting_count[NORMAL_CARD_NUM+1]; 
	
	void clear()
	{
		memset(this, 0, sizeof(sc_user_ting_info));
	}
	sc_user_ting_info()
	{
		memset(this, 0, sizeof(sc_user_ting_info));
	}
}user_ting_info;
//能听列表
typedef struct sc_user_can_ting_info
{	
	bool			can_ting;
	notify_ting		ting_info;
	void clear()
	{
		memset(this, 0, sizeof(sc_user_can_ting_info));
	}

	sc_user_can_ting_info()
	{
		memset(this, 0, sizeof(sc_user_can_ting_info));
	}
}user_can_ting_info;

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
/*******************************************************************************************/
//牌信息
typedef struct Stu_UserCard
{
	BYTE   chairid;			       //座位号
	BYTE   status;                 //游戏状态
	BYTE   bytime;                 //当前状态时间
	BYTE   byUsercard[5];          //单个玩家牌
	BYTE   byUserSecCard[5];       //闲家第二手牌
	BYTE   byCardType[2];          //玩家牌型
	bool   bHaveSecCard;           //是否有第二手牌
	UINT   bServeMoney[PLAY_COUNT];//服务费
	BYTE   byUserstatus[PLAY_COUNT];//玩家当前状态
	Stu_UserCard()
	{
		memset(this, 0, sizeof(Stu_UserCard));
		chairid = 255;
	}
}stu_usercard;
//抢庄状态
typedef struct Stu_Status_Qiang
{
	BYTE  status;                   //发送状态
	BYTE  bytime;                   //当前状态时间
	bool  bcan[PLAY_COUNT];         //是否能抢
	BYTE  beishu;                   //抢庄倍数
	INT64 byxianzhi[PLAY_COUNT];    //最大能抢倍数
	Stu_Status_Qiang()
	{
		memset(this, 0, sizeof(Stu_Status_Qiang));
	}
}stu_status_qiang;


//玩家抢庄
typedef  struct Stu_CallBanker
{
	BYTE  chairid;			       //座位号
	BYTE  byValue;			       //抢庄标志 0为不抢  1为抢庄,2为加倍抢庄
}stu_callBanker;

typedef  struct Stu_Status_XZ
{
	BYTE  status;                   //发送状态
	BYTE  bytime;                   //当前状态时间
	INT   iscore[PLAY_COUNT];       //推注分数
	bool  blimite[PLAY_COUNT];      //是否最小下注分限制
	bool  btuilimite[PLAY_COUNT];   //是否推注限制
	INT64 unxiazhu[PLAY_COUNT];     //下注限制分数
	Stu_Status_XZ()
	{
		memset(this, 0, sizeof(Stu_Status_XZ));
	}
}stu_status_XZ;


typedef  struct Stu_UserXiaZhu
{
	BYTE   chairid;			       //座位号
	INT    xiazhumoney;            //下注金额
}stu_userXiaZhu;

//接收玩家摊牌数据
typedef  struct Stu_UserOpenCard
{
	BYTE   chairid;              //座位号
	BYTE   openflag;             //摊牌标识（第几组牌）
	BYTE   cardlist[5];          //牌数据
}stu_userOpenCard;

//所有的牌数据
struct Stu_AllUserCard
{
	BYTE	status;								  //游戏状态
	BYTE	bytime;								  //当前状态时间
	BYTE	allusercard[PLAY_COUNT][5];           //所有玩家牌
	BYTE	cardtype[PLAY_COUNT]; 			      //牌型
	BYTE	alluserSecCard[PLAY_COUNT][5];        //玩家第二手牌的数据
	BYTE	cardSectype[PLAY_COUNT];              //第二手牌型
	BYTE	sortrank[PLAY_COUNT];                 //玩家排序

	Stu_AllUserCard()
	{
		memset(this, 0, sizeof(Stu_AllUserCard));
	}
};
//玩家分数变化
struct Stu_ChangeMoney
{
	Stu_AllUserCard  allCardData;
	BYTE			 chairid;			       //座位号
	BYTE			 status;                   //游戏状态
	int				 changemoney[PLAY_COUNT];  //输赢金币值 
	int				 iGetscore[PLAY_COUNT][2]; //每手牌获取的分数
	bool             benough[PLAY_COUNT][2];   //是否每手牌都够陪
	bool             changebanker;             //是否换庄
	BYTE             btanpai[PLAY_COUNT];
	BYTE             byUserstatus[PLAY_COUNT]; //玩家游戏状态
	BYTE             bnotenouth;               //玩家是否金币不足
	Stu_ChangeMoney()
	{
		memset(this, 0, sizeof(Stu_ChangeMoney));
		memset(btanpai, 255, sizeof(btanpai));
		chairid = 255;
	}
};
//游戏逻辑设定
struct GameLogicRuleSet
{
	BYTE		byGameLogicType;    //牌型设置（与Enum_RoomLogicSet对应获取掩码）
	BYTE        byKingType;        //王牌类型（0，没有王牌，1，王当10，2，大小王百变）	
};


//游戏获取放开
struct GameRoomConfigSet
{
	GameLogicRuleSet	 GameLogicSet;
	int					 iSetRoomCount;			//设置打多少盘
	int					 iCostOpenCard;			//对应盘数消耗的房卡数量
	BYTE				 byBeginPlayer;			//开始人数
	BYTE				 byFirstNT;				//开局庄家
	BYTE				 bychangeNTType;		//换庄模式  1~7对应游戏模式
	BYTE                 byHandCardNum;			//手牌数量（1，2）
	BYTE                 byplaymode[5];			//0，升分模式 1，3分模式 2,自由选牌
	int  				 byTuizhu;		        //推注
	WORD                 byFengding;            //封顶
	bool                 byxiazhulimit;         //下注限制
	int                  ixuanfen;              //选分
	WORD                 izhuangfen;            //庄分
	bool                 bylunzhuang;           //斗公牛轮庄 0抢庄 1轮庄
	bool                 byfreelowbank;         //可自由下庄
	int                  byGgbs;                //公共倍数
	bool                 bFangfei;              //房费
	BYTE                 byFanbei;              //翻倍规则
	BYTE                 byZuidaqz;             //最大抢庄
	BYTE                 byStart;               //自动开始
	bool                 bPlaygame;             //游戏开始后加入
	bool                 bChuopai;              //禁止搓牌
	GameRoomConfigSet()
	{
		GameLogicSet.byGameLogicType = 0;
		GameLogicSet.byKingType = 0;
		byHandCardNum = 1;
		iSetRoomCount = 12;
		iCostOpenCard = 1;
		byFirstNT = 0;
		bychangeNTType = 0;
		byTuizhu = 0;
		byFengding = 0;
		byxiazhulimit = false;
		ixuanfen = 0;
		izhuangfen = 0;
		bylunzhuang = false;
		byfreelowbank = false;
		memset(byplaymode, 0, sizeof(byplaymode));
	}
	void InitConfig()
	{
		GameLogicSet.byGameLogicType = 0;
		GameLogicSet.byKingType = 0;
		byHandCardNum = 1;
		iSetRoomCount = 12;
		iCostOpenCard = 1;
		byFirstNT = 0;
		bychangeNTType = 0;
		byTuizhu = 0;
		byFengding = 0;
		byxiazhulimit = false;
		ixuanfen = 0;
		izhuangfen = 0;
		bylunzhuang = false;
		byfreelowbank = false;
		memset(byplaymode, 0, sizeof(byplaymode));
	}
};
//游戏中玩家信息
typedef struct Stu_GameUserItem
{
	unsigned int	player_id;						//玩家ID
	BYTE            chairid;                        //座位id
	BYTE            bsex;                           //性别(1男2女)
	bool			look_status;					//是否是旁观状态
	bool            bplaygame;                      //是否不在游戏中
	bool            brest;                          //是否休息状态
	bool			cut_status;						//是否是断线状态
	long long		total_score;					//玩家总分
	long long		player_money;					//佘额(货币)
	long long		player_coins;					//金币(货币)
	long long		player_lottery;					//彩票(货币)
	long long       servercount;                    //服务费
	INT64           byRobbeishu;                    //抢庄最大倍数
	INT64           unXiazhu;                       //最大下注分
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;


struct Stu_GameStation
{
	BYTE			chairid;							//玩家座位号
	INT				userid;								//玩家ID
	BYTE			nowtime;							//当前状态剩余时间
	BYTE			readytime;							//准备时间
	BYTE			callbankertime;						//抢庄时间
	BYTE			xiazhutime;							//下注时间
	BYTE			sethandcouttime;					//设置手牌时间
	BYTE			opentime;							//开牌时间
	BYTE			resulttime;							//结算时间
	unsigned int    freeGametime;                       //解散包间时间
	BYTE			bybankeruser;						//当前庄家位置
	INT				chip[5];							//筹码值
	INT64			xzmoneycount[PLAY_COUNT];			//玩家下注筹码值
	INT64			winmoney[PLAY_COUNT];				//玩家输赢金额
	BYTE			usercard[PLAY_COUNT][5];			//玩家牌信息
	BYTE		    userSecCard[PLAY_COUNT][5];			//玩家第二手牌
	BYTE			cardtype[PLAY_COUNT];				//玩家牌型
	BYTE		    cardSectype[PLAY_COUNT];			//第二手牌的牌型
	BYTE			nowstatus;							//当前状态
	BYTE			callbanker[PLAY_COUNT];				//一局叫庄记录，没操作255，抢 1， 不抢 0
	BYTE			tanpai[PLAY_COUNT][2];				//一局摊牌记录，摊牌 1， 未摊牌 0
	BYTE			tanpaitype[PLAY_COUNT];				//一局摊牌类型记录，未摊牌255，其他为摊牌类型值	
	BYTE		    byHaveSecCad[PLAY_COUNT];			//玩家是否有第二手牌
	BYTE		    tanpaisectype[PLAY_COUNT];			//第二手摊牌类型
	BYTE			private_station;					//房主位置
	int				private_id;							//房主id
	BYTE		    byGamePlayNum;						//游戏打的盘数
	bool		    bAgreeStation[PLAY_COUNT];			//玩家同意状态
	bool			bOpenFree;							//是否开启解散
	unsigned int	free_apply_id;						//申请者id
	unsigned char	free_apply_station;					//申请者位置 
	bool			bUserChose[PLAY_COUNT];				//玩家是否操作
	unsigned int	free_remain_time;					//剩余时间
	unsigned int	fast_count[PLAY_COUNT];				//快速出牌
	bool            bChooseIndex[10];					//玩家选择索引
	UINT            ituizhu[PLAY_COUNT];                //推注分数
	UINT            iZhuangFen;                         //庄分
	bool            blimite[PLAY_COUNT];                //最低分下注限制
	BYTE			fourcard[PLAY_COUNT][4];			//玩家看牌信息
	bool            btuizhulimite[PLAY_COUNT];          //最低分推注限制
	BYTE            brestcount[PLAY_COUNT];             //休息局数
	BYTE            bcutcount[PLAY_COUNT];              //断线局数
	bool            bcuopai;                            //是否已搓牌
	bool            bfirstcount;                        //是否首局
	GameRoomConfigSet roomrule;                         //房间规则	
	GameUserItem    useritem[PLAY_COUNT];               //游戏中玩家信息
	Stu_GameStation()
	{
		ZeroMemory(this, sizeof(Stu_GameStation));
		memset(callbanker, 255, sizeof(callbanker));
		memset(tanpaitype, 255, sizeof(tanpaitype));
		bybankeruser = 255;
		private_station = 255;
	}
};







/*******************************************************************************************/
//C-->S
//消息结构体
//庄家打骰子
typedef struct cs_maker_play_dice
{
	unsigned char   chair_id;				//庄家位置
}maker_play_dice;

//玩家出牌
typedef struct cs_user_out_card
{
	unsigned char   chair_id;				//玩家位置
	unsigned char   card_value;				//牌值
	cs_user_out_card()
	{
		chair_id = 255;
		card_value = 0x00;
	}
}user_out_card;

//玩家拦牌操作
typedef struct cs_user_block_operate
{ 
	unsigned char   chair_id;				//玩家位置
	unsigned char	opreate_type;			//操作类型 0 放弃 0x01 吃  0x02 碰 0x04 杠 0x08 听 0x10 胡
	unsigned char   card_value[4];			//牌值 0-3 选择的拦牌数据
	bool			out;					//出牌/摸牌 
	unsigned int	block_index;					//拦牌索引
	cs_user_block_operate()
	{
		block_index = 0;
		chair_id = 255;
		opreate_type = 0x00; 
		out = false;
		memset(card_value, 0, sizeof(card_value));
	} 
}user_block_operate;

//设置好牌
typedef struct cs_set_next_wall
{
	unsigned char	card_value;
	cs_set_next_wall()
	{
		card_value = 0x00;
	}
}set_next_wall;

//解散游戏
typedef struct cs_user_free_opreate
{
	unsigned int		apply_id;		//申请者ID
	unsigned char		apply_chair;	//申请者位置
	unsigned int		userid;			//玩家ID
	unsigned char		chair_id;
	bool				free;
	cs_user_free_opreate()
	{
		memset(this, 0, sizeof(cs_user_free_opreate));
		apply_chair = 255;
		chair_id = 255;
	}
}user_free_opreate;

//解散结果
typedef struct cs_user_free_reslut
{
	unsigned int owner_id;		//不同意玩家
	unsigned char  owner_station;
	bool		result;
	cs_user_free_reslut()
	{
		memset(this, 0, sizeof(cs_user_free_reslut));
		owner_station = 255;
	}
}user_free_reslut;


//玩家申请更新游戏信息
typedef struct cs_apply_user_game_info
{
	unsigned int	user_id;
	unsigned char	chair_id;

	cs_apply_user_game_info()
	{
		memset(this, 0, sizeof(cs_apply_user_game_info));
		chair_id = 255;
	}
}apply_user_game_info;

//申请游戏配置信息
typedef struct cs_apply_game_config
{
	unsigned char	chair_id;
	cs_apply_game_config()
	{
		memset(this, 0, sizeof(cs_apply_game_config));
		chair_id = 255;
	}
}apply_game_config;

#pragma pack(pop)
#endif