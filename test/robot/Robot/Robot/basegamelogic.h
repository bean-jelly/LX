#pragma once
#ifndef __GAMEBASELOGIC_H_
#define __GAMEBASELOGIC_H_
#include "user/usermanager.h"
#include "basiccode/utility/BasicCriticalSection.h"

class basegamelogic
{
public:
	basegamelogic(){};
	virtual ~basegamelogic(){};
public:
	void add_user_mgr(userserver::usermanager* mgr){ m_user_mgr = mgr; }
	void set_g_handle(HWND handle){ m_handle = handle; }
	void set_g_user_index(unsigned int index) { m_user_index = index; }
public:
	//公共换桌
	virtual void deal_send_change_desk(unsigned int chair_id){}
	
	//需要重载下面函数
	virtual bool gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id){ return true; }
	virtual bool gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id){ return true; }
	virtual void OnTimer(unsigned int event_id, unsigned int user_id){}
	virtual void set_timer(unsigned int event_id, unsigned int elapse){}
	virtual void kill_timer(unsigned int event_id){}
	
protected:
	userserver::usermanager*			m_user_mgr;
	std::recursive_mutex				m_game_mutex;

	HWND								m_handle;
	unsigned int						m_user_index;

	int									game_count;							//玩了多少局
	unsigned int						m_rabot_ready_time;					//机器人已准备了的时间
	unsigned int						m_rabot_state;						//机器人状态 0 空闲 ，1 游戏中
};
#endif //__GAMELOGIC_H_