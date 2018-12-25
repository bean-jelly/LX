#pragma once
 #ifndef __CMJ_DATA_H__
#define __CMJ_DATA_H__
#include "mjmessage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
/*
�齫���������
*/  

//����������
class CMJHandCard
{
public:
	CMJHandCard();
	virtual~CMJHandCard();
	CMJHandCard(const CMJHandCard& info);
	//�ж������Ƿ���ָ����
	bool IsHaveCard(unsigned char cardvalue,unsigned int count = 1);
	//��ȡָ���Ƹ���
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//��ȡָ���Ƶ�λ��
	int OnGetCardPos(unsigned char cardvalue);
	//�����������
	int OnAddHandCard(unsigned char cardvalue, unsigned int count=1); 
	//���һ������
	bool OnAddHandCards(CMJHandCard cards);
	//ɾ����������
	int OnDelHandCard(unsigned char cardvalue, unsigned int count = 1);
	//ɾ��������
	int OnDelHandCardAll(unsigned char cardvalue[], unsigned int cardcount);
	//������������
	int OnSetHandCard(unsigned char cardvalue, unsigned int pos);
	//��ȡָ��λ��������
	unsigned char OnGetHandCard(unsigned int pos);
	//��ȡ��������
	unsigned int GetAllHandCard(unsigned char cardvalue[]);
	//��ȡ���Ƴ���
	unsigned int GetHandCardLength();
	//����
	void CardSort();
	//�����������
	void ClearAllData(); 
private: 
	unsigned char  m_hand_card[HAND_CARD_COUNT];				//�������
	unsigned int   m_hand_length;								//���������

};
 
class CMJBlockCard
{
public:
	CMJBlockCard();
	virtual~CMJBlockCard();

	//���һ������
	bool AddBlockCard(unsigned char chair_id, unsigned char blocked_card, unsigned char block_type, unsigned char block_sub_type, unsigned char card_value[4]);
	bool AddBlockCard(BlockData blockinfo);
	bool AddBlockCards(CMJBlockCard& block);
	//��ȡָ��λ�õ���������
	bool GetBlock(unsigned int pos, BlockData& blockinfo);
	//����ָ��λ�õ���������
	bool SetBlock(unsigned int pos, BlockData blockinfo);
	//ɾ������
	bool DelBlock(unsigned int pos);
	//�Ƴ����һ��
	bool PopBack(BlockData& blockinfo);
	//�����������
	void ClearAllData();
	//��ȡ���Ƹ���
	unsigned int GetBlockLength();
	//������Ϊ��
	bool SetPengToGang(unsigned char card_value);
	//��ԭ��Ϊ��
	bool BackGangToPeng(unsigned char card_value);
	//����������Ƿ���ĳ����
	bool IsHaveCard(unsigned char card_value, unsigned char block_type);

private:

	BlockData	   m_block[BLOCK_MAX];					//�����
	unsigned int   m_length;									//���Ƹ���

};

//����������
class CMJOutCard
{
public:
	CMJOutCard();
	virtual~CMJOutCard();

	//�ж������Ƿ���ָ����
	bool IsHaveCard(unsigned char cardvalue);
	//��ȡָ���Ƹ���
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//��ȡָ���Ƶ�λ��
	int OnGetCardPos(unsigned char cardvalue);
	//��ӳ�������
	int OnAddOutCard(unsigned char cardvalue, unsigned int count=1);
	bool OnAddHandCards(CMJOutCard cards);
	//ɾ����������
	int OnDelOutCard(unsigned char cardvalue, unsigned int count);
	//���ó�������
	int OnSetOutCard(unsigned char cardvalue, unsigned int pos);
	//��ȡָ��λ��������
	unsigned char OnGetOutCard(unsigned int pos);
	//��ȡ���г���
	unsigned int GetAllOutCard(unsigned char cardvalue[]);
	//��ȡ���Ƴ���
	unsigned int GetOutCardLength();
	//����
	void CardSort();
	//�����������
	void ClearAllData();
	//�Ƴ����һ������
	unsigned char PopEndData();


private:

	unsigned char	  m_out_card[MAX_OUT_CARD];			//��ҳ���
	unsigned int	  m_length;								//���Ƹ���

};

//��ǽ������
class CMJWallCard
{
public:
	CMJWallCard();
	virtual~CMJWallCard();
	//ϴ��
	void RandWallCard(int randcount);
	//�ж������Ƿ���ָ����
	bool IsHaveCard(unsigned char cardvalue);
	//��ȡָ���Ƹ���
	int OnGetCardCount(unsigned char cardvalue);
	//��ȡָ���Ƶ�λ��
	int OnGetCardPos(unsigned char cardvalue);
	//��ȡĳ��λ�õ���
	unsigned char OnGetCardValue(unsigned int pos);
	//�����ǽ����
	bool OnAddWallCard(unsigned char cardvalue, unsigned int cardcount = 1);
	bool OnAddWallCards(unsigned char cardvalue[], unsigned int cardcount);
	bool OnAddWallCards(CMJWallCard wall);
	//ɾ��ָ��λ�õ���
	unsigned char OnDelWallCard(unsigned int pos=0); 
	//�Ƴ����һ������
	unsigned char PopEndData(); 
	//�Ƴ���һ������
	unsigned char PopBeginData();
	//��ȡ��ǽ����
	unsigned int GetWallCardLength();
	//��ȡ������ǽֵ
	unsigned int GetAllWallCard(unsigned char wall_card[]);
	//������ֵ
	bool  Swap(unsigned int first_pos, unsigned int second_pos);
	//��ȡpos1 ��pos2��ֵ
	int GetCards(unsigned int first_pos, unsigned int count, unsigned char card[]);
	//���
	void ClearData(); 

private:
	unsigned char	m_wall_card[MAX_CARD_NUM];
	unsigned int	m_wall_len;
};



//����������
class CMJFlowerCard
{
public:
	CMJFlowerCard();
	virtual~CMJFlowerCard();
	//�жϻ����Ƿ���ָ����
	bool IsHaveCard(unsigned char cardvalue, unsigned int count = 1);
	//��ȡָ���Ƹ���
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//��ȡָ���Ƶ�λ��
	int OnGetCardPos(unsigned char cardvalue);
	//��ӻ�������
	int OnAddFlowerCard(unsigned char cardvalue, unsigned int count = 1);
	//���һ�黨��
	bool OnAddFlowerCards(CMJFlowerCard cards);
	//ɾ����������
	int OnDelFlowerCard(unsigned char cardvalue, unsigned int count);
	//���û�������
	int OnSetFlowerCard(unsigned char cardvalue, unsigned int pos);
	//��ȡָ��λ��������
	unsigned char OnGetFlowerCard(unsigned int pos);
	//��ȡ��������
	unsigned int GetAllFlowerCard(unsigned char cardvalue[]);
	//��ȡ���Ƴ���
	unsigned int GetFlowerCardLength();
	//����
	void CardSort();
	//�����������
	void ClearAllData();
public:
	unsigned char  m_flower_card[MAX_FLOWER_NUM];			//��һ���
	unsigned int   m_length;								//��һ�����

};
  
#endif//__CMJ_DATA_H__