
#pragma once
//#include "StdAfx.h"

#ifndef __ROBORT_LOGIC_H_
#define __ROBORT_LOGIC_H_


typedef  unsigned char  BYTE;
typedef   int			BOOL;
typedef  unsigned int	UNIT;

#define    TRUE      1
#define    FALSE     0

#include <vector>
//#include <WinDef.h>
#include "gamemsg.h"
#include "robotmsg.h"


//扑克分析结构	（只用于服务器对比扑克）


class CRecordTool
{
public:
	CRecordTool(void);
	virtual	~CRecordTool();

public:
	//初始化记牌器
	bool InitLastCard();
	//设置记牌器牌值
	bool SetLastCard(CRecordTool src);
	//设置记牌器牌值
	bool SetLastCard(BYTE card_value[], unsigned int card_count);
	//删除牌
	UNIT RemoveCard(BYTE out_card_list[], unsigned int out_card_count);
	////是否有某张牌
	bool IsHaveCard(BYTE byCard, unsigned int count = 1);
	////是否有炸弹
	int IsHaveBoom(BYTE start_card = 0x00);
	//排序
	bool Sort();
	//获取所有牌值
	UNIT GetAllCard(BYTE byCardList[]);
	//获取所有余牌牌值index
	UNIT GetAllCardIndex(BYTE byIndex[]);
	//获取剩余的最大的牌
	BYTE GetLastMaxCard(UINT count = 1);
	//是否有大的过的顺子
	bool IsHaveBigShun(BYTE start_val, BYTE count);
	//清理数据
	bool ClearAllData();

private:
	unsigned char				m_LastCard[CARD_MAX_COUNT];
	unsigned int				m_LastCount;
};


class COperateRecord
{
public:
	COperateRecord(void);
	virtual	~COperateRecord();

public:
	//添加一组出牌信息
	bool AddOutCardInfo(BYTE byCardList[], BYTE	byCardCount, BYTE byCardType, BYTE byFirstOut);
	//添加一组过牌信息
	bool AddPassCardInfo(BYTE byCardList[], BYTE byCardCount, BYTE byCardType, BYTE byFriend);
	//获取此牌型上次出的牌值
	bool GetLastOutCardbyType(BYTE byCardType, BYTE byCardList[], BYTE byCardCount);
	//获取此牌型上次出的牌型
	BYTE GetLastOutType();
	//获取此牌型上次不跟的牌值
	bool GeteLastPassCardbyType(BYTE byCardType, BYTE byCardList[], BYTE &byCardCount);
	//获取此牌型出牌的次数
	unsigned int GetCountOutbyType(BYTE byCardType);
	//获取此牌型不跟的次数
	unsigned int GetCountPassbyType(BYTE byCardType);
	//获取连续同类型不跟次数
	unsigned int GetContinuePassCount(BYTE iBaseShape);
	//清空所有数据
	bool ClearAllData();

private:
	operate_info				m_out_info[HAND_CARD_MAX];
	unsigned int				m_out_count;
	operate_info				m_pass_info[2 * HAND_CARD_MAX];
	unsigned int				m_pass_count;
	unsigned int				m_continue_pass_count;//连续同类型牌不跟次数
};

//扑克类逻辑
class CGameLogic
{
	//变量定义
private:
	BYTE			m_bSortCardStyle;       //排序方式
	BOOL			m_bKingCanReplace;		//王是否可当
	game_config		gameconfig;				//游戏配置
	bool			m_bFeedCard;			//是否喂牌
private:
	//手数分析temp数据
	int m_total_outhand_count;
	int m_real_single_count;

	bool m_have_little_king;
	bool m_have_large_king;

	int m_boom_count;
	int m_single_count;//单张(包括三张,飞机带的)
	int m_double_count;
	int m_triple_count;
	BYTE m_boom_info[18];
	BYTE m_single_info[18];
	BYTE m_double_info[18];
	BYTE m_triple_info[18];

	BYTE m_SequenceListStart[4];
	BYTE m_SequenceCount[4];//0x15表示单顺五连，0x24表示双顺4连，0x32表示飞机2连
	//函数定义
public:
	//构造函数		
	CGameLogic(void);
	//析构函数
	virtual ~CGameLogic();

	//功能函数（公共函数）
public:
	//[设置相关]
	void InitAllData();
	void SetGameRule(game_config config);
	//清除手数相关数据
	void ClearTempAnalyseInfo();
	//获取扑克数字
	int GetCardNum(BYTE iCard);
	//获取扑克花色(默认为真实花色)
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua = TRUE);
	//获取扑克相对大小(默认为牌大小,非排序大小)
	int  GetCardBulk(BYTE iCard, bool bExtVal = false);
	//获取扑克牌通过相对大小
	BYTE GetCardByValue(int iCardValue);
	//設置王可以當牌
	void SetKingCanReplace(BOOL bKingCanReplace = false){ m_bKingCanReplace = bKingCanReplace; }
	//獵取王是否可以當牌
	BOOL GetKingCanReplace(){ return m_bKingCanReplace; }
	//设置排序方式
	void SetSortCardStyle(BYTE SortCardStyle){ m_bSortCardStyle = SortCardStyle; }
	//获取排序方式
	BYTE GetSortCardStyle(){ return m_bSortCardStyle; }
	//牌值合法性检验
	bool IsValidataCard(BYTE byCard);
	//[排序]
public:
	//排列扑克,按大小(保留系统序例)
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount, BOOL bSysSort = FALSE);
	//反转牌顺(从低->高)
	BOOL ReverseCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount);
	//按牌型排序
	BOOL SortCardByStyle(BYTE iCardList[], BYTE iCardCount);
public:
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount);		//bHaveKing表示是否有大小猫,false无,ture有

	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
	//清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);
public://[辅助函数]
	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);
	//是否为同一数字牌
	BOOL IsSameNumCard(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//是否为同一花色
	BOOL IsSameHuaKind(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//查找 >=4 炸弹的数量炸弹基数
	BYTE GetBombCount(BYTE iCardList[], int iCardCount, int iNumCount = 4, bool bExtVal = false);
	//获取指定大小牌个数
	BYTE GetCountBySpecifyNumCount(BYTE iCardList[], int iCardCount, int Num);
	//获取指定牌个数
	BYTE GetCountBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCard);
	//获取指定牌面值牌的个数
	BYTE GetCountBySpecifyBulk(BYTE byCardList[], int iCardCount, int byCardBulk);
	//获取指定牌张数牌大小(队例中只能够有一种牌的张数为iCount,不然传出去的将是第一个指定张数的值)
	BYTE GetBulkBySpecifyCardCount(BYTE iCardList[], int iCardCount, int iCount);
	//是否为某指定的顺子(变种顺子)
	BOOL IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount);
	//是否为某指定的顺子
	BOOL IsSequence(BYTE iCardList[], int iCardCount, int iCount);

	int GetSequenceXLen(BYTE iCardList[], int iCardCount, int iCount, int iStartPos);
	//提取指定的牌
	BYTE TackOutBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCardBuffer[], int &iResultCardCount, BYTE bCard);
	//提取某张指定数字的牌
	BOOL TackOutCardBySpecifyCardNum(BYTE iCardList[], int iCardCount, BYTE iBuffer[], int &iBufferCardCount, BYTE iCard, BOOL bExtVal = false);
	//提取所有符合条件的牌,单张,对牌,三张,4炸弹牌型
	int  TackOutBySepcifyCardNumCount(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BYTE bCardNum, bool bExtVal = false);
	//提取指定花色牌
	int  TackOutByCardKind(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BYTE iCardKind);
	//拆出(将手中牌多的拆成少的)
	int  TackOutMuchToFew(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], int &iBufferCardCount, BYTE iCardMuch, BYTE iCardFew);
	//查找大于iCard的单牌所在iCardList中的序号
	BYTE GetSerialByMoreThanSpecifyCard(BYTE iCardList[], int iCardCount, BYTE iCard, BYTE iBaseCardCount, bool bExtValue = false);
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int  GetSerialBySpecifyCard(BYTE iCardList[], int iStart, int iCardCount, BYTE iCard);
	//获取指定顺子中牌点最小值(iSequence 代表顺子的牌数最多为
	BYTE GetBulkBySpecifySequence(BYTE iCardList[], int iCardCount, int iSequence = 3);
	//获取指定顺子中牌点最大值变种顺子
	BYTE GetBulkBySpecifyVariationSequence(BYTE iCardList[], int iCardCount, int iSequence = 3);
	//查找最小 (1) or 最大 (255) 牌
	int	 GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax, bool bExtVal = false);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//[牌型相关]
public:
	//获取牌型
	BYTE GetCardShape(BYTE iCardList[], int iCardCount);
	//是否单牌
	inline BOOL IsOnlyOne(BYTE iCardList[], int iCardCount) { return iCardCount == 1; };
	//是否对牌
	BOOL IsDouble(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//3 带 1or2(带一对带二单张或带一单张
	BOOL IsThreeX(BYTE iCardList[], int iCardCount, int iX/*1or2*/, bool bExtVal = false);
	//王炸
	BOOL IsKingBomb(BYTE iCardList[], int iCardCount);
	//4+张牌 炸弹
	BOOL IsBomb(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//同花炸弹
	BOOL IsBombSameHua(BYTE iCardList[], int iCardCount);
	//同花(非顺子)
	BOOL IsFlush(BYTE iCardList[], int iCardCount);
	//是否是同花顺
	BOOL IsStraightFlush(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//变种单甩
	BOOL IsVariationStraight(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//单甩
	BOOL IsStraight(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//变种对甩
	BOOL IsVariationDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//对甩 //连对?
	BOOL IsDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//是否变种是连续的三带X(0,1,2,3)
	BOOL IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/, bool bExtVal = false);
	//是否是连续的三带X(0,1,2,3)
	BOOL IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/, bool bExtVal = false);
	//是否三顺带二顺(蝴蝶)
	BOOL IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//是否三顺带二顺(蝴蝶)
	BOOL IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//四带一或者四带二
	BOOL IsFourX(BYTE iCardList[], int iCardCount, int iX/*1or 2*/);//单张1,任意张2,一对子,2对4
	//是否变种四带X顺
	BOOL IsVariationFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX);
	//四带一或者四带二的顺子
	BOOL IsFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX);
	//[出牌相关]
public:
	//是否可以出牌
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, bool bFirstOut = false);
	//查找比当前出牌大的
	BOOL TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, bool bSingleOrder);
	//[提取牌]
public:
	//提取单个的三带1 or 2or 3(单,一对,或二单张)
	BYTE TackOutThreeX(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int  xValue);
	//提取2个以上连续的三带1,2
	BOOL TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue);
	//提取2个以上连续的三带1,2
	BOOL TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount);
	//获取顺子中最小位置值(xSequence表示默认单顺)
	int GetSequenceStartPostion(BYTE iCardList[], int iCardCount, int xSequence = 1);
	//提取单张的顺子,连对顺子,连三顺子
	BOOL TackOutSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xSequence, BOOL bNoComp = false);
	//提取同花顺
	BOOL TackOutStraightFlush(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount);
	//提取所的炸弹
	BOOL TackOutAllBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, int iNumCount = 4);
	//提取炸弹(张数默认为4)
	BOOL TackOutBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, int iNumCount = 4);
	//提取王炸
	BOOL TackOutKingBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount);
	//拆大桌面牌
	BOOL TackOutCardByNoSameShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount);
	//用大的牌牌大桌面上的牌
	BOOL TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount);
	//查找三连牌中最小的那个三张的牌点
	int SearchMinThreeSeq(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//检测指定数组是否是子集关系
	bool CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount);
	//是否是比较牌型
	bool IsMustCallShape(BYTE cardlist[], BYTE count);
	//获取选择牌型
	void GetchangeCardList(BYTE byCardList[], BYTE byCardCount, BYTE byReslut[]);
	//检测是否是一副牌
	bool IsASetCardlist(BYTE byCardList[], BYTE byCardCount);
public:
	//牌型叫分参考信息
	BYTE InfoForCallScore(BYTE byCardList[], BYTE byCardCount);
	//分析手牌
	Analysis_Result AnalyzeHandCard(BYTE byCardList[], BYTE byCardCount, bool bSplitBomb = true, bool bSaveResult = true);

	//查找顺子
	void FindSequence(BYTE byTempCardList[], BYTE bySequenceList[], BYTE bySequenceCount[], bool bSplitBomb);
	//获取手牌中的对应牌值
	BYTE GetRealCardValue(BYTE byTempCardList[], BYTE &byTempCardCount, BYTE check_value);
	//寻找匹配牌型
	bool GetMatchCards(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int& iResultCardCount, bool bFellowFriend);
	//判断手中是否只剩炸弹加一手牌
	int CheckOnlyBombAndOneHand(BYTE byCardList[], BYTE byCardCount, BYTE byBoomCardList[]);

	//获取顺子 bCanSplite:是否拆牌找
	int GetSequence(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bCanSplite);
	//提起三张  
	int GetTripleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//提取对子
	int GetDoubleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//提起单牌
	int GetSingleCard(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//是否能一手出完(包括多次出牌,只要别人跟不了就算一手)
	bool CanOutAllCardOnce(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card);
	//寻找手中的大单牌(用做下家地主一张,且上家出了一张单的情况)
	BYTE FindMaxSingleInHand(BYTE byCardList[], BYTE byCardCount, CRecordTool last_card);
	//保存手牌分析数据
	bool SaveAnalysisResult(BYTE byCardList[], BYTE byCardCount, Analysis_Result &result);
	//寻找先手出牌类型
	BYTE FindOutCardType(Analysis_Result result, BYTE PreHandCount, BYTE PostHandCount, COperateRecord out_record, shun_info &shun_type, CRecordTool Uplast_card, CRecordTool Downlast_card);
	//寻找手中在余牌中的最大牌(不是手中最大)
	BYTE FindMaxCardInLast(BYTE byCardList[], BYTE byCardCount, BYTE mix_value, CRecordTool Uplast_card, CRecordTool Downlast_card, UINT out_count = 1);
	//寻找应该出的牌
	bool FindOutCard(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
		CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record, BYTE NTpos);

public:
	//先手出牌
	void FirstOutCard(Card_Info card_info, BYTE byOutCardList[], int &byOutCardCount, COperateRecord out_record);

	//地主先手
	void LandlordFirst(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
	//农民先手(下家地主)
	void FamerFirstBefLD(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
	//农民先手(下家队友)
	void FamerFirstBefFM(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//-------------------------------------
	//跟牌方位判断
	bool FellowCard(Card_Info card_info, BYTE iResultCard[], int& iResultCardCount, COperateRecord out_record);
	//地主跟牌
	void LandlordFellow(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//农民跟(上家地主)
	void FamerFellowPreLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//农民跟(下家地主)
	void FamerFellowPostLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//农民跟(上家队友)
	void FamerFellowPreFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//农民跟(下家队友)
	void FamerFellowPostFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
};

#endif
