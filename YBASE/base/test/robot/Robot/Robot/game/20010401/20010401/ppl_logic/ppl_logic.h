#pragma once

#ifndef __GAME_LOGIC_H_
#define __GAME_LOGIC_H_ 

//��������
#define	UG_HUA_MASK						0xF0			//1111 0000
#define	UG_VALUE_MASK					0x0F			//0000 1111
#define ALL_CARD_COUNT					54				//�������
#define CARD_COUNT						16				//��ֵ����

#define MAX_COUNT                       3               //�����Ŀ
#define ROW_COUNT						3
#define COL_COUNT						2

//�˿˻�ɫ
#define UG_FANG_KUAI					0x00			//����	0000 0000
#define UG_MEI_HUA						0x10			//÷��	0001 0000
#define UG_HONG_TAO						0x20			//����	0010 0000
#define UG_HEI_TAO						0x30			//����	0011 0000
#define UG_NT_CARD						0x40			//����	0100 0000
#define UG_ERROR_HUA					0xF0			//����  1111 0000

#define TYPE_ERROR						0				//���ʹ���
#define TYPE_SPEIAL						1				//�ӻ�235
#define TYPE_HIGH_CARD					2				//����
#define TYPE_PAIR						3				//����
#define TYPE_STR						4				//˳��
#define TYPE_FLUSH						5				//ͬ��
#define TYPE_STR_FLUSH					6				//ͬ��˳
#define TYPE_THREE						7				//����
#define TYPE_MAX						8				//�������


class CGameLogic
{
public:
	CGameLogic();
	~CGameLogic();
public:
	//�����˿�
	bool RandCard(unsigned char iCard[], int iCardCount);
	//ȡ�����б�
	bool GetCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen);
	//������б�
	int AddCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen);
	//����Ϊ����ӵ�ģʽ
	int SetReplaceCard(bool b_is_have_king);
	//������б�����
	bool CheckCardListType(unsigned char iCard[], int iCardCount, int &TypeValue, int &Type, unsigned char &HighCard);
	//��ȡ�˿�Ȩֵ
	unsigned char GetCardNum(unsigned char iCard);
	//��ȡ�˿˻�ɫ
	unsigned char GetCardColor(unsigned char iCard){ return iCard & UG_HUA_MASK; }
	//��ȡ����
	unsigned char GetHighCard(unsigned char iCard[], int iCardCount);
	//�˿����Ƿ����ָ���˿˵�����С����
	bool IsHaveCardNum(unsigned char iCard[], unsigned char iCardCount, unsigned char iCardValue);
	//�����˿�
	bool SortCard(unsigned char iCard[], unsigned char iCardCount, bool bSortA = false);
	//��ת�б�
	bool Reverse(unsigned char iCard[], unsigned char iCardCount);
	//��ȡ����
	unsigned char GetCardType(unsigned char iCard[], unsigned char iCardCount = MAX_COUNT);
	//�Ƚ������˿˵Ĵ�С 0��� 1����  2С��
	unsigned char CompareCardValue(unsigned char iCard_First, unsigned char iCard_Second, bool bCompareHua);
	//��ȡ������������ֵ
	void GetDoubleMaxNums(unsigned char iCard[], unsigned char iCardCount, unsigned char& max, unsigned char& single);
	//�Ƚϴ�С 0��� 1���� 2С��
	unsigned char CompareCardData(unsigned char iCard_First[], unsigned char iCard_Second[], unsigned char type1, unsigned char type2, bool flag_ex, unsigned char iCardCount = MAX_COUNT);
	//����Ƿ�Ϊ�����
	bool IsReplaceCard(unsigned char byCard);
	//ɾ���˿�
	int RemoveCard(unsigned char iRemoveCard[], int iRemoveCount, unsigned char iCardList[], int iCardCount);
	//ɾ�������˿�
	int RemoveCard(unsigned char iRemoveCard, unsigned char iCardList[], int iCardCount);
	//��� 0 λ�˿�
	int RemoveNummCard(unsigned char iCardList[], int iCardCount);

	//����Ƿ�����
	bool CheckThree(unsigned char iCard[], int iCardCount);
	//����Ƿ�˳��
	bool CheckStraight(unsigned char iCard[], int iCardCount);
	//�Ƿ���˳��123
	bool CheckStraight123(unsigned char iCard[], unsigned char iCardCount);
	//�Ƿ���˳��QKA
	bool CheckStraightQKA(unsigned char iCard[], unsigned char iCardCount);
	//����Ƿ�ͬ��ɫ
	bool CheckSameColor(unsigned char iCard[], unsigned char iCardCount);
	//����Ƿ�ͬ������˳�ӣ�
	bool CheckFlush(unsigned char iCard[], unsigned char iCardCount);
	//����Ƿ�ͬ��˳
	bool CheckStraightFlush(unsigned char iCard[], unsigned char iCardCount);
	//����Ƿ����
	bool CheckPair(unsigned char iCard[], int iCardCount);
	//����Ƿ��ӻ�
	bool CheckTypeSpecial(unsigned char iCard[], int iCardCount);
private:
	bool m_have_king;												//�Ƿ������
};

#endif//__GAMELOGIC_H_