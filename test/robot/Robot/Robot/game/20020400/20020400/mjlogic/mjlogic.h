#pragma once
#ifndef  __MJ_LOGIC_H__
#define __MJ_LOGIC_H__

#include "mjmessage.h"
#include "mjdata.h"
class CMJlogic
{
public:
	CMJlogic();
	virtual ~CMJlogic();
	//��������
	bool clearpartdata();
	//�Ƿ�����Ч��ֵ
	bool is_valid_card(unsigned char card_value);

	//����齫������ֵ
	void add_normal_card(unsigned char card_value);

	//���������
	bool check_cardvalue(unsigned char card_value);

	//��ȡ��ɫ
	int get_card_color(unsigned char card_value);

	//��ȡ��ֵ
	int get_card_value(unsigned char card_value);

	//��ֵת��-���ݻ�ɫ
	int change_card_value(unsigned char card_value[], unsigned int card_count, int card_color, int card_index[]);

	//ת��������ֵ
	int change_card_index(const unsigned char card_value[], unsigned int card_count, int card_index[]);
	int change_card_index(CMJHandCard handcard,  int card_index[] );
	//��ȡ�Ƶĸ���
	int get_card_count(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card);

	//�����Ƶ�λ��
	int find_card_index(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card);

	//�����ֵ
	bool add_card(unsigned char hand_card_value[], unsigned int& card_count, unsigned char add_card[], unsigned int add_count);

	//ɾ����ֵ
	bool del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card[], unsigned int del_count);
	bool del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card, unsigned int del_count);

	//����
	void sort_card(unsigned char hand_card_value[], unsigned int card_count);


	//����Ƿ���Գ���
	unsigned char check_can_chi(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card, unsigned char& chi_type, block_card_info blockinfo[]);
	bool	check_chi(CMJHandCard handcard, unsigned char check_user, unsigned char out_user, unsigned char check_card, CMJBlockCard& block_info);
	//����Ƿ��������
	unsigned char check_can_peng(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card);

	//����Ƿ���Ը���--����
	unsigned char check_can_gang(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card);
	//����Ƿ���԰��ܣ�����
	unsigned char check_can_gang_self(const unsigned char hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block, unsigned char check_card, unsigned char block_card[]);
	//unsigned char check_can_bu_gang(block_data block_value, unsigned char check_card);

	//����Ƿ��������
	unsigned char check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card);

	//����Ƿ����
	unsigned char check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add);
	  
	//��������Ƿ�����
	bool is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card);
	  
	//����Ƿ���Ժ���
	//bool checkwin(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//�жϺ�
	bool checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//���ָ�����ܷ����˳�ӡ�����,�����
	bool check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang);
	bool is_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool jiang);
	//����Ƿ�ȫ�Ƕ���
	bool check_all_double(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//�ж��������Ƿ������ϳ�˳��
	bool check_comb_shun(unsigned char card_value[],unsigned int card_count); 

	//�ж��Ƿ������߶�
	bool check_qidui(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//�ж��Ƿ��131
	bool check_131(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//�ж��Ƿ��13�� 
	bool check_qbk(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//��ȡ��������
	int get_index_count(const  int card_index[],unsigned int card_count);
	//��������
	bool analyze_card(const  int card_index[], const unsigned int jiang_index, int  remain_index[]);

	//������Ϸģʽ
	void set_mode(game_mode param);
	//����������
	void set_king_value(unsigned  char  kingcard[], unsigned int kingcount);
	//���� �����Ƴ���һ�����������ֱ�Ӻ���
	void set_king_over_win( unsigned int kingcount );

	//������������Ƿ�Ϸ�
	bool check_all_data(CMJHandCard user_hand[], CMJBlockCard user_block[],CMJOutCard user_out[],CMJWallCard wall,unsigned int paly_count);

	//���ٱ����Ƿ�任
	bool check_king_usedd(CMJHandCard user_hand);
	//�ж��Ƿ�������
	bool is_king(unsigned char cardvalue);
	//����Ƿ�������
	int is_have_king_value(unsigned char handcard[], unsigned char cardcount);
	//ɾ�����е�����
	int del_king_value(unsigned char handcard[], unsigned char cardcount);
	//�ж��Ƿ�������
	bool  is_zi_card(unsigned char card);
	//�ж��Ƿ��Ƿ��� �� �� �� ��
	bool is_wind_card(unsigned char card);
	//�ж��Ƿ��Ǽ��ƣ��� �� ��
	bool is_arrow_card(unsigned char card);
	//�ж��Ƿ���19
	bool  is_one_nine(unsigned char card);
	//�ж��Ƿ���2��5��8
	bool is_258(unsigned char card); 
	//��������������˳��
	int check_index_combox(int index_card[]);
public:
	unsigned int				m_normal_len;						//�Ƴ���
	unsigned char				m_normal_card[NORMAL_CARD_NUM];		//���͸���
	bool						m_allow_qbk;						//�Ƿ�����13��/ȫ����
	bool						m_allow_131;						//�Ƿ�����131 
	bool						m_allow_qidui;						//�Ƿ������߶�
	bool						m_allow_mix_jiang;					//�Ƿ������ҽ�
	bool						m_allow_laizi;						//�Ƿ���������
	bool						m_allow_chi;						//�Ƿ������
	bool						m_allow_wind_chi;					//�Ƿ�����Է���
	bool						m_allow_arrow_chi;					//�Ƿ�����Լ���
	bool						m_allow_peng;						//�Ƿ�������
	bool						m_allow_gang;						//�Ƿ������
	unsigned int				m_king_count_limit;					//�������ƣ�ֱ�Ӻ� 
	unsigned int				m_king_count;						//���ӵĸ���
	unsigned char				m_king_card[KING_CARD_NUM];			//����
	
};
#endif//__MJ_LOGIC_H__
