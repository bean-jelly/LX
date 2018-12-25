#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "gamelogic.h"
#include "ppl_logic/ppl_logic.h"
#include "gamemsg.h"
#include "basiccode/utility/BasicCriticalSection.h"

#ifdef _WINDOWS_
#undef _WINDOWS_
#include <afx.h>
#endif

struct pick_record
{
	int row;
	unsigned char pick_card;
	int list_type;
	int card_num;
	pick_record()
	{
		memset(this, 0, sizeof(pick_record));
	}
};

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
	//��ȡ�����ļ� ini
	void read_config_ini();
	//��������
	void init_data();
	//��ҳ���
	void deal_user_out_card();
	//�ҵ���ѳ���
	void find_best_out_card(unsigned char &out_card, int &row, int &col);
	//������ͬȨֵ�Ƶ�����
	int  find_same_value(unsigned char card);
	//������ͬ��ɫ�Ƶ�����
	int  find_same_color(unsigned char card);
	//���ĳ�еİ����Ƿ���ɣ���û������������Ŀո�
	bool find_blank(int row, int &out_row, int &out_col);
	//���ĳ�еĿո���
	int	 find_blank_num(int row);
	//��ӡ����
	void printhandcard();
	//���ʣ���Ƶĸ���
	int CountCardNum(unsigned char card);
	//������ֵλ��
	int	check_pos(unsigned char card);

	//����Ƿ�����ϳ��ӻ�235
	bool check_combine_235(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳ�����
	bool check_combine_three(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳ�ͬ��˳
	bool check_combine_straightflush(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳ�ͬ��
	bool check_combine_flush(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳ�˳��
	bool check_combine_straight(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳɶ���
	bool check_combine_pair(unsigned char &out_card, int &row, int &col);
	//����Ƿ�����ϳɸ���
	bool check_combine_high(unsigned char &out_card, int &row, int &col);

	std::string translate(unsigned char card);
private:
	//�����׼����Ϸ
	void deal_count_socre(char* buf, int len);

private:
	game_base_info					m_base_info;								//������Ϣ

	int								m_has_out[ALL_CARD_COUNT];					//�������Ѿ���������
	unsigned char					m_hand_card[HAND_CARD_NUM];					//�������
	int								m_hand_card_num;							//���Ƹ���
	user_placed_card				m_placed_card[PLAY_COUNT];					//����
	pick_record						m_placed_status[HAND_CARD_NUM][ROW_COUNT];	//���Ƽ�¼
	bool							m_is_done[ROW_COUNT];						//�Ƿ����ĳ�еİ���
	unsigned char					m_public_card[ROW_COUNT];					//������
	int								m_public_num;								//�����Ƹ���
	bool							m_have_king;								//�Ƿ��������
	unsigned char					m_king_card[2];								//�����
	int								m_current_turn;								//��ǰ����
	CGameLogic						m_ppl_logic;								//ƴƴ���߼�
	bool							m_is_sp235[ROW_COUNT];						//�Ƿ�������ӻ�235
	bool							m_is_three[ROW_COUNT];						//�Ƿ������3��

	int								m_time_out_card1;							//�������ʱ��
	int								m_time_out_card2;							//�����ʱ��
};

#endif	