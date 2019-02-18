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

#define PLAY_COUNT							8			//��Ϸ����
#define MAX_COUNT							5			//�����Ŀ
#define  CHIP_COUNT							5           //�����
#define ERROR_PLAYER_POS                    0XFF        //��Ҵ���λ��   
#define OX_MAX_TYPE_COUNT					20          //ţţ������͸���
#define MAX_CARD_SET_INDEX                  10          //ѡ���������
#define MAX_CARD_SET_INDEX_KPXZ             10          //����ѡ���������

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
	//��ׯ
	void  deal_send_qiang_info(char* buf, int len);
	//��ע
	void  deal_send_xiazhu_info(char* buf, int len);
	//���4�������Ƿ���ţ > 0
	int  check_has_Ox(BYTE cardlist[]);

	void  deal_send_change_desk(unsigned int chair_id);
	void set_timer(unsigned int event_id, unsigned int elapse);
	void OnTimer(unsigned int event_id,unsigned int user_id);
	void kill_timer(unsigned int event_id);
private:
	CMJHandCard						m_hand_card;
	unsigned char					m_check_card;
	unsigned char					m_draw_card;
	/********************����*************************/
	BYTE							m_UserCard[PLAY_COUNT][MAX_COUNT];		//�������Ϣ
	BYTE							m_UserSecCard[PLAY_COUNT][MAX_COUNT];	//��ҵڶ����� 
	BYTE							m_CardType[2];							//�������
	bool							m_bHaveSecCard;							//�Ƿ��еڶ�����

	BYTE							m_bybeishu;                         //��ע����ׯ����
	INT64							m_byRobxianzhi[PLAY_COUNT];         //��ׯ���ƽ��
	INT64							m_byXiaZhuRes[PLAY_COUNT];          //��ע�������ƽ��

	bool							m_buserready[PLAY_COUNT];			//���׼�����
	
	bool							m_usercut[PLAY_COUNT];				//��¼����ע����;ǿ��/������ң���Ϸ�������Ҫ�ֶ��������
	BYTE                            m_usercutcount[PLAY_COUNT];         //��¼��Ҷ��߾���
	BYTE                            m_usercutnum;                       //��¼�����������
	bool							m_bHavecallbanker;					//�Ƿ��Ͷ�ׯ��Ϣ����ֹ�û�����û�в���
	bool							m_bHavexiazhu;						//�Ƿ�����ע��Ϣ����ֹ�û�����û�в���
	BYTE							m_callbanker[PLAY_COUNT];			//һ�ֽ�ׯ��¼��û����255���� 1�� ���� 0
	BYTE							m_tanpai[PLAY_COUNT][2];			//һ��̯�Ƽ�¼��̯�� 1�� δ̯�� 0
	BYTE							m_tanpaitype[PLAY_COUNT];			//һ��̯�����ͼ�¼��δ̯��255������Ϊ̯������ֵ	
	BYTE							m_byUserHandCount[PLAY_COUNT];		//�Ƿ������˵ڶ�����
	BYTE							m_tanpaiSecType[PLAY_COUNT];        //�ڶ���̯������
	bool                            m_btuizhu[PLAY_COUNT];              //�Ƿ�����ע
	bool                            m_blimit[PLAY_COUNT];               //�Ƿ���ע������С��
	bool                            m_btuizhulimit[PLAY_COUNT];         //�Ƿ���ע������ע��
	bool                            m_bSelectIndex[MAX_CARD_SET_INDEX]; //���ѡ�Ƶ�����						
	INT64                           m_ipayserver;                       //�����
public:
	unsigned int					m_rabot_ready_time;//��������׼���˵�ʱ��
	unsigned int					m_rabot_state;	//������״̬ 0 ���� ��1 ��Ϸ��
	INT							    m_chip[CHIP_COUNT];					//��ע����ֵ
	stu_userXiaZhu					m_xia_zhu_param;

	stu_userOpenCard				m_tan_pai_param;

	stu_callBanker					m_qiang_banker_param;

};
#endif