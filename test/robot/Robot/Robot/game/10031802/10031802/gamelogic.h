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
	//处理换桌
	virtual void deal_send_change_desk(unsigned int);
	virtual void kill_timer(unsigned int event_id);
	virtual void set_timer(unsigned int event_id, unsigned int elapse);
	virtual void OnTimer(unsigned int event_id, unsigned int user_id);
private:
	void deal_send_xiazhu_info(char* buf, int len);
	//获取玩家下注值 返回下注时间
	void get_user_chips();

private:
	unsigned char					m_makers_station;						//庄家位置
	int                             m_chip[CHOUMA_COUNT];                   //筹码值
	long long						m_usermoney;							//玩家金币
	int								m_chip_num;								//玩家下注次数
	BYTE							m_chip_area;							//下注区域
	int								m_seat_userid[SEAT_COUNT];				//座位上玩家userid
	BYTE							m_myPos;								//玩家位置
	bool							m_bfirst;								
	user_out_chip_operate			m_xia_zhu_param;                        //下注值
};

#endif	