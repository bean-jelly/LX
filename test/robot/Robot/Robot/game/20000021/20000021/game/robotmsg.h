#pragma once
#ifndef __ROBOTMSG_H_
#define __ROBOTMSG_H_ 
#include <windows.h>
#include "gamemsg.h"

//��������
#define	UG_HUA_MASK					0xF0			//1111 0000
#define	UG_VALUE_MASK				0x0F			//0000 1111

//�˿˻�ɫ
#define UG_FANG_KUAI				0x00			//����	0000 0000
#define UG_MEI_HUA					0x10			//÷��	0001 0000
#define UG_HONG_TAO					0x20			//����	0010 0000
#define UG_HEI_TAO					0x30			//����	0011 0000
#define UG_NT_CARD					0x40			//����	0100 0000
#define UG_ERROR_HUA				0xF0			//����  1111 0000

//�˿˳�������
#define UG_ERROR_KIND					0				//����
#define UG_ONLY_ONE						1				//����
#define UG_DOUBLE						2				//����
#define UG_VAR_STRAIGHT					3				//����˳��(A2345)˳������С
#define UG_STRAIGHT						4               //˳��,5+��������
#define UG_FLUSH						5				//ͬ��(����)
#define UG_STRAIGHT_FLUSH				6               //ͬ��˳,��ɫ��ͬ��˳��

#define UG_THREE						7				//����
#define UG_THREE_ONE					8               //3 �� 1
#define UG_THREE_TWO					9               //3 �� 2
#define UG_THREE_DOUBLE					10				//3 ��1��

#define UG_VAR_DOUBLE_SEC				11				//����˫˳(AA22)��С
#define UG_DOUBLE_SEC					12				//����,2+�������Ķ���

#define UG_VAR_THREE_SEC				13				//������˳(AAA222��С)
#define UG_THREE_SEQUENCE				14				//�����ţ�2+������������

#define UG_VAR_THREE_ONE_SEC			15				//������˳��һ
#define UG_THREE_ONE_SEQUENCE			16              //2+������������һ

#define UG_VAR_THREE_TWO_SEC			17				//������˳����
#define UG_THREE_TWO_SEQUENCE			18				//2+��������������

#define UG_VAR_THR_DOUBLE_SEC_1			19				//���������Ŵ���
#define UG_THREE_DOUBLE_SEC				20				//�����Ŵ���

#define UG_VAR_THR_DOUBLE_SEC_2		    21				//���ֺ���(��˳����˳)
#define UG_THREE_SEC_DOUBLE_SEC			22				//����(��˳����˳)

#define UG_FOUR_ONE						23				//�Ĵ�һ
#define UG_FOUR_TWO						24				//�Ĵ�����
#define UG_FOUR_ONE_DOUBLE				25				//�Ĵ�һ��
#define UG_FOUR_TWO_DOUBLE				26				//�Ĵ�����

#define UG_VAR_FOUR_SEC					27				//��˳
#define UG_FOUR_SEQUENCE				28				//��˳

#define UG_VAR_FOUR_ONE_SEC				29				//�Ĵ�һ˳
#define UG_FOUR_ONE_SEC					30				//�Ĵ�һ˳

#define UG_VAR_FOUR_TWO_SEC				31				//�Ĵ���˳
#define UG_FOUR_TWO_SEC					32				//�Ĵ���˳

#define UG_VAR_FOUR_ONE_DOUBLE_SEC		33				//�Ĵ���˳
#define UG_FOUR_ONE_DOUBLE_SEC			34				//�Ĵ���˳

#define UG_VAR_FOUR_TWO_DOUBLE_SEC		35				//�Ĵ�����˳
#define UG_FOUR_TWO_DOUBLE_SEC			36				//�Ĵ�����˳

#define UG_BOMB							39              //ը��>=4��
#define UG_BOMB_SAME_HUA				40				//ͬ��ը��(���ĸ������ϵ����г���)
#define UG_KING_BOMB					41				//��ը(���ը��)

#define KING_COUNT						2				//�������ĸ���
#define CARD_MAX_COUNT                  54              //�����Ƶ����� 
#define MAX_COUNT						20									//�����Ŀ

#define MAX_CARD_INDEX					80				//��ֵ�������

#define CARD_BULK_INDEX					18				//��ֵ��С����
#pragma pack(push,1)
typedef struct str_operate_info
{
	BYTE		byCardCount;					//�˿���Ŀ
	BYTE		byCardList[HAND_CARD_MAX];		//�˿���Ϣ 
	BYTE		byCardType;						//�˿�����
	BYTE		byExInfo;						//������Ϣ 1-���� 2-���� 
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
	int single_count;//����(��������,�ɻ�����)
	int double_count;
	int triple_count;
	BYTE boom_info[18];
	BYTE single_info[18];
	BYTE double_info[18];
	BYTE triple_info[18];

	BYTE SequenceListStart[4];
	BYTE SequenceCount[4];

	int iSeqCount;
	int iAloneSingle;	 //û�д��ƹܵĵ�
	int iAloneDouble;	 //û�д��ƹܵĶ�

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
	BYTE seq_count;  //��������
	int  imax_pos;	 //˳�ӵ�����
	BYTE type;		 //�� ˫  ��˳
	BYTE count;		 //˳�����Ƹ���
	BYTE seq_end;	 //˳�ӵ�β�Ŵ�С
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
	BYTE UserCard[HAND_CARD_MAX];               //�Լ�������
	BYTE UserCardCount;							//�Լ������Ƹ���
	BYTE UpUserCard[HAND_CARD_MAX];				//�ϼҵ�����
	BYTE UpUserCardCount;						//�ϼҵ����Ƹ���
	BYTE DownUserCard[HAND_CARD_MAX];			//�¼ҵ�����
	BYTE DownUserCardCount;						//�¼ҵ����Ƹ���
	BYTE DeskCard[HAND_CARD_MAX];				//���������
	BYTE DeskCardCount;							//��������Ƹ���
	BYTE NTposition;							//ׯ��λ��			0:�Լ�  1:�ϼ� 2 :�¼�
	BYTE DeskPos;								//���������Ϊ֮	0:�Լ� 1:�ϼ� 2:�¼�
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