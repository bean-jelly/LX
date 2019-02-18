#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "mjdata.h"
#include "basiccode/utility/BasicCriticalSection.h"
#include ".\gamemsg.h"
#ifdef _WINDOWS_
#undef _WINDOWS_
#include <afx.h>
#endif

#define PLAY_COUNT							8			//游戏人数
#define MAX_COUNT							5			//最大数目
#define  CHIP_COUNT							5           //筹码个
#define ERROR_PLAYER_POS                    0XFF        //玩家错误位置   
#define OX_MAX_TYPE_COUNT					20          //牛牛最大牌型个数
#define MAX_CARD_SET_INDEX                  10          //选牌最大索引
#define MAX_CARD_SET_INDEX_KPXZ             10          //看牌选牌最大索引

class gamelogic :public basegamelogic
{
public:
	gamelogic();
	virtual ~gamelogic();
	virtual bool gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id);
	virtual bool gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id);

public:
	void  deal_send_hand_card(char* buf, int len);
	void  deal_draw_hand_card(char* buf, int len);
	void  deal_out_card(char* buf, int len);
	void  deal_notify_out_card(char* buf, int len);
	void  deal_block(char* buf, int len);
	void  deal_block_result(char* buf, int len);
	void  deal_count_socre(char* buf, int len);


	void  deal_send_cards_info(char* buf, int len);
	//抢庄
	void  deal_send_qiang_info(char* buf, int len);
	//下注
	void  deal_send_xiazhu_info(char* buf, int len);
	//检测4张牌中是否有牛 > 0
	int  check_has_Ox(BYTE cardlist[]);

	void  deal_send_change_desk(unsigned int chair_id);
	void set_timer(unsigned int event_id, unsigned int elapse);
	void OnTimer(unsigned int event_id,unsigned int user_id);
	void kill_timer(unsigned int event_id);
private:
	CMJHandCard						m_hand_card;
	unsigned char					m_check_card;
	unsigned char					m_draw_card;
	/********************牌类*************************/
	BYTE							m_UserCard[PLAY_COUNT][MAX_COUNT];		//玩家牌信息
	BYTE							m_UserSecCard[PLAY_COUNT][MAX_COUNT];	//玩家第二手牌 
	BYTE							m_CardType[2];							//玩家牌型
	bool							m_bHaveSecCard;							//是否有第二手牌

	BYTE							m_bybeishu;                         //下注与抢庄参数
	INT64							m_byRobxianzhi[PLAY_COUNT];         //抢庄限制结果
	INT64							m_byXiaZhuRes[PLAY_COUNT];          //下注倍数限制结果

	bool							m_buserready[PLAY_COUNT];			//玩家准备情况
	
	bool							m_usercut[PLAY_COUNT];				//记录已下注，中途强退/断线玩家，游戏结算后需要手动清除数据
	BYTE                            m_usercutcount[PLAY_COUNT];         //记录玩家断线局数
	BYTE                            m_usercutnum;                       //记录断线玩家人数
	bool							m_bHavecallbanker;					//是否发送定庄信息，防止用户断线没有操作
	bool							m_bHavexiazhu;						//是否发送下注信息，防止用户断线没有操作
	BYTE							m_callbanker[PLAY_COUNT];			//一局叫庄记录，没操作255，抢 1， 不抢 0
	BYTE							m_tanpai[PLAY_COUNT][2];			//一局摊牌记录，摊牌 1， 未摊牌 0
	BYTE							m_tanpaitype[PLAY_COUNT];			//一局摊牌类型记录，未摊牌255，其他为摊牌类型值	
	BYTE							m_byUserHandCount[PLAY_COUNT];		//是否设置了第二手牌
	BYTE							m_tanpaiSecType[PLAY_COUNT];        //第二局摊牌类型
	bool                            m_btuizhu[PLAY_COUNT];              //是否能推注
	bool                            m_blimit[PLAY_COUNT];               //是否下注限制最小分
	bool                            m_btuizhulimit[PLAY_COUNT];         //是否下注限制推注分
	bool                            m_bSelectIndex[MAX_CARD_SET_INDEX]; //玩家选牌的索引						
	INT64                           m_ipayserver;                       //服务费
public:
	unsigned int					m_rabot_ready_time;//机器人已准备了的时间
	unsigned int					m_rabot_state;	//机器人状态 0 空闲 ，1 游戏中
	INT							    m_chip[CHIP_COUNT];					//下注筹码值
	stu_userXiaZhu					m_xia_zhu_param;

	stu_userOpenCard				m_tan_pai_param;

	stu_callBanker					m_qiang_banker_param;

};
#endif