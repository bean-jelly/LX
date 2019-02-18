#pragma once
#include "mjlogic/mjlogic.h"

///����Ϊ��Ϸ��
class CMJGameLgic : public CMJlogic
{

public:
	CMJGameLgic();
	~CMJGameLgic();

public:
	//����Ƿ�������,�������ӵĸ���
	int GetKingcardList(BYTE handcard[], BYTE cardcount, BYTE byResult[]);
	//���������Ӽ���ϵ
	bool CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount);
	//����Ƿ�����
	BYTE CheckCanPeng(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking);
	//����Ƿ���Գ���	
	BYTE check_can_chi(const BYTE hand_card[], unsigned int hand_card_count, BYTE check_card, BYTE& chi_type, block_card_info blockinfo[]);
	//����Ƿ��ܸ�
	bool CheckOtherGang(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking);
	//��д����
	BYTE CheckSelfGang(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, BYTE block_card[], bool bUseKing);
	//�������Ƿ���Ը�
	BYTE CheckGangAfterblock(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE block_card[], bool bUseKing);
	//�Ƿ������,byForbidCard,���ܴ����
	BYTE CheckCanTing(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, TingData& ting_info, BYTE byFatforbidcard, BYTE byLimitHuFan);
	//�Ƿ����
	bool IsTingEX(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, BYTE win_card[], BYTE& bytingCount, BYTE byLimitHuFan);
	//��ȡ�����л�ɫ������
	int GetUserHuaKindCount(const BYTE hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block);
	//�Ƿ�ɺ�
	bool CheckCanWin(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, bool add,BYTE byLimitHuFan);
	//�Ƿ���ȫ˳
	bool checkIsAllSeq(const BYTE hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool bHaveKing);
	//����Ƿ���ȫ˳
	bool CheckIsAllSeqType(BYTE byCardList[], BYTE byCardCount, CMJBlockCard blocks);
	//�Ƿ���������
	bool CheckPPH(CMJHandCard handcards, CMJBlockCard blocks);
	//����Ƿ��к���
	int is_have_hz(unsigned char handcard[], unsigned char cardcount);
	//���ָ�����ܷ����˳�ӡ�����,�����
	bool check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang);
	//����Ƿ����
	unsigned char check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add, bool bking);
	//����Ƿ��ܺ�
	bool checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//����Ƿ�����
	unsigned char check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card);
	//����Ƿ�����
	bool is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card);
	//���ù���
	void setmode(BYTE beight, bool bqidui);
public:
	BYTE m_beight;
	bool m_bqidui;
};

