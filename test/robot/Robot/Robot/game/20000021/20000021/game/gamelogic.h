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
		//�޲���֪ͨ��Ϣ
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

		//��Ҫ�������Ϣ�ܽӿ�
		void  deal_game_notify_interface(char* buf, int len);
		//������
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
		BYTE							m_UserCard[PLAY_COUNT][HAND_CARD_MAX];		//�������Ϣ
		BYTE							m_UserCardCount[PLAY_COUNT];				//���������
		BYTE							m_Back_card[BACK_CARD_COUNT];				//����

		BYTE							m_DeskCard[HAND_CARD_MAX];					//�����ϵ���
		BYTE							m_DeskCount;								//�����ϵ�������
		BYTE							m_DeskPositon;								//�����ϵ������

		BYTE							m_BigOutUser;								//������
		BYTE							m_SelectCard[3];							//������ѡ�����	
		BYTE							m_byCurrentOut;								//��ǰ�������
		BYTE							m_byNTpos;									//ׯ��λ��
		BYTE							m_curCallScore;								//��ǰ�зַ�ֵ

		CGameLogic						m_GameLogic;								//��Ϸ�߼�
		COperateRecord					m_User_Operate[PLAY_COUNT];					//��Ҳ���
		GSBaseInfo						m_game_base;								//��Ϸ������Ϣ
		
	};
	 

#endif //__GAMELOGIC_H_