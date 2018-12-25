#pragma once
#ifndef __MJMESSAGE_H_
#define __MJMESSAGE_H_
//麻将相关定义 
//#include <windows.h>
#include "memory.h"

typedef unsigned char	BYTE;


#define  MAX_CARD_NUM				144									//最大牌数据
#define  NORMAL_CARD_NUM			34									//玩家出牌值 	
#define  CARD_COUNT					136									//牌张数 
#define  KING_CARD_NUM				10									//最大赖子个数
#define  MAX_OUT_CARD				60									//最大出牌数 
#define	 MASK_COLOR					0xF0								//花色掩码
#define	 MASK_VALUE					0x0F								//数值掩码
#define  MAX_FLOWER_NUM				20									//花牌最大数	
#define  USER_CARD_MAX				18									//玩家最大牌数（包含拦牌和手牌）
#define  HAND_CARD_COUNT			14									//玩家手牌数
#define  CARD_INDEX_NUM				0x40								//牌值索引
#define  JIANG_NUM					7									//最大将牌数
#define  BLOCK_MAX					5									//最大拦牌数

//拦牌类型
#define	 BLOCK_TYPE_GAVE			0x00								//弃 
#define  BLOCK_TYPE_CHI				0x01								//吃
#define  BLOCK_TYPE_PENG			0x02								//碰
#define  BLOCK_TYPE_GANG			0x04								//杠
#define  BLOCK_TYPE_TING			0x08								//听
#define  BLOCK_TYPE_HU				0x10								//胡

//吃牌类型
#define BLOCK_TYPE_CHI_ERR			0x00								//类型错误
#define BLOCK_TYPE_CHI_FIS			0x01								//吃头
#define BLOCK_TYPE_CHI_MID			0x02								//吃中
#define BLOCK_TYPE_CHI_END			0x04								//吃尾

//杠牌类型
#define BLOCK_TYPE_GANG_ERR			0x00								//类型错误
#define BLOCK_TYPE_GANG_AN			0x01								//暗杠
#define BLOCK_TYPE_GANG_MING		0x02								//明杠
#define BLOCK_TYPE_GANG_BU			0x04								//补杠
  
//麻将状态
#define MJ_STATUS_FREE				0									//空闲，等待准备
#define	MJ_STATUS_BANKER			1									//定庄
#define MJ_STATUS_DICE				2									//打骰，
#define MJ_STATUS_SEND				3									//发牌, 发牌13张
#define MJ_STATUS_DRAW				4									//摸牌, 庄第14张或杠，摸的牌
#define MJ_STATUS_BLOCK				5									//拦牌
#define MJ_STATUS_BLOCK_WAIT		6									//拦牌出牌等待
#define MJ_STATUS_GIVE				7									//出牌
#define MJ_STATUS_DRAW_WAIT			8									//摸牌出牌等待
#define MJ_STATUS_END				9									//结束
#define MJ_STATUS_KING				10									//定鬼 
#define MJ_STATUS_FLOWER			11									//补花阶段

#define MJ_WIN_TYPE_HZ				0									//荒庄
#define MJ_WIN_TYPE_ZM				1									//自摸
#define MJ_WIN_TYPE_DP				2									//点炮
#define MJ_WIN_TYPE_QG				3									//抢杠
#define MJ_WIN_TYPE_GK				4									//杠开

//麻将牌值
//万
#define	MJ_CARD_EMPTY				0x00									//空牌
#define	MJ_CARD_WAN_1				0x01									//一万
#define	MJ_CARD_WAN_2				0x02									//二万
#define	MJ_CARD_WAN_3				0x03									//三万
#define	MJ_CARD_WAN_4				0x04									//四万
#define	MJ_CARD_WAN_5				0x05									//五万
#define	MJ_CARD_WAN_6				0x06									//六万
#define	MJ_CARD_WAN_7				0x07									//七万
#define	MJ_CARD_WAN_8				0x08									//八万
#define	MJ_CARD_WAN_9				0x09									//九万  
//条
#define	MJ_CARD_TIAO_1				0x11									//一条
#define	MJ_CARD_TIAO_2				0x12									//二条
#define	MJ_CARD_TIAO_3				0x13									//三条
#define	MJ_CARD_TIAO_4				0x14									//四条
#define	MJ_CARD_TIAO_5				0x15									//五条
#define	MJ_CARD_TIAO_6				0x16									//六条
#define	MJ_CARD_TIAO_7				0x17									//七条
#define	MJ_CARD_TIAO_8				0x18									//八条
#define	MJ_CARD_TIAO_9				0x19									//九条
//筒
#define	MJ_CARD_TONG_1				0x21									//一筒
#define	MJ_CARD_TONG_2				0x22									//二筒
#define	MJ_CARD_TONG_3				0x23									//三筒
#define	MJ_CARD_TONG_4				0x24									//四筒
#define	MJ_CARD_TONG_5				0x25									//五筒
#define	MJ_CARD_TONG_6				0x26									//六筒
#define	MJ_CARD_TONG_7				0x27									//七筒
#define	MJ_CARD_TONG_8				0x28									//八筒
#define	MJ_CARD_TONG_9				0x29									//九筒 
//风
#define MJ_CARD_WIND_EAST			0x31									//东风
#define MJ_CARD_WIND_SOUTH			0x32									//南风
#define MJ_CARD_WIND_WEST			0x33									//西风
#define MJ_CARD_WIND_NORTH			0x34									//北风
//箭牌
#define MJ_CARD_ARROW_HZ			0x35									//红中
#define MJ_CARD_ARROW_FC			0x36									//发财
#define MJ_CARD_ARROW_BB			0x37									//白板
//花 

#define MJ_CARD_FLOWER_MEI			0x38									// 梅
#define MJ_CARD_FLOWER_LAN			0x39									// 兰
#define MJ_CARD_FLOWER_ZHU			0x3A									// 竹
#define MJ_CARD_FLOWER_JU			0x3B									// 菊
#define MJ_CARD_FLOWER_CHUN			0x3C									// 春
#define MJ_CARD_FLOWER_XIA			0x3D									// 夏
#define MJ_CARD_FLOWER_QIU			0x3E									// 秋
#define MJ_CARD_FLOWER_DONG			0x3F									// 冬

#pragma pack(push,1)

//牌型类型
enum FAN_TYPE
{
	FAN_TYPE_PIHU = 0,		//平胡 0
	FAN_TYPE_QIDUI,			//七对 1
	FAN_TYPE_QINGYISE,		//清一色 2
	FAN_TYPE_HUNYISE,		//混一色 3
	FAN_TYPE_PPH,			//碰碰胡 4
	FAN_TYPE_MENQING,		//门清 5
	FAN_TYPE_DADIAO,		//大吊 6 
	FAN_TYPE_YJ,			//幺九 11
	FAN_TYPE_QF,			//全番--全由风和字组成 12
	FAN_TYPE_13Y,			//十三幺 13
	FAN_TYPE_QIDUI_1,		//一龙七对 14
	FAN_TYPE_QIDUI_2,		//二龙七对 15
	FAN_TYPE_QIDUI_3,		//三龙七对 16
	FAN_TYPE_SSL,			//13烂
	FAN_TYPE_QXSSL,			//七星13烂
	FAN_TYPE_COUNT_1,
	FAN_TYPE_COUNT_2,
	FAN_TYPE_COUNT_3,
	FAN_TYPE_COUNT_4,
	FAN_TYPE_COUNT_5,
	FAN_TYPE_COUNT_6,
	FAN_TYPE_COUNT_7,
	FAN_TYPE_COUNT_8,
	FAN_TYPE_COUNT_9,
	FAN_TYPE_COUNT_10,
	FAN_TYPE_COUNT_11,
	FAN_TYPE_COUNT_12,
	FAN_TYPE_COUNT_13,
	FAN_TYPE_COUNT_14,
	FAN_TYPE_COUNT_15,
	FAN_TYPE_COUNT_16,
	FAN_TYPE_COUNT_17,
	FAN_TYPE_COUNT_18,
	FAN_TYPE_COUNT_19,
	FAN_TYPE_COUNT_20,
	FAN_TYPE_COUNT_21,
	FAN_TYPE_COUNT_22,
	FAN_TYPE_COUNT_23,
	FAN_TYPE_COUNT_24,
	FAN_TYPE_COUNT_25,
	FAN_TYPE_COUNT
};

//设置游戏模式
typedef struct str_game_mode
{
	bool      bqidui;			//七对
	bool	  bmixjiang;		// 乱将
	bool	  blaizi;			//赖子
	bool	  bchi;				//吃
	bool	  barrow;			//吃箭
	bool	  bwind;			//吃风
	bool      bpeng;			//碰
	bool	  bgang;			//杠
	bool	  b131;				//13幺
	bool	  bqbk;				//13乱、全不靠
	str_game_mode()
	{
		memset(this, 0, sizeof(str_game_mode)); 
	}

}game_mode;



//拦牌数据类 
typedef struct str_block_data
{
	unsigned char card_value[4];	 //拦牌数据数据
	unsigned char chair_id;			 //拦谁的
	unsigned char blocked_card;      //被拦的牌
	unsigned char block_type;		 //拦牌类型
	unsigned char block_sub_type;	 //吃、杠子类型
	str_block_data()
	{
		memset(this, 0, sizeof(str_block_data));
	}
}BlockData;

//拦牌数据 
typedef struct str_block_card_info
{
	unsigned char	block_type;	//拦牌类型
	unsigned char	block_card[4];//要拦的牌
	str_block_card_info()
	{
		block_type = 0x00;
		block_card[0] = 0x00;
		block_card[1] = 0x00;
		block_card[2] = 0x00;
		block_card[3] = 0x00;
	}
	 
}block_card_info;
 

//听牌数据类型
typedef struct str_ting_data
{
	unsigned char  ting_card[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];//第一个位置表示出的牌,后面对应胡的牌

	str_ting_data()
	{
		memset(this, 0, sizeof(str_ting_data));
	}

}TingData;

//麻将配置信息
typedef struct str_mj_config
{
	bool							m_wan;									//有万字牌
	bool							m_tiao;									//有条字牌
	bool							m_tong;									//有筒字牌
	bool							m_wind;									//有风字牌
	bool							m_arrow;								//有箭字牌
	bool							m_flower;								//有花字牌
	bool							m_allow_dianpao;						//是否允许放炮
	bool							m_allow_qgh;							//是否允许抢杠胡 
	bool							m_allow_mul_win;						//是否允许一炮多响
	bool							m_must_ting;							//是否需要听牌可胡
	bool							m_allow_auto_win;						//是否允许自动胡牌
	bool							m_allow_block_check;					//是否允许拦牌后检测
	bool							m_allow_gang_after;						//是否允许后手杠
	bool							m_allow_ting_gang;						//是否允许听后杠
	bool							m_allow_gang;							//是否允许杠
	bool							m_an_gang_show;							//暗杠是否显示
	bool							m_allow_peng;							//是否允许碰 
	bool							m_allow_chi;							//是否允许吃
	bool							m_allow_chi_bwind;						//是否允许吃风
	bool							m_allow_chi_barrow;						//是否允许吃箭 
	bool							m_allow_flower;							//是否需要补花
	bool							m_allow_laizi;							//是否允许万能牌
	unsigned int					m_laizi_count;							//万能牌张数
	bool							m_allow_luanjiang;						//是否允许乱将
	bool							m_allow_qxd;							//是否允许七对
	bool							m_allow_131;							//是否允许十三幺
	bool							m_allow_qbk;							//是否允许全不靠
	bool							m_count_gang;							//计算杠分
	bool							m_follow_banker;						//是否跟庄
	unsigned int					m_huang_score;							//荒庄分数
	unsigned int					m_follow_score;							//跟庄分数
	bool							m_block_ting_info;						//可拦牌时是否发送听牌消息
	bool							m_out_ting_info;						//出牌时，是否发送听牌信息

	bool							m_gave_win;								//是否弃胡
	bool							m_gave_peng;							//是否弃碰
	bool							m_gave_chi;								//是否弃吃
	bool							m_allow_auto;							//是否允许自动操作
	//人数设置
	unsigned int					m_player_mode;							//玩家模式  0:满人 1：全部同意
	unsigned int					m_min_player;							//最少人数
	unsigned int					m_send_card_mode;						//发牌模式 0:(每次13)   1:(4,4,4,1模式)
	//解散模式
	unsigned int					m_free_mode;							//解散模式  0:所有人同意  1：大部分人同意
	//时间类
	unsigned int					m_time_dice;							//打骰时间
	unsigned int					m_time_send_card;						//发牌时间
	unsigned int					m_time_auto;							//自动出牌时间 
	unsigned int					m_time_king_card;						//翻万能牌时间
	unsigned int					m_time_send_one_card;					//发送一张牌时间
	unsigned int					m_time_user_out_card;					//玩家出牌时间
	unsigned int					m_time_user_draw_card;					//玩家摸牌时间

	str_mj_config()
	{
		memset(this, 0, sizeof(str_mj_config)); 
	}
}mj_config;

#pragma pack(pop)
#endif//__MJMESSAGE_H_
