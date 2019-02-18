#pragma once

#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "../user/usermanager.h"
#include "basiccode/utility/BasicCriticalSection.h"
#include "gamemsg.h"
#include "robot_ai.h"

class gamelogic :public basegamelogic
{
public:
	gamelogic();
	virtual ~gamelogic();
	virtual bool gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id);
	virtual bool gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id);

public:
	void set_timer(unsigned int event_id, unsigned int elapse);
	void OnTimer(unsigned int event_id,unsigned int user_id);
	void kill_timer(unsigned int event_id);
public:
	//无操作通知消息
	void  notifymsg_send_hand_card(char* buf, int len);
	void  notifymsg_get_other_card(char* buf, int len);
	void  notifymsg_new_turn(char* buf, int len);
	void  notifymsg_change_card(char* buf, int len);
	void  notifymsg_get_back_card(char* buf, int len);
	void  notifymsg_out_card_result(char* buf, int len);
	void  notifymsg_game_end(char* buf, int len);
	void  notifymsg_free_game(char* buf, int len);
	void  deal_res_interface(char* buf, int len);
	void  deal_res_call_score(BYTE byChairID, BYTE byValueEx);

	//需要处理的消息总接口
	void  deal_game_notify_interface(char* buf, int len);
	//处理函数
	void  deal_start_game(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_change_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_rob_nt(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_call_score(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_show_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_nt_add_double(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_user_add_double(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);
	void  deal_out_card(BYTE byChairID, BYTE byFlagEx[]=NULL);

public:
	BYTE   robort_logic_call_score(int RobNTtime = 0);
		
private: 
	BYTE							m_UserCard[PLAY_COUNT][HAND_CARD_MAX];		//玩家牌信息
	BYTE							m_UserCardCount[PLAY_COUNT];				//玩家牌数量
private:
	BYTE							m_Banker;									//庄家位置
	BYTE							m_curCallScore;								//当前叫分分值
	BYTE							m_SelectCard[3];							//换三张选择的牌
private:
	BYTE							m_CardDroit;								//当前控手玩家
	BYTE							m_byCurrentOut;								//当前出牌玩家
	BYTE							m_TurnCardCount;							//控手出牌数目
	BYTE							m_TurnCardData[MAX_COUNT];					//控手出牌列表
private:
	GSBaseInfo						m_game_base;								//游戏基本信息
	robotAI							m_RobotAI;									//机器人信息
		
public:
	unsigned int					m_rabot_ready_time;							//机器人已准备了的时间
	unsigned int					m_rabot_state;								//机器人状态 0 空闲 ，1 游戏中
	int								game_count;									//玩了多少局
};
	 

#endif //__GAMELOGIC_H_