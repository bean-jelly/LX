#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#include "mjlogic\mjmessage.h"

#define PLAY_COUNT						4						             // 游戏人数 
#define MAX_ZHA_MA						108									 //最大扎码数
//新增游戏状态，从12开始

//新增录像记录，从 0x30开始
//游戏定时器 1-50 
#define		TIME_SEND_CARD					1							//发牌
#define		TIME_SEND_ONE_CARD				2							//摸牌  
#define		TIME_FREE_DESK					3							//解散包间
#define		TIME_SEND_KING_CARD				4							//万能牌
#define		TIME_USER_OUT_CARD				5							//玩家出牌定时器
#define     TIME_USER_BLOCK_OPER            6                           //玩家拦牌操作
#define		TIME_NOTIFY_BLOCK				9							//通知玩家出牌 
#define		TIME_AUTO_OUT_CARD				14							//自动出牌
#define		TIME_DRAW_CARD					15							//玩家摸牌
#define		TIME_NOTIFY_OUT					16							//通知出牌
#define	    GAME_TIME_JS_YANSHI			    17	                		//延时计时器
#define	    GAME_TIME_FORCE_LEAVE         	18	     			        //强制踢人
#define	    GAME_TIME_FREE_DESK         	19	     			        //强制踢人
#define     GAME_TIME_START_GAME            20                          //开始倒计时
#define     TIME_ONE_SECOND                 1000                        //一秒
#define		TIME_SEND_DICE					21							//机器人摇骰子

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
#define STC_SEND_USER_TING_RESULT		1019						//发送通知玩家听牌
#define STC_SEND_KING_CARD				1020						//发送通知玩家万能牌 
#define STC_SEND_NOTIFY_FLOWER			1021						//发送通知补花消息	
#define STC_SEND_OTHER_BLOCK			1022						//等待其他玩家拦牌操作
#define STC_SEND_AUTO_PALY   			1023						//玩家托管结果
#define STC_SEND_START_COUNT  			1024						//开始倒计时按钮

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
#define CTS_RECV_USER_TING				2006						//收到玩家听牌消息
#define CTS_RECV_AUTO_PLAY				2007						//收到玩家托管消息

#pragma pack(push,1)

//玩家状态
enum USER_STATUS
{
	USER_SEE = 0,				//旁观
	USER_JOIN,					//参与游戏
	USER_REST,                  //休息
};

//房间游戏配置
typedef struct stu_game_config
{
	bool				qidui;				//七对
	bool                sihong;             //四红全中码
	unsigned int        zhama;              //扎码
	unsigned int        wugui;              //无鬼
	bool                zhuapao;            //无鬼可抓炮
	unsigned int        guipai;             //鬼牌
	unsigned int        difen;              //底分
	unsigned int        startcount;         //开始人数
	unsigned int        card_length;        //牌墙个数
	stu_game_config()
	{
		memset(this, 0, sizeof(stu_game_config));
	}
}game_config;

//S-->C
//游戏消息结构体 
//游戏状态基本信息
typedef struct sc_game_base_info
{
	unsigned char		game_status;				//游戏状态
	unsigned int		user_id[PLAY_COUNT];		//玩家id
	long long			user_score[PLAY_COUNT];		//玩家分数
	bool				user_cut[PLAY_COUNT];		//玩家状态
	unsigned int		total_play_count;
	unsigned int		cur_play_count;
	unsigned char		mj_status;					//麻将阶段状态
	unsigned char		banker_station;				//庄家位置
	unsigned char		my_station;					//自己的位置	
	unsigned char       auto_out_time;              //自动出牌时间
	unsigned char       auto_block_time;            //自动拦牌时间
	game_config			gameconfig;					//游戏配置		18 
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));
		banker_station = 255;
		my_station = 255;
	}

}game_base_info;
//游戏中玩家信息
typedef struct Stu_GameUserItem
{
	unsigned int	player_id;						//玩家ID
	BYTE            chairid;                        //座位id
	BYTE            bsex;                           //性别(1男2女)
	bool			look_status;					//是否是旁观状态
	bool            bplaygame;                      //是否在游戏中
	bool            brest;                          //是否休息状态
	bool			cut_status;						//是否是断线状态
	long long		total_score;					//玩家总分
	long long		player_money;					//佘额(货币)
	long long		player_coins;					//金币(货币)
	long long		player_lottery;					//彩票(货币)
	long long       servercount;                    //服务费
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;
//游戏准备阶段
typedef struct sc_game_status_ready
{
	game_base_info		base_info;
	GameUserItem        useritem[PLAY_COUNT];                       //游戏中玩家信息
	bool				user_ready[PLAY_COUNT];
	bool                bfirst;                                     //是否第一局
	unsigned int		dissolve_time;				                //解散时间 -分钟  
	BYTE                bycreater;                                  //房主
	sc_game_status_ready()
	{
		memset(user_ready, 0, sizeof(user_ready));
	}

}game_status_ready;

//其他人操作
typedef struct sc_notify_wait_other_block
{
	unsigned char			chair_id;		//玩家位置
	unsigned char			block_level;	//操作等级
	sc_notify_wait_other_block()
	{
		chair_id = 255;
		block_level = 0x00;
	}
}notify_wait_other_block;

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
	unsigned char	draw_card;						//玩家摸的牌 
	unsigned int	flower_count;					//花牌个数
	unsigned char	flower_card[MAX_FLOWER_NUM];	//花牌数
	sc_game_user_info()
	{
		memset(this, 0, sizeof(game_user_info));
	}
}game_user_info;
//更新玩家信息 
typedef struct stu_update_user_info
{
	unsigned char	chair_id;						//玩家
	game_block_data	block_info[BLOCK_MAX];			//玩家拦牌 
	unsigned int	hand_card_count;				//玩家手牌数
	unsigned int	out_card_count;					//玩家出牌数	
	unsigned int	block_count;					//玩家拦牌数	
	unsigned char	hand_card[HAND_CARD_COUNT];		//玩家手牌
	unsigned char	out_card[MAX_OUT_CARD];			//玩家出牌
	unsigned char	draw_card;						//玩家摸的牌
	unsigned int	flower_count;					//花牌个数
	unsigned char	flower_card[MAX_FLOWER_NUM];	//花牌数

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}
}update_user_info;

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

//游戏开始阶段
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//基础信息
	game_user_info		user_info[PLAY_COUNT];						//玩家信息  
	notify_ting			notify_ting;
	unsigned int		wall_count;									//牌墙数量
	unsigned int		gang_all_count;								//杠的总数
	unsigned char		dice[2];									//骰子
	unsigned char		cur_operate_station;						//当前操作玩家
	unsigned char		cur_gams_stats;								//当前游戏状态
	unsigned char		cur_block_level;							//当前拦牌等级
	unsigned char		cur_check_user;								//当前被拦的人
	unsigned char		cur_check_card;								//当前被拦的牌
	bool				bdraw_stats;								//是否是摸牌  
	unsigned char	    block_info[BLOCK_MAX][5];					//可以拦的牌     
	bool				ting[PLAY_COUNT];							//是否听
	bool				chose_ting[PLAY_COUNT];						//是否选择听
	unsigned char		ting_list[NORMAL_CARD_NUM + 1];				//听的牌
	unsigned char		ting_count[NORMAL_CARD_NUM + 1];			//听牌的个数 
	bool				canting;									//是否能听
	unsigned int		block_index;								//拦牌索引
	unsigned int		player_num;									//玩家人数 
	bool                bfirst;                                     //是否第一局
	bool                bauto[PLAY_COUNT];                          //是否托管
	bool                bautowin[PLAY_COUNT];                       //是否自动胡牌
	unsigned char       nowtime;                                    //当前操作剩余时间
	GameUserItem        useritem[PLAY_COUNT];                       //游戏中玩家信息
	BYTE                bycreater;                                  //房主
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
	INT64           server_money;           //服务费
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
		memset(dice, 0, sizeof(dice));
	}
}dice_info;
//万能牌消息
typedef struct sc_king_card_info
{
	unsigned char  card;
	unsigned char  kingcard[2];
	sc_king_card_info()
	{
		card = 0x00;
		kingcard[0] = 0x00;
		kingcard[1] = 0x00;
	}
}king_card_info;
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
	unsigned char   card_index;             //牌位置
	bool            bting;                  //玩家是否听牌（显示自动摸打）
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
	unsigned char   block_sub_type;                     //拦牌子类型(无子类型则为0x00)
	sc_notify_block_info()
	{
		memset(this, 0, sizeof(sc_notify_block_info));
		checked_id = 255;
	}
	void  cleardata()
	{
		memset(this, 0, sizeof(sc_notify_block_info));
		checked_id = 255;
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
	unsigned char   card_index[4];          //牌位置
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
	game_block_data			 block_info[PLAY_COUNT][BLOCK_MAX];			//玩家拦牌 
	int						 win_type;									//胡牌方式 0：荒庄   1：自摸 2：放炮  3：抢杠 4:杠开
	bool					 win[PLAY_COUNT];							//胡牌玩家
	unsigned char			 hand_card[PLAY_COUNT][HAND_CARD_COUNT];	//玩家手牌
	unsigned char			 win_card;									//胡的牌
	unsigned char			 dianpao_id;								//放炮的玩家
	bool                     ballking[PLAY_COUNT];                      //是否全码
	unsigned char			 luck_card[MAX_ZHA_MA];			            //扎码牌值 
	unsigned int			 user_luck_count[PLAY_COUNT];				//玩家中码个数
	int						 luck_score[PLAY_COUNT];					//中码分
	int						 gang_score[PLAY_COUNT];					//玩家杠分
	int						 fan_score[PLAY_COUNT];						//玩家胡牌番数 
	int						 win_score[PLAY_COUNT];						//输赢分
	long long				 total_score[PLAY_COUNT];					//玩家总分 
	int						 mgang_count[PLAY_COUNT];					//接杠次数
	int						 agang_count[PLAY_COUNT];					//暗杠次数
	int						 bgang_count[PLAY_COUNT];					//明杠次数
	int						 fgang_count[PLAY_COUNT];					//放杠次数 
	bool					 win_fan_xing[PLAY_COUNT][FAN_TYPE_COUNT];	//胡的牌型

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		dianpao_id = 255;
	}

	void init()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		dianpao_id = 255;
	}
}notify_game_result;

//补花
typedef struct sc_notify_flower
{
	unsigned char	chair_id;			//玩家
	unsigned char	flowercard;			//花牌
	unsigned char	newcard;			//更换的牌
}notify_flower;


//听牌信息
typedef struct sc_user_ting_info
{
	bool		ting;
	unsigned char ting_list[NORMAL_CARD_NUM + 1];
	unsigned char ting_count[NORMAL_CARD_NUM + 1];

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

//设置托管
typedef struct sc_auto_paly_res
{
	unsigned char	chairid;       //玩家座位号
	BYTE            bytype;        //状态（1主动，2被动）
	bool            bautowin;      //是否托管
	sc_auto_paly_res()
	{
		chairid = 255;
		bautowin = false;
	}
}auto_paly_res;

//开始按钮
typedef struct sc_start_btn
{
	unsigned char	chairid;       //玩家座位号
	sc_start_btn()
	{
		chairid = 255;
	}
}start_btn;

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
	unsigned char   card_index;             //牌位置
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
	unsigned int	block_index;
	unsigned char   card_index[4];          //牌位置
	cs_user_block_operate()
	{
		block_index = 0;
		chair_id = 255;
		opreate_type = 0x00;
		out = false;
		memset(card_value, 0, sizeof(card_value));
	}
}user_block_operate;

//玩家出牌消息
typedef struct cs_user_ting_operate
{
	unsigned char   chair_id;				//玩家位置
	unsigned char	card_value;				//听牌打出的牌

}user_ting_operate;

//设置好牌
typedef struct cs_set_next_wall
{
	unsigned char	card_value;
	cs_set_next_wall()
	{
		card_value = 0x00;
	}
}set_next_wall;

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

//设置托管
typedef struct cs_set_auto_paly
{
	unsigned char	chairid;       //玩家座位号
	BYTE            bytype;        //托管类型(1主动，2被动)
	bool            bauto;      //是否托管
	cs_set_auto_paly()
	{
		chairid = 255;
		bytype = 0;
		bauto = false;
	}
}set_auto_paly;

//游戏开始
typedef struct cs_set_start_game
{
	unsigned char	watchcount[PLAY_COUNT];       //旁观局数
	cs_set_start_game()
	{
		memset(watchcount, 0, sizeof(watchcount));
	}
}set_start_game;

#pragma pack(pop)

#endif