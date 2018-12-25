#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#include <windows.h>
#include <stdarg.h>

#define PLAY_COUNT						4				//游戏人数 --参与最大的人数
#define HAND_CARD_NUM					4				//手牌数
#define ROW_COUNT						3				//公共牌个数(或行数)
#define COL_COUNT						2				//列数
#define ALL_CARD_COUNT					54				//最大牌数
#define MAX_TURN						6				//最大轮数

#define CARD_INIT						0x00			//牌值初始值
#define PLAYER_INIT						255				//位置初始值
#define	BLACK_JOKER						0x4E			//小王
#define RED_JOKER						0x4F			//大王

//游戏定时器 1-40
#define TIME_ONE_SECOND                 1000            //一秒
#define TIME_STARTGAME					1				//开始游戏倒计时
#define TIME_PICKCARD					2				//选牌倒计时
#define TIME_SENDCARD					3				//发牌倒计时
#define TIME_DELAY						4				//小局重开倒计时
#define TIME_DISSOLVE					5				//小局解散倒计时
#define TIME_CAL						6				//结算延时
#define TIME_NO_PERPARE					7				//检查是否可以开局

//游戏消息
//send data from server to  client MsgID>=2000
#define	STC_SEND_GAME_RESULT				2000		//结算消息
#define STC_SEND_UPDATE_USER_INFO			2001		//更新玩家信息
#define STC_SEND_USER_AGREE					2002		//发送玩家准备消息
#define STC_SEND_PUBLIC_INFO				2003		//发送公共牌信息
#define STC_SEND_HAND_CARD					2004		//发送手牌
#define STC_NOTIFY_PICK_CARD				2005		//发送选牌通知
#define STC_NOTIFY_PICK_RESULT				2006		//发送选牌结果
#define STC_SEND_START_COUNT  				2007		//开始倒计时按钮
#define STC_SEND_COMMON_MESSAGE  			2008		//发送通用信息
#define STC_SEND_NOTIFY_PREPARE				2009		//通知所有玩家准备
#define STC_SEND_ALL_USER_STATUS			2010		//通知客户端所有玩家状态

//recv data from client to  server MsgID>=1000
#define CTS_UPDATE_USER_PICK				1000		//处理玩家选牌

#pragma pack(push,1)

//游戏状态
enum GAME_STATUS
{
	STATUS_FREE = 1,									//空闲状态
	STATUS_BEGIN,										//开始
	STATUS_SEND_PUBLIC_CARD,							//发送公共牌阶段
	STATUS_SEND_HAND_CARD,								//发送手牌阶段
	STATUS_PICK,										//选牌阶段
	STATUS_COUNT,										//结算状态
	STATUS_WAIT_FINISH									//游戏结束状态
};

//玩家状态
enum USER_STATUS
{
	USER_SEE = 0,										//旁观
	USER_JOIN,											//参与游戏
	USER_RES											//休息
};

//游戏逻辑中，需要的结构体
//基本配置
typedef struct sg_game_basic_config
{
	unsigned int	player_mode;						//玩家模式  0:满人 1：全部同意
	int				time_pick_card;						//选牌时间
	int				time_restart;						//小局重开时间
	int				time_dissolve;						//小局未开始解散时间
	int				time_over_wait;						//小局结束等待时间
	int				look_status_limit;					//旁观次数限制
	sg_game_basic_config()
	{
		memset(this, 0, sizeof(sg_game_basic_config));
	}
}game_basic_config;

//房间游戏配置-根据游戏不同而设置
typedef struct sg_game_config
{
	game_basic_config	basic_config;						//基础配置
	bool				have_king;							//是否有癞子
	bool				show_card;							//对手牌可见
	int					player_min;							//最少人数
	int					base_point;							//底分
	int					tax;								//服务费
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
	sg_game_config()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
}game_config;

//玩家信息
typedef struct sg_game_user_info
{
	unsigned int	user_id;							//玩家ID 
	unsigned char	chair_id;							//玩家在桌上的位置
	unsigned int	game_status;						//游戏状态 0：旁观 1:参与游戏 2:休息
	bool			cut_status;							//断线状态 false：在线 true：断线
	bool			user_ready;							//玩家是否准备 false：未准备 true：准备
	long long		game_score;							//游戏积分(金币)
	char			nickname[60];						//玩家昵称
	char			head_url[200];						//头像路径
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = PLAYER_INIT;
	}
	sg_game_user_info()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = PLAYER_INIT;
	}
}game_user_info;

//每个玩家的每行游戏得分信息
typedef struct sg_score_info
{
	int row;
	int score;
	int card_type;
	unsigned char high_card;
	bool is_row_winner;
	sg_score_info()
	{
		score = 0;
		card_type = 0;
		is_row_winner = false;
	}
}score_info;

//自身摆牌
typedef struct sg_user_placed_card
{
	unsigned char card_value[ROW_COUNT][COL_COUNT];
	sg_user_placed_card()
	{
		memset(card_value, CARD_INIT, sizeof(card_value));
	}
	void init()
	{
		memset(card_value, CARD_INIT, sizeof(card_value));
	}
}user_placed_card;

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////S-->C////////////////////////////////////////////////
//游戏消息结构体 
//游戏状态基本信息
typedef struct sc_game_base_info
{
	game_config			gameconfig;							//游戏配置
	unsigned char		game_status;						//当前桌子状态
	unsigned char		cur_status;							//当前游戏阶段
	int					time_auto_out;						//自动出牌时间
	int					game_num;							//当前游戏局数
	sc_game_base_info()
	{
		game_status = USER_LOOK_STATE;
		cur_status = STATUS_FREE;
	}
}game_base_info;

//通知玩家游戏结束
typedef struct sc_notify_game_result
{
	user_placed_card	placed_card[PLAY_COUNT];					//摆牌
	unsigned char		card_type[ROW_COUNT][PLAY_COUNT];			//每行的玩家的牌型
	int					row_score[ROW_COUNT][PLAY_COUNT];			//每行的玩家得分
	unsigned char		row_winner[ROW_COUNT];						//每行加分的玩家
	int					user_win_score[PLAY_COUNT];					//每个玩家的赢分
	int					user_score[PLAY_COUNT];						//每个玩家的输赢分
	__int64				total_score[PLAY_COUNT];					//每个玩家的输赢分
	__int64				player_coins[PLAY_COUNT];					//玩家剩余金币
	unsigned char		win_user;									//最终赢家

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		win_user = PLAYER_INIT;
	}
	void init()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		win_user = PLAYER_INIT;
	}
}notify_game_result;

//游戏准备阶段
typedef struct sc_game_status_ready
{
	game_base_info		base_info;
	game_user_info		user_info[PLAY_COUNT];			//玩家信息
	bool				first_game;						//是否为第一局
	int					dissolve_time;					//好友房未开始游戏解散时间
	int					watch_times[PLAY_COUNT];		//旁观次数
	int					ready_time;						//准备时间
	sc_game_status_ready()
	{
		memset(user_info, 0, sizeof(user_info));
		first_game = false;
		dissolve_time = 900;
		memset(watch_times, 0, sizeof(watch_times));
	}
}game_status_ready;

//游戏开始阶段
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//基础信息
	game_user_info		user_info[PLAY_COUNT];						//玩家信息
	bool				first_game;									//是否为第一局
	unsigned int		player_num;									//参与游戏的人数
	unsigned char		public_card[ROW_COUNT];						//公共牌
	user_placed_card	placed_card[PLAY_COUNT];					//摆牌
	unsigned char		hand_card[PLAY_COUNT][HAND_CARD_NUM];		//手牌
	int					current_turn;								//当前回合
	int					time_remain;								//剩余时间
	notify_game_result  game_result;								//游戏结果
	sc_game_status_play()
	{
		player_num = 0;
		current_turn = 0;
		time_remain = 0;
		memset(public_card, 0, sizeof(public_card));
		memset(hand_card, 0, sizeof(hand_card));
	}
}game_status_play;

//通知玩家同意(开始-继续共用)
typedef struct sc_user_agree
{
	unsigned int		 user_id;									//玩家ID
	unsigned char		 chair_id;									//玩家位置
	bool				 bagree;									//是否同意
	sc_user_agree()
	{
		user_id = 0;
		chair_id = PLAYER_INIT;
		bagree = false;
	}
}user_agree;

//更新玩家信息
typedef struct sc_update_user_info
{
	unsigned int		user_id;									//玩家ID
	unsigned char		chair_id;									//玩家位置
	long long			score;										//分数
	sc_update_user_info()
	{
		user_id = 0;
		chair_id = PLAYER_INIT;
		score = 0;
	}
}update_user_info;

//手牌数据
typedef struct sc_hand_card_info
{
	unsigned char		card_count;									//手牌
	unsigned char		hand_card[HAND_CARD_NUM];					//玩家手牌
	sc_hand_card_info()
	{
		card_count = 0;
		memset(hand_card, CARD_INIT, sizeof(hand_card));
	}
}hand_card_info;

//公共牌信息
typedef struct sc_public_card_info
{
	unsigned char		replacecard[2];								//癞子牌
	unsigned char		card_count;									//公共牌个数
	unsigned char		public_card[ROW_COUNT];						//公共牌信息
	sc_public_card_info()
	{
		memset(replacecard, 0, sizeof(replacecard));
		card_count = 0;
		memset(public_card, CARD_INIT, sizeof(public_card));
	}
}public_card_info;

//通知选牌信息
typedef struct sc_notify_pickcard
{
	unsigned char		current_turn;
	sc_notify_pickcard()
	{
		current_turn = 255;
	}
}notify_pickcard;

//回复选牌结果信息
typedef struct sc_pickcard_result
{
	unsigned char		chair_id;									//玩家位置
	unsigned char		current_turn;								//当前回合
	unsigned char		pick_card;									//选牌牌值
	unsigned char		choose_row;									//行
	unsigned char		choose_col;									//列
	sc_pickcard_result()
	{
		chair_id = PLAYER_INIT;
		current_turn = 0;
		pick_card = CARD_INIT;
		choose_row = 255;
		choose_col = 255;
	}
}pickcard_result;

//通用消息
typedef struct  sc_notify_user_operate
{
	unsigned char		flag;										//动作标识   2:通知游戏结算阶段结束 3:通知玩家显示新手引导
	unsigned char		chair_id;									//玩家位置
	unsigned char		operate_val;								//操作值
	unsigned char		flag_ex[PLAY_COUNT];						//附加标识
	int					time;										//操作时间
	sc_notify_user_operate()
	{
		memset(this, 0, sizeof(sc_notify_user_operate));
	}
}notify_user_operate;

//游戏开始
typedef struct sc_set_start_game
{
	unsigned char	time_countdown;									//开始倒计时
	int				watch_times[PLAY_COUNT];						//旁观次数
	sc_set_start_game()
	{
		time_countdown = 10;
		memset(watch_times, 0, sizeof(watch_times));
	}
}set_start_game;

typedef struct sc_notify_prepare
{
	unsigned char		chair_id;									//玩家位置
	bool				is_prepare;									//是否准备          //true表示通知该玩家准备，false表示通知该玩家取消准备
}notify_prepare;

typedef struct sc_notify_user_status
{
	unsigned int		user_status[PLAY_COUNT];					//0:旁观 1:游戏中 2:空闲
	sc_notify_user_status()
	{
		memset(user_status, 2, sizeof(user_status));
	}
}notify_user_status;

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////C-->S////////////////////////////////////////////////
//消息结构体

//选牌信息
typedef struct cs_pickcard_info
{
	unsigned char		chair_id;									//玩家位置
	unsigned char		current_turn;								//当前回合
	unsigned char		pick_card;									//选牌牌值
	unsigned char		choose_row;									//行
	unsigned char		choose_col;									//列
	cs_pickcard_info()
	{
		chair_id = PLAYER_INIT;
		current_turn = 0;
		pick_card = CARD_INIT;
		choose_row = 255;
		choose_col = 255;
	}
}pickcard_info;

//玩家通用操作信息
typedef struct  cs_user_operate
{
	unsigned char		flag;										//动作标识	
	unsigned char		chair_id;									//玩家位置
	unsigned char		operate_val;								//操作值
	unsigned char		flag_ex[PLAY_COUNT];						//附加标识
}user_operate;

#pragma pack(pop)

#endif