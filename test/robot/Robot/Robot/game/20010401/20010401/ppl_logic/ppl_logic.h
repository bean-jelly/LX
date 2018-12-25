#pragma once

#ifndef __GAME_LOGIC_H_
#define __GAME_LOGIC_H_ 

//操作掩码
#define	UG_HUA_MASK						0xF0			//1111 0000
#define	UG_VALUE_MASK					0x0F			//0000 1111
#define ALL_CARD_COUNT					54				//最大牌数
#define CARD_COUNT						16				//牌值长度

#define MAX_COUNT                       3               //最大数目
#define ROW_COUNT						3
#define COL_COUNT						2

//扑克花色
#define UG_FANG_KUAI					0x00			//方块	0000 0000
#define UG_MEI_HUA						0x10			//梅花	0001 0000
#define UG_HONG_TAO						0x20			//红桃	0010 0000
#define UG_HEI_TAO						0x30			//黑桃	0011 0000
#define UG_NT_CARD						0x40			//主牌	0100 0000
#define UG_ERROR_HUA					0xF0			//错误  1111 0000

#define TYPE_ERROR						0				//类型错误
#define TYPE_SPEIAL						1				//杂花235
#define TYPE_HIGH_CARD					2				//高牌
#define TYPE_PAIR						3				//对子
#define TYPE_STR						4				//顺子
#define TYPE_FLUSH						5				//同花
#define TYPE_STR_FLUSH					6				//同花顺
#define TYPE_THREE						7				//三条
#define TYPE_MAX						8				//最大牌型


class CGameLogic
{
public:
	CGameLogic();
	~CGameLogic();
public:
	//混乱扑克
	bool RandCard(unsigned char iCard[], int iCardCount);
	//取出牌列表
	bool GetCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen);
	//添加牌列表
	int AddCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen);
	//设置为有癞子的模式
	int SetReplaceCard(bool b_is_have_king);
	//检查牌列表类型
	bool CheckCardListType(unsigned char iCard[], int iCardCount, int &TypeValue, int &Type, unsigned char &HighCard);
	//获取扑克权值
	unsigned char GetCardNum(unsigned char iCard);
	//获取扑克花色
	unsigned char GetCardColor(unsigned char iCard){ return iCard & UG_HUA_MASK; }
	//获取高牌
	unsigned char GetHighCard(unsigned char iCard[], int iCardCount);
	//扑克中是否存在指定扑克点数大小的牌
	bool IsHaveCardNum(unsigned char iCard[], unsigned char iCardCount, unsigned char iCardValue);
	//排列扑克
	bool SortCard(unsigned char iCard[], unsigned char iCardCount, bool bSortA = false);
	//翻转列表
	bool Reverse(unsigned char iCard[], unsigned char iCardCount);
	//获取牌型
	unsigned char GetCardType(unsigned char iCard[], unsigned char iCardCount = MAX_COUNT);
	//比较两张扑克的大小 0相等 1大于  2小于
	unsigned char CompareCardValue(unsigned char iCard_First, unsigned char iCard_Second, bool bCompareHua);
	//获取对子中最大的牌值
	void GetDoubleMaxNums(unsigned char iCard[], unsigned char iCardCount, unsigned char& max, unsigned char& single);
	//比较大小 0相等 1大于 2小于
	unsigned char CompareCardData(unsigned char iCard_First[], unsigned char iCard_Second[], unsigned char type1, unsigned char type2, bool flag_ex, unsigned char iCardCount = MAX_COUNT);
	//检测是否为癞子牌
	bool IsReplaceCard(unsigned char byCard);
	//删除扑克
	int RemoveCard(unsigned char iRemoveCard[], int iRemoveCount, unsigned char iCardList[], int iCardCount);
	//删除单个扑克
	int RemoveCard(unsigned char iRemoveCard, unsigned char iCardList[], int iCardCount);
	//清除 0 位扑克
	int RemoveNummCard(unsigned char iCardList[], int iCardCount);

	//检查是否三条
	bool CheckThree(unsigned char iCard[], int iCardCount);
	//检查是否顺子
	bool CheckStraight(unsigned char iCard[], int iCardCount);
	//是否是顺子123
	bool CheckStraight123(unsigned char iCard[], unsigned char iCardCount);
	//是否是顺子QKA
	bool CheckStraightQKA(unsigned char iCard[], unsigned char iCardCount);
	//检查是否同花色
	bool CheckSameColor(unsigned char iCard[], unsigned char iCardCount);
	//检查是否同花（非顺子）
	bool CheckFlush(unsigned char iCard[], unsigned char iCardCount);
	//检查是否同花顺
	bool CheckStraightFlush(unsigned char iCard[], unsigned char iCardCount);
	//检查是否对子
	bool CheckPair(unsigned char iCard[], int iCardCount);
	//检查是否杂花
	bool CheckTypeSpecial(unsigned char iCard[], int iCardCount);
private:
	bool m_have_king;												//是否有癞子
};

#endif//__GAMELOGIC_H_