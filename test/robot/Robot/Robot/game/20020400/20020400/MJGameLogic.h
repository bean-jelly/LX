#pragma once
#include "mjlogic/mjlogic.h"

///基类为游戏类
class CMJGameLgic : public CMJlogic
{

public:
	CMJGameLgic();
	~CMJGameLgic();

public:
	//检测是否有赖子,返回赖子的个数
	int GetKingcardList(BYTE handcard[], BYTE cardcount, BYTE byResult[]);
	//检测数组的子集关系
	bool CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount);
	//检测是否能碰
	BYTE CheckCanPeng(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking);
	//检测是否可以吃牌	
	BYTE check_can_chi(const BYTE hand_card[], unsigned int hand_card_count, BYTE check_card, BYTE& chi_type, block_card_info blockinfo[]);
	//检测是否能杠
	bool CheckOtherGang(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking);
	//重写杠牌
	BYTE CheckSelfGang(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, BYTE block_card[], bool bUseKing);
	//检碰后是否可以杠
	BYTE CheckGangAfterblock(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE block_card[], bool bUseKing);
	//是否可以听,byForbidCard,不能打的牌
	BYTE CheckCanTing(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, TingData& ting_info, BYTE byFatforbidcard, BYTE byLimitHuFan);
	//是否可听
	bool IsTingEX(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, BYTE win_card[], BYTE& bytingCount, BYTE byLimitHuFan);
	//获取手牌中花色的种类
	int GetUserHuaKindCount(const BYTE hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block);
	//是否可胡
	bool CheckCanWin(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, bool add,BYTE byLimitHuFan);
	//是否是全顺
	bool checkIsAllSeq(const BYTE hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool bHaveKing);
	//检测是否是全顺
	bool CheckIsAllSeqType(BYTE byCardList[], BYTE byCardCount, CMJBlockCard blocks);
	//是否是碰碰胡
	bool CheckPPH(CMJHandCard handcards, CMJBlockCard blocks);
	//检测是否有红中
	int is_have_hz(unsigned char handcard[], unsigned char cardcount);
	//检测指定牌能否组成顺子、刻子,有序的
	bool check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang);
	//检测是否放炮
	unsigned char check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add, bool bking);
	//检测是否能胡
	bool checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//检测是否能听
	unsigned char check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card);
	//检测是否能听
	bool is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card);
	//设置规则
	void setmode(BYTE beight, bool bqidui);
public:
	BYTE m_beight;
	bool m_bqidui;
};

