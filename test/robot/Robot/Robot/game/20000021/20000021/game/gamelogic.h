#pragma once

#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "../user/usermanager.h"
#include "basiccode/utility/BasicCriticalSection.h"
#include "gamemsg.h"
#include "robortlogic.h"
 
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
		void  notifymsg_all_card(char* buf, int len);
		void  notifymsg_new_turn(char* buf, int len);
		void  notifymsg_change_card(char* buf, int len);
		void  notifymsg_get_back_card(char* buf, int len);
		void  notifymsg_out_card(char* buf, int len);
		void  notifymsg_game_end(char* buf, int len);
		void  notifymsg_free_game(char* buf, int len);
		void  deal_res_interface(char* buf, int len);
		void  deal_res_call_score(BYTE byChairIDEx, BYTE byValueEx);

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
		void  deal_out_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[]);

	public:
		BYTE   robort_logic_call_score(int RobNTtime = 0);
		
	private: 
		BYTE							m_UserCard[PLAY_COUNT][HAND_CARD_MAX];		//玩家牌信息
		BYTE							m_UserCardCount[PLAY_COUNT];				//玩家牌数量
		BYTE							m_Back_card[BACK_CARD_COUNT];				//底牌

		BYTE							m_DeskCard[HAND_CARD_MAX];					//桌面上的牌
		BYTE							m_DeskCount;								//桌面上的牌数据
		BYTE							m_DeskPositon;								//桌面上的牌玩家

		BYTE							m_BigOutUser;								//最大玩家
		BYTE							m_SelectCard[3];							//换三张选择的牌	
		BYTE							m_byCurrentOut;								//当前出牌玩家
		BYTE							m_byNTpos;									//庄家位置
		BYTE							m_curCallScore;								//当前叫分分值

		CGameLogic						m_GameLogic;								//游戏逻辑
		COperateRecord					m_User_Operate[PLAY_COUNT];					//玩家操作
		GSBaseInfo						m_game_base;								//游戏基本信息
		
	};
	 

#endif //__GAMELOGIC_H_