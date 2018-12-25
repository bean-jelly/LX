#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "gamemsg.h"
#include "basiccode/utility/BasicCriticalSection.h"
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
	void deal_send_xiazhu_info(char* buf, int len);
	//��ȡ�����עֵ ������עʱ��
	void get_user_chips();

private:
	unsigned char					m_makers_station;						//ׯ��λ��
	int                             m_chip[CHOUMA_COUNT];                   //����ֵ
	long long						m_usermoney;							//��ҽ��
	int								m_chip_num;								//�����ע����
	BYTE							m_chip_area;							//��ע����
	int								m_seat_userid[SEAT_COUNT];				//��λ�����userid
	BYTE							m_myPos;								//���λ��
	bool							m_bfirst;								
	user_out_chip_operate			m_xia_zhu_param;                        //��עֵ
};

#endif	