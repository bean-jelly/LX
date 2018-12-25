#pragma once
 #ifndef __CMJ_DATA_H__
#define __CMJ_DATA_H__
#include "mjmessage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
/*
麻将相关数据类
*/  

//手牌数据类
class CMJHandCard
{
public:
	CMJHandCard();
	virtual~CMJHandCard();
	CMJHandCard(const CMJHandCard& info);
	//判断手里是否有指定牌
	bool IsHaveCard(unsigned char cardvalue,unsigned int count = 1);
	//获取指定牌个数
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//获取指定牌的位置
	int OnGetCardPos(unsigned char cardvalue);
	//添加手牌数据
	int OnAddHandCard(unsigned char cardvalue, unsigned int count=1); 
	//添加一组手牌
	bool OnAddHandCards(CMJHandCard cards);
	//删除手牌数据
	int OnDelHandCard(unsigned char cardvalue, unsigned int count = 1);
	//删除所有牌
	int OnDelHandCardAll(unsigned char cardvalue[], unsigned int cardcount);
	//设置手牌数据
	int OnSetHandCard(unsigned char cardvalue, unsigned int pos);
	//获取指定位置牌数据
	unsigned char OnGetHandCard(unsigned int pos);
	//获取所有手牌
	unsigned int GetAllHandCard(unsigned char cardvalue[]);
	//获取手牌长度
	unsigned int GetHandCardLength();
	//排序
	void CardSort();
	//清除所有数据
	void ClearAllData(); 
private: 
	unsigned char  m_hand_card[HAND_CARD_COUNT];				//玩家手牌
	unsigned int   m_hand_length;								//玩家手牌数

};
 
class CMJBlockCard
{
public:
	CMJBlockCard();
	virtual~CMJBlockCard();

	//添加一组拦牌
	bool AddBlockCard(unsigned char chair_id, unsigned char blocked_card, unsigned char block_type, unsigned char block_sub_type, unsigned char card_value[4]);
	bool AddBlockCard(BlockData blockinfo);
	bool AddBlockCards(CMJBlockCard& block);
	//获取指定位置的拦牌数据
	bool GetBlock(unsigned int pos, BlockData& blockinfo);
	//设置指定位置的拦牌数据
	bool SetBlock(unsigned int pos, BlockData blockinfo);
	//删除拦牌
	bool DelBlock(unsigned int pos);
	//推出最后一个
	bool PopBack(BlockData& blockinfo);
	//清除所有数据
	void ClearAllData();
	//获取拦牌个数
	unsigned int GetBlockLength();
	//将碰改为杠
	bool SetPengToGang(unsigned char card_value);
	//还原杠为碰
	bool BackGangToPeng(unsigned char card_value);
	//检测拦牌里是否有某张牌
	bool IsHaveCard(unsigned char card_value, unsigned char block_type);

private:

	BlockData	   m_block[BLOCK_MAX];					//玩家拦
	unsigned int   m_length;									//拦牌个数

};

//出牌数据类
class CMJOutCard
{
public:
	CMJOutCard();
	virtual~CMJOutCard();

	//判断手里是否有指定牌
	bool IsHaveCard(unsigned char cardvalue);
	//获取指定牌个数
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//获取指定牌的位置
	int OnGetCardPos(unsigned char cardvalue);
	//添加出牌数据
	int OnAddOutCard(unsigned char cardvalue, unsigned int count=1);
	bool OnAddHandCards(CMJOutCard cards);
	//删除出牌数据
	int OnDelOutCard(unsigned char cardvalue, unsigned int count);
	//设置出牌数据
	int OnSetOutCard(unsigned char cardvalue, unsigned int pos);
	//获取指定位置牌数据
	unsigned char OnGetOutCard(unsigned int pos);
	//获取所有出牌
	unsigned int GetAllOutCard(unsigned char cardvalue[]);
	//获取出牌长度
	unsigned int GetOutCardLength();
	//排序
	void CardSort();
	//清除所有数据
	void ClearAllData();
	//推出最后一个数据
	unsigned char PopEndData();


private:

	unsigned char	  m_out_card[MAX_OUT_CARD];			//玩家出牌
	unsigned int	  m_length;								//出牌个数

};

//牌墙数据类
class CMJWallCard
{
public:
	CMJWallCard();
	virtual~CMJWallCard();
	//洗牌
	void RandWallCard(int randcount);
	//判断手里是否有指定牌
	bool IsHaveCard(unsigned char cardvalue);
	//获取指定牌个数
	int OnGetCardCount(unsigned char cardvalue);
	//获取指定牌的位置
	int OnGetCardPos(unsigned char cardvalue);
	//获取某个位置的牌
	unsigned char OnGetCardValue(unsigned int pos);
	//添加牌墙数据
	bool OnAddWallCard(unsigned char cardvalue, unsigned int cardcount = 1);
	bool OnAddWallCards(unsigned char cardvalue[], unsigned int cardcount);
	bool OnAddWallCards(CMJWallCard wall);
	//删除指定位置的牌
	unsigned char OnDelWallCard(unsigned int pos=0); 
	//推出最后一个数据
	unsigned char PopEndData(); 
	//推出第一个数据
	unsigned char PopBeginData();
	//获取牌墙长度
	unsigned int GetWallCardLength();
	//获取所有牌墙值
	unsigned int GetAllWallCard(unsigned char wall_card[]);
	//交换牌值
	bool  Swap(unsigned int first_pos, unsigned int second_pos);
	//获取pos1 到pos2的值
	int GetCards(unsigned int first_pos, unsigned int count, unsigned char card[]);
	//清空
	void ClearData(); 

private:
	unsigned char	m_wall_card[MAX_CARD_NUM];
	unsigned int	m_wall_len;
};



//花牌数据类
class CMJFlowerCard
{
public:
	CMJFlowerCard();
	virtual~CMJFlowerCard();
	//判断花里是否有指定牌
	bool IsHaveCard(unsigned char cardvalue, unsigned int count = 1);
	//获取指定牌个数
	unsigned int OnGetCardCount(unsigned char cardvalue);
	//获取指定牌的位置
	int OnGetCardPos(unsigned char cardvalue);
	//添加花牌数据
	int OnAddFlowerCard(unsigned char cardvalue, unsigned int count = 1);
	//添加一组花牌
	bool OnAddFlowerCards(CMJFlowerCard cards);
	//删除花牌数据
	int OnDelFlowerCard(unsigned char cardvalue, unsigned int count);
	//设置花牌数据
	int OnSetFlowerCard(unsigned char cardvalue, unsigned int pos);
	//获取指定位置牌数据
	unsigned char OnGetFlowerCard(unsigned int pos);
	//获取所有手牌
	unsigned int GetAllFlowerCard(unsigned char cardvalue[]);
	//获取手牌长度
	unsigned int GetFlowerCardLength();
	//排序
	void CardSort();
	//清除所有数据
	void ClearAllData();
public:
	unsigned char  m_flower_card[MAX_FLOWER_NUM];			//玩家花牌
	unsigned int   m_length;								//玩家花牌数

};
  
#endif//__CMJ_DATA_H__