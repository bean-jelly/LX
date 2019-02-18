
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


//�˿˷����ṹ	��ֻ���ڷ������Ա��˿ˣ�


class CRecordTool
{
public:
	CRecordTool(void);
	virtual	~CRecordTool();

public:
	//��ʼ��������
	bool InitLastCard();
	//���ü�������ֵ
	bool SetLastCard(CRecordTool src);
	//���ü�������ֵ
	bool SetLastCard(BYTE card_value[], unsigned int card_count);
	//ɾ����
	UNIT RemoveCard(BYTE out_card_list[], unsigned int out_card_count);
	////�Ƿ���ĳ����
	bool IsHaveCard(BYTE byCard, unsigned int count = 1);
	////�Ƿ���ը��
	int IsHaveBoom(BYTE start_card = 0x00);
	//����
	bool Sort();
	//��ȡ������ֵ
	UNIT GetAllCard(BYTE byCardList[]);
	//��ȡ����������ֵindex
	UNIT GetAllCardIndex(BYTE byIndex[]);
	//��ȡʣ���������
	BYTE GetLastMaxCard(UINT count = 1);
	//�Ƿ��д�Ĺ���˳��
	bool IsHaveBigShun(BYTE start_val, BYTE count);
	//��������
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
	//���һ�������Ϣ
	bool AddOutCardInfo(BYTE byCardList[], BYTE	byCardCount, BYTE byCardType, BYTE byFirstOut);
	//���һ�������Ϣ
	bool AddPassCardInfo(BYTE byCardList[], BYTE byCardCount, BYTE byCardType, BYTE byFriend);
	//��ȡ�������ϴγ�����ֵ
	bool GetLastOutCardbyType(BYTE byCardType, BYTE byCardList[], BYTE byCardCount);
	//��ȡ�������ϴγ�������
	BYTE GetLastOutType();
	//��ȡ�������ϴβ�������ֵ
	bool GeteLastPassCardbyType(BYTE byCardType, BYTE byCardList[], BYTE &byCardCount);
	//��ȡ�����ͳ��ƵĴ���
	unsigned int GetCountOutbyType(BYTE byCardType);
	//��ȡ�����Ͳ����Ĵ���
	unsigned int GetCountPassbyType(BYTE byCardType);
	//��ȡ����ͬ���Ͳ�������
	unsigned int GetContinuePassCount(BYTE iBaseShape);
	//�����������
	bool ClearAllData();

private:
	operate_info				m_out_info[HAND_CARD_MAX];
	unsigned int				m_out_count;
	operate_info				m_pass_info[2 * HAND_CARD_MAX];
	unsigned int				m_pass_count;
	unsigned int				m_continue_pass_count;//����ͬ�����Ʋ�������
};

//�˿����߼�
class CGameLogic
{
	//��������
private:
	BYTE			m_bSortCardStyle;       //����ʽ
	BOOL			m_bKingCanReplace;		//���Ƿ�ɵ�
	game_config		gameconfig;				//��Ϸ����
	bool			m_bFeedCard;			//�Ƿ�ι��
private:
	//��������temp����
	int m_total_outhand_count;
	int m_real_single_count;

	bool m_have_little_king;
	bool m_have_large_king;

	int m_boom_count;
	int m_single_count;//����(��������,�ɻ�����)
	int m_double_count;
	int m_triple_count;
	BYTE m_boom_info[18];
	BYTE m_single_info[18];
	BYTE m_double_info[18];
	BYTE m_triple_info[18];

	BYTE m_SequenceListStart[4];
	BYTE m_SequenceCount[4];//0x15��ʾ��˳������0x24��ʾ˫˳4����0x32��ʾ�ɻ�2��
	//��������
public:
	//���캯��		
	CGameLogic(void);
	//��������
	virtual ~CGameLogic();

	//���ܺ���������������
public:
	//[�������]
	void InitAllData();
	void SetGameRule(game_config config);
	//��������������
	void ClearTempAnalyseInfo();
	//��ȡ�˿�����
	int GetCardNum(BYTE iCard);
	//��ȡ�˿˻�ɫ(Ĭ��Ϊ��ʵ��ɫ)
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua = TRUE);
	//��ȡ�˿���Դ�С(Ĭ��Ϊ�ƴ�С,�������С)
	int  GetCardBulk(BYTE iCard, bool bExtVal = false);
	//��ȡ�˿���ͨ����Դ�С
	BYTE GetCardByValue(int iCardValue);
	//�O�������Ԯ���
	void SetKingCanReplace(BOOL bKingCanReplace = false){ m_bKingCanReplace = bKingCanReplace; }
	//�Cȡ���Ƿ���Ԯ���
	BOOL GetKingCanReplace(){ return m_bKingCanReplace; }
	//��������ʽ
	void SetSortCardStyle(BYTE SortCardStyle){ m_bSortCardStyle = SortCardStyle; }
	//��ȡ����ʽ
	BYTE GetSortCardStyle(){ return m_bSortCardStyle; }
	//��ֵ�Ϸ��Լ���
	bool IsValidataCard(BYTE byCard);
	//[����]
public:
	//�����˿�,����С(����ϵͳ����)
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount, BOOL bSysSort = FALSE);
	//��ת��˳(�ӵ�->��)
	BOOL ReverseCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount);
	//����������
	BOOL SortCardByStyle(BYTE iCardList[], BYTE iCardCount);
public:
	//�����˿�
	BYTE RandCard(BYTE iCard[], int iCardCount);		//bHaveKing��ʾ�Ƿ��д�Сè,false��,ture��

	//ɾ���˿�
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
	//��� 0 λ�˿�
	int RemoveNummCard(BYTE iCardList[], int iCardCount);
public://[��������]
	//�Աȵ���
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);
	//�Ƿ�Ϊͬһ������
	BOOL IsSameNumCard(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//�Ƿ�Ϊͬһ��ɫ
	BOOL IsSameHuaKind(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//���� >=4 ը��������ը������
	BYTE GetBombCount(BYTE iCardList[], int iCardCount, int iNumCount = 4, bool bExtVal = false);
	//��ȡָ����С�Ƹ���
	BYTE GetCountBySpecifyNumCount(BYTE iCardList[], int iCardCount, int Num);
	//��ȡָ���Ƹ���
	BYTE GetCountBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCard);
	//��ȡָ������ֵ�Ƶĸ���
	BYTE GetCountBySpecifyBulk(BYTE byCardList[], int iCardCount, int byCardBulk);
	//��ȡָ���������ƴ�С(������ֻ�ܹ���һ���Ƶ�����ΪiCount,��Ȼ����ȥ�Ľ��ǵ�һ��ָ��������ֵ)
	BYTE GetBulkBySpecifyCardCount(BYTE iCardList[], int iCardCount, int iCount);
	//�Ƿ�Ϊĳָ����˳��(����˳��)
	BOOL IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount);
	//�Ƿ�Ϊĳָ����˳��
	BOOL IsSequence(BYTE iCardList[], int iCardCount, int iCount);

	int GetSequenceXLen(BYTE iCardList[], int iCardCount, int iCount, int iStartPos);
	//��ȡָ������
	BYTE TackOutBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCardBuffer[], int &iResultCardCount, BYTE bCard);
	//��ȡĳ��ָ�����ֵ���
	BOOL TackOutCardBySpecifyCardNum(BYTE iCardList[], int iCardCount, BYTE iBuffer[], int &iBufferCardCount, BYTE iCard, BOOL bExtVal = false);
	//��ȡ���з�����������,����,����,����,4ը������
	int  TackOutBySepcifyCardNumCount(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BYTE bCardNum, bool bExtVal = false);
	//��ȡָ����ɫ��
	int  TackOutByCardKind(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BYTE iCardKind);
	//���(�������ƶ�Ĳ���ٵ�)
	int  TackOutMuchToFew(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], int &iBufferCardCount, BYTE iCardMuch, BYTE iCardFew);
	//���Ҵ���iCard�ĵ�������iCardList�е����
	BYTE GetSerialByMoreThanSpecifyCard(BYTE iCardList[], int iCardCount, BYTE iCard, BYTE iBaseCardCount, bool bExtValue = false);
	//����==iCard�ĵ�������iCardList�е����(��ʼλ��,���K�cλ��)
	int  GetSerialBySpecifyCard(BYTE iCardList[], int iStart, int iCardCount, BYTE iCard);
	//��ȡָ��˳�����Ƶ���Сֵ(iSequence ����˳�ӵ��������Ϊ
	BYTE GetBulkBySpecifySequence(BYTE iCardList[], int iCardCount, int iSequence = 3);
	//��ȡָ��˳�����Ƶ����ֵ����˳��
	BYTE GetBulkBySpecifyVariationSequence(BYTE iCardList[], int iCardCount, int iSequence = 3);
	//������С (1) or ��� (255) ��
	int	 GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax, bool bExtVal = false);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//[�������]
public:
	//��ȡ����
	BYTE GetCardShape(BYTE iCardList[], int iCardCount);
	//�Ƿ���
	inline BOOL IsOnlyOne(BYTE iCardList[], int iCardCount) { return iCardCount == 1; };
	//�Ƿ����
	BOOL IsDouble(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//3 �� 1or2(��һ�Դ������Ż��һ����
	BOOL IsThreeX(BYTE iCardList[], int iCardCount, int iX/*1or2*/, bool bExtVal = false);
	//��ը
	BOOL IsKingBomb(BYTE iCardList[], int iCardCount);
	//4+���� ը��
	BOOL IsBomb(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//ͬ��ը��
	BOOL IsBombSameHua(BYTE iCardList[], int iCardCount);
	//ͬ��(��˳��)
	BOOL IsFlush(BYTE iCardList[], int iCardCount);
	//�Ƿ���ͬ��˳
	BOOL IsStraightFlush(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//���ֵ�˦
	BOOL IsVariationStraight(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//��˦
	BOOL IsStraight(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//���ֶ�˦
	BOOL IsVariationDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//��˦ //����?
	BOOL IsDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//�Ƿ����������������X(0,1,2,3)
	BOOL IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/, bool bExtVal = false);
	//�Ƿ�������������X(0,1,2,3)
	BOOL IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/, bool bExtVal = false);
	//�Ƿ���˳����˳(����)
	BOOL IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//�Ƿ���˳����˳(����)
	BOOL IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//�Ĵ�һ�����Ĵ���
	BOOL IsFourX(BYTE iCardList[], int iCardCount, int iX/*1or 2*/);//����1,������2,һ����,2��4
	//�Ƿ�����Ĵ�X˳
	BOOL IsVariationFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX);
	//�Ĵ�һ�����Ĵ�����˳��
	BOOL IsFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX);
	//[�������]
public:
	//�Ƿ���Գ���
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, bool bFirstOut = false);
	//���ұȵ�ǰ���ƴ��
	BOOL TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, bool bSingleOrder);
	//[��ȡ��]
public:
	//��ȡ����������1 or 2or 3(��,һ��,�������)
	BYTE TackOutThreeX(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int  xValue);
	//��ȡ2����������������1,2
	BOOL TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue);
	//��ȡ2����������������1,2
	BOOL TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount);
	//��ȡ˳������Сλ��ֵ(xSequence��ʾĬ�ϵ�˳)
	int GetSequenceStartPostion(BYTE iCardList[], int iCardCount, int xSequence = 1);
	//��ȡ���ŵ�˳��,����˳��,����˳��
	BOOL TackOutSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xSequence, BOOL bNoComp = false);
	//��ȡͬ��˳
	BOOL TackOutStraightFlush(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount);
	//��ȡ����ը��
	BOOL TackOutAllBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, int iNumCount = 4);
	//��ȡը��(����Ĭ��Ϊ4)
	BOOL TackOutBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, int iNumCount = 4);
	//��ȡ��ը
	BOOL TackOutKingBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount);
	//���������
	BOOL TackOutCardByNoSameShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount);
	//�ô�����ƴ������ϵ���
	BOOL TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount);
	//��������������С���Ǹ����ŵ��Ƶ�
	int SearchMinThreeSeq(BYTE iCardList[], int iCardCount, bool bExtVal = false);
	//���ָ�������Ƿ����Ӽ���ϵ
	bool CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount);
	//�Ƿ��ǱȽ�����
	bool IsMustCallShape(BYTE cardlist[], BYTE count);
	//��ȡѡ������
	void GetchangeCardList(BYTE byCardList[], BYTE byCardCount, BYTE byReslut[]);
	//����Ƿ���һ����
	bool IsASetCardlist(BYTE byCardList[], BYTE byCardCount);
public:
	//���ͽзֲο���Ϣ
	BYTE InfoForCallScore(BYTE byCardList[], BYTE byCardCount);
	//��������
	Analysis_Result AnalyzeHandCard(BYTE byCardList[], BYTE byCardCount, bool bSplitBomb = true, bool bSaveResult = true);

	//����˳��
	void FindSequence(BYTE byTempCardList[], BYTE bySequenceList[], BYTE bySequenceCount[], bool bSplitBomb);
	//��ȡ�����еĶ�Ӧ��ֵ
	BYTE GetRealCardValue(BYTE byTempCardList[], BYTE &byTempCardCount, BYTE check_value);
	//Ѱ��ƥ������
	bool GetMatchCards(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int& iResultCardCount, bool bFellowFriend);
	//�ж������Ƿ�ֻʣը����һ����
	int CheckOnlyBombAndOneHand(BYTE byCardList[], BYTE byCardCount, BYTE byBoomCardList[]);

	//��ȡ˳�� bCanSplite:�Ƿ������
	int GetSequence(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bCanSplite);
	//��������  
	int GetTripleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//��ȡ����
	int GetDoubleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//������
	int GetSingleCard(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite);
	//�Ƿ���һ�ֳ���(������γ���,ֻҪ���˸����˾���һ��)
	bool CanOutAllCardOnce(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card);
	//Ѱ�����еĴ���(�����¼ҵ���һ��,���ϼҳ���һ�ŵ������)
	BYTE FindMaxSingleInHand(BYTE byCardList[], BYTE byCardCount, CRecordTool last_card);
	//�������Ʒ�������
	bool SaveAnalysisResult(BYTE byCardList[], BYTE byCardCount, Analysis_Result &result);
	//Ѱ�����ֳ�������
	BYTE FindOutCardType(Analysis_Result result, BYTE PreHandCount, BYTE PostHandCount, COperateRecord out_record, shun_info &shun_type, CRecordTool Uplast_card, CRecordTool Downlast_card);
	//Ѱ�������������е������(�����������)
	BYTE FindMaxCardInLast(BYTE byCardList[], BYTE byCardCount, BYTE mix_value, CRecordTool Uplast_card, CRecordTool Downlast_card, UINT out_count = 1);
	//Ѱ��Ӧ�ó�����
	bool FindOutCard(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
		CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record, BYTE NTpos);

public:
	//���ֳ���
	void FirstOutCard(Card_Info card_info, BYTE byOutCardList[], int &byOutCardCount, COperateRecord out_record);

	//��������
	void LandlordFirst(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
	//ũ������(�¼ҵ���)
	void FamerFirstBefLD(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
	//ũ������(�¼Ҷ���)
	void FamerFirstBefFM(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//-------------------------------------
	//���Ʒ�λ�ж�
	bool FellowCard(Card_Info card_info, BYTE iResultCard[], int& iResultCardCount, COperateRecord out_record);
	//��������
	void LandlordFellow(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//ũ���(�ϼҵ���)
	void FamerFellowPreLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//ũ���(�¼ҵ���)
	void FamerFellowPostLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//ũ���(�ϼҶ���)
	void FamerFellowPreFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);

	//ũ���(�¼Ҷ���)
	void FamerFellowPostFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
		BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record);
};

#endif
