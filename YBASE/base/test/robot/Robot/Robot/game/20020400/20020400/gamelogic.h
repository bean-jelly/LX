#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "gamelogic.h"
#include "mjlogic/mjdata.h"
#include "mjlogic/mjlogic.h"
#include "mjlogic/mjfancount.h"
#include "mjlogic/mjmessage.h"
#include "gamemsg.h"
#include "basiccode/utility/BasicCriticalSection.h"
#include "MJGameLogic.h"
#include "gamecount.h"
#ifdef _WINDOWS_
#undef _WINDOWS_
#include <afx.h>
#endif

class gamelogic :public basegamelogic
{
public:
	gamelogic();
	virtual ~gamelogic();
	virtual bool gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id);
	virtual bool gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id);

public:
	//������
	virtual void deal_send_change_desk(unsigned int);
	virtual void kill_timer(unsigned int event_id);
	virtual void set_timer(unsigned int event_id, unsigned int elapse);
	virtual void OnTimer(unsigned int event_id, unsigned int user_id);

private:
	//���ú����齫������ֵ
	void set_mj_logic();
	//��ҳ���
	void deal_user_out_card();
	//�������
	void deal_user_block();
	//��Һ���(�Ƿ�����)
	void deal_send_hu_msg();
	//��Ұ���
	void deal_send_gang_msg();
	//��Ҽ���Ƿ�������״̬��
	bool check_in_ting(int& hu_count);
	//�����������֮���Ƿ�������
	bool check_in_ting_after_penggang(unsigned char card_value,int& hu_count,int& has_cout);
	//�������Ƿ���Ҫ��
	bool check_need_peng(unsigned char card_value);
	//��һ����Ϸ��ʼ��
	void init_for_game();

	//�ҵ���ѳ���
	int find_best_out_card();
	//�ҵ���ѳ���---����
	unsigned char find_best_out_single_card(CMJHandCard&);
	//�ҵ���Ѵ� 
	unsigned char find_best_out_other_card(CMJHandCard&);

	//�õ��������Զ�׼����Ҫ�Ķ���ʱ��
	int get_ready_time(notify_game_result* ptr);

	//��ӡ����
	void printhandcard(unsigned char chair_id);
	std::string findcard(unsigned char value);

private:
	//�����׼����Ϸ
	void deal_count_socre(char* buf, int len);

private:
	game_base_info					m_base_info;							//������Ϣ
	unsigned char					m_makers_station;						//ׯ��λ��
	INT64                           m_servermoney;                          //�����

	CMJBlockCard					m_user_block[PLAY_COUNT];				//�����������
	CMJOutCard						m_user_outcard[PLAY_COUNT];				//��ҳ�������
	CMJHandCard						m_user_handcard;						//�����������
	notify_block_info				m_cur_block_info;						//�����������
	int								m_has_out[CARD_INDEX_NUM];				//�������Ѿ���������
	CMJGameLgic						m_logic;								//�齫�߼�
};

#endif	