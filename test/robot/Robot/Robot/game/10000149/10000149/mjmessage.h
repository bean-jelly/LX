#pragma once
#ifndef __MJMESSAGE_H_
#define __MJMESSAGE_H_
//麻将相关定义



#define  MAX_CARD_NUM				144									//最大牌数据
#define  NORMAL_CARD_NUM			25									//玩家出牌值 	
#define  CARD_COUNT					100									//牌张数 
#define  KING_CARD_NUM				10									//最大赖子个数
#define  MAX_OUT_CARD				40									//最大出牌数 
#define	 MASK_COLOR					0xF0								//花色掩码
#define	 MASK_VALUE					0x0F								//数值掩码
#define  MAX_FLOWER_NUM				20									//花牌最大数	
#define  USER_CARD_MAX				18									//玩家最大牌数（包含拦牌和手牌）
#define  HAND_CARD_COUNT			14									//玩家手牌数
#define  CARD_INDEX_NUM				0x40								//牌值索引
#define  JIANG_NUM					7									//最大将牌数
#define  BLOCK_MAX					5									//最大拦牌数

//拦牌类型
#define	 BLOCK_TYPE_ERR				0x00								//非法类型 
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
#define MJ_CARD_FLOWER_CHUN			0x38									// 春
#define MJ_CARD_FLOWER_XIA			0x39									// 夏
#define MJ_CARD_FLOWER_QIU			0x3A									// 秋
#define MJ_CARD_FLOWER_DONG			0x3B									// 冬
#define MJ_CARD_FLOWER_MEI			0x3C									// 梅
#define MJ_CARD_FLOWER_LAN			0x3D									// 兰
#define MJ_CARD_FLOWER_ZHU			0x3E									// 竹
#define MJ_CARD_FLOWER_JU			0x3F									// 菊

#pragma pack(push,1)

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
	str_game_mode()
	{
		bqidui = false;
		bmixjiang = false;
		blaizi = false;
		bchi = false;
		barrow = false;
		bwind = false;
		bpeng = false;
		bgang = false;
		b131 = false;
	}

}game_mode;

//拦牌数据 
typedef struct str_block_card_info
{
	unsigned char	block_type;	//拦牌类型
	unsigned char	block_card[4];//要拦的牌
	 
}block_card_info;
 


#pragma pack(pop)
#endif//__MJMESSAGE_H_
