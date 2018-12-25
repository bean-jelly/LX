#pragma once
#ifndef __ROBOTMSG_H_
#define __ROBOTMSG_H_ 
#include <windows.h>
#include "gamemsg.h"

//操作掩码
#define	UG_HUA_MASK					0xF0			//1111 0000
#define	UG_VALUE_MASK				0x0F			//0000 1111

//扑克花色
#define UG_FANG_KUAI				0x00			//方块	0000 0000
#define UG_MEI_HUA					0x10			//梅花	0001 0000
#define UG_HONG_TAO					0x20			//红桃	0010 0000
#define UG_HEI_TAO					0x30			//黑桃	0011 0000
#define UG_NT_CARD					0x40			//主牌	0100 0000
#define UG_ERROR_HUA				0xF0			//错误  1111 0000

//扑克出牌类型
#define UG_ERROR_KIND					0				//错误
#define UG_ONLY_ONE						1				//单张
#define UG_DOUBLE						2				//对牌
#define UG_VAR_STRAIGHT					3				//变种顺子(A2345)顺子中最小
#define UG_STRAIGHT						4               //顺子,5+张连续牌
#define UG_FLUSH						5				//同花(非连)
#define UG_STRAIGHT_FLUSH				6               //同花顺,花色相同的顺子

#define UG_THREE						7				//三张
#define UG_THREE_ONE					8               //3 带 1
#define UG_THREE_TWO					9               //3 带 2
#define UG_THREE_DOUBLE					10				//3 带1对

#define UG_VAR_DOUBLE_SEC				11				//变种双顺(AA22)最小
#define UG_DOUBLE_SEC					12				//连对,2+个连续的对子

#define UG_VAR_THREE_SEC				13				//变种三顺(AAA222最小)
#define UG_THREE_SEQUENCE				14				//连三张，2+个连续的三张

#define UG_VAR_THREE_ONE_SEC			15				//变种三顺带一
#define UG_THREE_ONE_SEQUENCE			16              //2+个连续的三带一

#define UG_VAR_THREE_TWO_SEC			17				//变种三顺带二
#define UG_THREE_TWO_SEQUENCE			18				//2+个连续的三带二

#define UG_VAR_THR_DOUBLE_SEC_1			19				//变种三连张带对
#define UG_THREE_DOUBLE_SEC				20				//三连张带对

#define UG_VAR_THR_DOUBLE_SEC_2		    21				//变种蝴蝶(三顺带二顺)
#define UG_THREE_SEC_DOUBLE_SEC			22				//蝴蝶(三顺带二顺)

#define UG_FOUR_ONE						23				//四带一
#define UG_FOUR_TWO						24				//四带二张
#define UG_FOUR_ONE_DOUBLE				25				//四带一对
#define UG_FOUR_TWO_DOUBLE				26				//四带二对

#define UG_VAR_FOUR_SEC					27				//四顺
#define UG_FOUR_SEQUENCE				28				//四顺

#define UG_VAR_FOUR_ONE_SEC				29				//四带一顺
#define UG_FOUR_ONE_SEC					30				//四带一顺

#define UG_VAR_FOUR_TWO_SEC				31				//四带二顺
#define UG_FOUR_TWO_SEC					32				//四带二顺

#define UG_VAR_FOUR_ONE_DOUBLE_SEC		33				//四带对顺
#define UG_FOUR_ONE_DOUBLE_SEC			34				//四带对顺

#define UG_VAR_FOUR_TWO_DOUBLE_SEC		35				//四带二对顺
#define UG_FOUR_TWO_DOUBLE_SEC			36				//四带二对顺

#define UG_BOMB							39              //炸弹>=4張
#define UG_BOMB_SAME_HUA				40				//同花炸弹(在四副或以上的牌中出现)
#define UG_KING_BOMB					41				//王炸(最大炸弹)

#define KING_COUNT						2				//所有王的个数
#define CARD_MAX_COUNT                  54              //所有牌的数量 
#define MAX_COUNT						20									//最大数目

#define MAX_CARD_INDEX					80				//牌值最大索引

#define CARD_BULK_INDEX					18				//牌值大小索引
#pragma pack(push,1)
typedef struct str_operate_info
{
	BYTE		byCardCount;					//扑克数目
	BYTE		byCardList[HAND_CARD_MAX];		//扑克信息 
	BYTE		byCardType;						//扑克牌型
	BYTE		byExInfo;						//附加信息 1-先手 2-跟牌 
	str_operate_info()
	{
		byCardCount = 0x00;
		byCardType = UG_ERROR_KIND;
		byExInfo = 0x00;
		memset(byCardList, 0, sizeof(byCardList));
	}
	void clear()
	{
		byCardCount = 0x00;
		byCardType = UG_ERROR_KIND;
		byExInfo = 0x00;
		memset(byCardList, 0, sizeof(byCardList));
	}
}operate_info;

typedef struct str_Analysis_Result
{
	int total_outhand_count;
	int real_single_count;

	bool have_little_king;
	bool have_large_king;

	int boom_count;
	int single_count;//单张(包括三张,飞机带的)
	int double_count;
	int triple_count;
	BYTE boom_info[18];
	BYTE single_info[18];
	BYTE double_info[18];
	BYTE triple_info[18];

	BYTE SequenceListStart[4];
	BYTE SequenceCount[4];

	int iSeqCount;
	int iAloneSingle;	 //没有大牌管的单
	int iAloneDouble;	 //没有大牌管的对

	str_Analysis_Result()
	{
		total_outhand_count = 0;
		real_single_count = 0;
		have_little_king = false;
		have_large_king = false;

		boom_count = 0;
		single_count = 0;
		double_count = 0;
		triple_count = 0;
		memset(boom_info, 0, sizeof(boom_info));
		memset(single_info, 0, sizeof(single_info));
		memset(double_info, 0, sizeof(double_info));
		memset(triple_info, 0, sizeof(triple_info));

		memset(SequenceListStart, 0, sizeof(SequenceListStart));
		memset(SequenceCount, 0, sizeof(SequenceCount));

		iSeqCount = 0;
		iAloneSingle = 0;
		iAloneDouble = 0;
	}
}Analysis_Result;

typedef struct str_shun_info
{
	BYTE seq_count;  //牌型张数
	int  imax_pos;	 //顺子的序数
	BYTE type;		 //单 双  三顺
	BYTE count;		 //顺子主牌个数
	BYTE seq_end;	 //顺子的尾张大小
	str_shun_info()
	{
		seq_count = 0x00; 
		imax_pos = 255;
		type = 0x00;  
		count = 0x00;	
		seq_end = 0x00;
	}
}shun_info;

typedef struct str_card_info
{
	BYTE UserCard[HAND_CARD_MAX];               //自己的手牌
	BYTE UserCardCount;							//自己的手牌个数
	BYTE UpUserCard[HAND_CARD_MAX];				//上家的手牌
	BYTE UpUserCardCount;						//上家的手牌个数
	BYTE DownUserCard[HAND_CARD_MAX];			//下家的手牌
	BYTE DownUserCardCount;						//下家的手牌个数
	BYTE DeskCard[HAND_CARD_MAX];				//桌面的手牌
	BYTE DeskCardCount;							//桌面的手牌个数
	BYTE NTposition;							//庄家位置			0:自己  1:上家 2 :下家
	BYTE DeskPos;								//桌面牌玩家为之	0:自己 1:上家 2:下家
	str_card_info()
	{
		memset(UserCard, 0, sizeof(UserCard));
		UserCardCount = 0;
		memset(UpUserCard, 0, sizeof(UpUserCard));
		UpUserCardCount = 0;
		memset(DownUserCard, 0, sizeof(DownUserCard));
		DownUserCardCount = 0;
		memset(DeskCard, 0, sizeof(DeskCard));
		DeskCardCount = 0;
		NTposition = 0;
		DeskPos = 0;
	}
}Card_Info;

#pragma pack(pop)
#endif