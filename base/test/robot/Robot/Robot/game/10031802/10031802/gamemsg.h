#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_


#include <windows.h>
#include <stdarg.h> 


#define   ONE_SECOND_VALUE					1000						//一秒
#define	  PLAY_COUNT						180							// 游戏人数 
#define   CHOUMA_COUNT						5							//筹码个数		
#define   SESSION_COUNT						4							//场次
#define   PLAY_LEAST_COUNT				    1							//最少人数
#define   SEAT_COUNT						6							//座位数
#define   LINE_COUNT						100							//路线图最大个数
#define	  AREA_COUNT						3							//下注区域个数
#define   AREA_TOTAL_COUNT					15							//区域个数 * 筹码个数
#define   NOSITLENGTH						180							//无座玩家列表长度
//新增游戏状态，从12开始
//游戏定时器 1-50 
#define		TIME_CHECK_CUT					19							//断线检测
#define		TIME_NOTIFY_OUT					20							//玩家下注 
#define		TIME_SEND_CARD					21							//发牌
#define		TIME_USER_CHIP					24							//玩家操作时间
#define		TIME_AUTO_START					25							//自动开始时间



//小状态
#define MJ_STATUS_FREE				0									//空闲，等待准备
#define MJ_STATUS_CON_ANIMATION		1									//对抗+发牌墙动画
#define MJ_STATUS_CHIP				2									//下注
#define MJ_STATUS_SEND_CARD			3									//发牌
#define MJ_STATUS_END				9									//结束

//游戏状态定义
#define  GS_WAIT_SETGAME			0									//等待东家设置状态
#define  GS_WAIT_ARGEE				1									//等待同意设置
#define  GS_SEND_CARD				20									//发牌状态
#define  GS_PLAY_GAME				22									//游戏中状态
#define  GS_WAIT_NEXT				23									//等待下一盘开始

//游戏消息
//server to  client 
#define STC_SEND_GAME_STATUS			1006							//发送游戏小状态
#define STC_SEND_NOTIFY_OUT				1007							//通知玩家下注
#define STC_SEND_CARD_INFO				1008							//发送牌信
#define STC_SEND_UPDATE_SEAT			1009							//更新上座玩家信息
#define STC_SEND_GAME_RESULT			1010							//发送游戏结算信息 
#define STC_SEND_USER_AGREE				1011							//发送玩家准备消息	
#define STC_SEND_GAME_RECORD			1012							//发送牌局信息
#define STC_SEND_COUNT_DOWN				1013							//发送倒计时
#define STC_SEND_GAME_INFO				1017							//游戏配置信息
#define STC_SEND_CHIP_RESULT			1019							//发送下注结果
#define STC_SEND_SEAT_RESULT			1020							//发送上座结果
#define STC_SEND_NOSEATLIST_RESULT		1021							//发送无座列表结果




//发送共用消息
#define STC_UPDATE_USER_INFO			1101							//更新玩家信息	 
#define CTS_GET_GAME_CONFIG				1201							//申请获取游戏配置
#define CTS_UPDATE_USER_INFO			1202							//申请更新自己信息

//client to server
#define CTS_RECV_USER_AGREE				2004							//收到玩家准备消息
#define CTS_RECV_OUT_CARD				2005							//收到玩家下注消息		
#define CTS_RECV_SEAT_OPERATE			2007							//玩家上座操作
#define CTS_RECV_SHOW_NOSEAT_LIST		2008							//玩家显示无座列表


#define MAX_CARD_HAND				    3								//牌的个数
#define CARD_TOTAL_COUNT                52								//一副牌的总数

#pragma pack(push,1)

//游戏小状态
typedef struct sc_notify_status
{
	unsigned int		gameSmallStatus;					//游戏小状态
	int					seat_userid[SEAT_COUNT];			//上座玩家userid
	__int64				seat_user_score[SEAT_COUNT];		//上座玩家金币
	sc_notify_status()
	{
		memset(this, 0, sizeof(sc_notify_status));
	}
}notify_status;

//路线图
typedef struct game_user_line_map
{
	BYTE				cardtype;							//牌型
	BYTE				winArea;							//胜利区域 1红2黑
	game_user_line_map()
	{
		memset(this, 0, sizeof(game_user_line_map));
	}
	void initdata()
	{
		memset(this, 0, sizeof(game_user_line_map));
	}
}user_line_map;

//发牌消息
typedef struct sc_hand_card_info
{
	BYTE	hand_card[2][MAX_CARD_HAND];					//玩家手牌 0红方 1黑方
	BYTE	cardtype[2];									//牌类型
	sc_hand_card_info()
	{
		memset(this, 0, sizeof(sc_hand_card_info));
	}
}hand_card_info;

//通知玩家下注
typedef struct sc_notify_out_card
{
	unsigned char                  chair_id;				        //玩家位置
	unsigned int				   time_count;						//玩家操作时间 
	bool						   bCanChip[CHOUMA_COUNT];			//对应筹码是否可下注
	sc_notify_out_card()
	{
		memset(this, 0, sizeof(sc_notify_out_card));
		chair_id = 255;
	}
}notify_out_card;

//房间游戏配置
typedef struct stu_game_config
{
	unsigned int               tax;	                    //服务费     
	unsigned int			   bet_limit;				//下注限制
	unsigned int			   seat_coin;				//上座金额
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
	unsigned char		game_status;								//游戏状态
	unsigned char		sm_gameStatus;								//游戏小状态
	unsigned int		free_remain_time;							//剩余时间
	unsigned int		tax;										//服务费	
	unsigned int        chouma[CHOUMA_COUNT];						//当前下注筹码
	unsigned char		chair_id;									//当前玩家id
	__int64				total_score;								//当前玩家总分
	int					seatlist[SEAT_COUNT];						//上座列表 储存的时玩家id 012富豪榜 345神算子
	__int64				seat_total_score[SEAT_COUNT];				//上座列表玩家的总分
	bool				user_cut[SEAT_COUNT];						//上座列表玩家断线状态
	unsigned int		noSeatCount;								//无座列表人数
	user_line_map		lineMap[LINE_COUNT];						//路线图
	game_config			gameconfig;									//游戏配置		
	
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));
		chair_id = 255;
		memset(seatlist,255,sizeof(seatlist));
		for (int i = 0; i < LINE_COUNT; i++)
		{
			lineMap[i].initdata();
		}
	}

}game_base_info;

//更新玩家信息 
typedef struct stu_update_user_info
{
	unsigned char	    chair_id;						//玩家
	unsigned char       userCard[3];                    //玩家手牌
	__int64		        taozhu_count[3];                //单局玩家下注总值
	__int64				dici_count[3];                  //底池筹码
	__int64		        user_total_score;			    //玩家总分

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}

}update_user_info;

//游戏开始阶段
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//基础信息
	unsigned int		player_num;									//玩家人数 
	unsigned char       userCard[2][MAX_CARD_HAND];                 //红黑的牌
	unsigned char       cardtype[2];                                //牌的类型
	__int64				area_total_bet[AREA_COUNT];					//区域所有玩家总下注
	__int64				area_user_bet[AREA_COUNT];					//区域玩家下注
	int					area_total_chouma[AREA_TOTAL_COUNT];		//区域玩家总筹码	0-4红 5-9黑 10-14幸运一击
	int					area_user_chouma[AREA_TOTAL_COUNT];			//区域玩家筹码	0-4红 5-9黑 10-14幸运一击
	
	sc_game_status_play()
	{
		memset(this, 0, sizeof(sc_game_status_play));
	}
}game_status_play;

//通知玩家游戏结束
typedef struct sc_notify_game_result
{
	unsigned int			 gameStatus;								//游戏大状态
	unsigned int			 smGameStatus;								//游戏小状态
	__int64					 seat_win_score[SEAT_COUNT];				//上座玩家输赢分
	__int64					 seat_total_score[SEAT_COUNT];				//上座玩家总分 
	__int64					 win_score;									//自己输赢分
	__int64					 total_score;								//自己总分
	BYTE					 winArea;									//1红方胜 2黑方胜
	user_line_map			 lineMap[LINE_COUNT];						//路线图
	

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
	}
}notify_game_result;

//玩家下注信息
typedef struct user_total_chip_info
{
	__int64    chip_count[3][CHOUMA_COUNT];	    //下注数目  0红1黑2幸运一击  
	user_total_chip_info()
	{
		memset(chip_count, 0, sizeof(user_total_chip_info));
	}
	void initdata()
	{
		memset(chip_count, 0, sizeof(user_total_chip_info));
	}
}total_chip_info;

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

//玩家下注结果
typedef struct sc_user_out_chip_result
{	
	BYTE			chair_id;					 //下注玩家位置
	int				userid;						 //下注玩家id
	BYTE			area;						 //下注区域 1红 2黑 3幸运一击
	bool            isSeat;						 //是否是上座玩家 富豪榜 神算子
	int				xiazhumomey;				 //当前下注值
	__int64			area_total_bet[AREA_COUNT];  //区域所有玩家总下注
	__int64			area_user_bet[AREA_COUNT];	 //区域玩家下注
	BYTE			byresult;					 //下注返回码  0成功 1失败 2金币不足 3下注金额已达封顶
	sc_user_out_chip_result()
	{
		memset(this, 0, sizeof(sc_user_out_chip_result));
		chair_id = 255;
	}

}user_out_chip_result;

//上座结果
typedef struct sc_user_seat_result
{
	BYTE			byFlag;					  //1上座 2下座
	BYTE			chair_id;				  //玩家id
	BYTE			seatStation;			  //座位号 1-7
	sc_user_seat_result()
	{
		byFlag = 0;
		chair_id = 255;
		seatStation = 0;
	}

}user_seat_result;

//显示无座列表结果
typedef struct sc_user_show_noseat_result
{
	unsigned int    userid[NOSITLENGTH];		//玩家id
	long long		user_money[NOSITLENGTH];	//金币
	long long		total_bet[NOSITLENGTH];		//近十局下注总金额
	int				win_num[NOSITLENGTH];		//近十局获胜局数
	
	sc_user_show_noseat_result()
	{
		memset(this, 0, sizeof(sc_user_show_noseat_result));
	}

}user_show_noseat_result;

//发送倒计时
//下注倒计时
typedef struct sc_count_down
{
	int			  remain_time;									//剩余时间		
	sc_count_down()
	{
		remain_time = 0;
	}
}count_down;

//C-->S
//消息结构体

//玩家加注或跟注
typedef struct cs_user_out_chip_operate
{
	BYTE		    chair_id;						//玩家位置
	BYTE			area;							//下注区域 0红 1黑 2幸运一击
	INT				xiazhumoney;					//下注金额
	cs_user_out_chip_operate()
	{
		memset(this, 0, sizeof(cs_user_out_chip_operate));
		chair_id = 255;
	}
}user_out_chip_operate;

//上座操作
typedef struct cs_user_seat_operate
{
	BYTE			byFlag;					  //1上座 2下座
	BYTE			chair_id;				  //玩家id
	BYTE			seatStation;			  //座位号 1-7
	cs_user_seat_operate()
	{
		byFlag = 0;
		seatStation = 0;
		chair_id = 255;
	}

}user_seat_operate;

//玩家搓牌
typedef struct cs_user_rub_operate
{
	BYTE chair_id;				//搓牌玩家
	BYTE byArea;				//搓牌区域 1红 2黑
	cs_user_rub_operate()
	{
		chair_id = 255;
		byArea = 0;
	}
}user_rub_operate;

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

//玩家查看无座玩家列表
typedef struct cs_user_show_noseat_oper
{
	BYTE	chair_id;				
	cs_user_show_noseat_oper()
	{
		chair_id = 255;
	}
}user_show_noseat_oper;

#pragma pack(pop)

#endif