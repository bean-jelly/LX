#pragma once
#include <stdio.h>
#include <set>
#include "tcp/tcpClientmanager.h"  
#include "basictable/GameRoomMsg.h"
#include "basictcp/basicnetdef.h" 
#include "datastruct.h"
#include "RobotManager\usermanager.h"

class CheckC :public userserver::tcpClientmanager
{
public:
	CheckC();
	~CheckC();

public:
	unsigned int						m_real_heart_count;					//心跳包,未接收到服务端信息次数
	HWND								m_hWnd;
	unsigned int						m_club_room_id;						//房间ID
	unsigned int						m_real_player_count;				//真人总数
	unsigned int						m_real_check_time;					//真人检测时间间隔
	userserver::usermanager*			m_pUserMgr[MAX_USER_PLAY];

	DCGR::stuAllDeskInfo                m_all_desk_info;					//所有的桌子信息
	std::vector <idle_player_info>		m_idle_player_list;					//空闲机器人列表
	unsigned int						m_desk_real_player[MAX_DESK_NUM];	//桌子真人数
	unsigned int                        m_desk_real_player_no_robat;		//每张桌子有多少真人时，该桌子不需要机器人
	unsigned int                        m_rabot_off_player_count;			//机器人下线的的真人最大值
	unsigned int						m_max_robot;						//每张桌子允许的机器人数量
	unsigned int						m_desk_for_robot_count;				//机器人自由坐桌最大桌子数
	std::map<int, int>					m_desk_for_robot;					//机器人桌子/该桌子限制机器人数量(可不包含真人)

public:
	virtual void on_recv_event(unsigned int socket, char* buf, int len);
	virtual void on_close_event(unsigned int socket, char* buf, int len);
	virtual void on_connect_event(unsigned int socket);

public:
	int  init(unsigned int club_room_id, char room_real_host[], int real_port, unsigned int real_off, unsigned int max_robot);
	void set_hwnd(HWND hWnd);
	void set_room_id(unsigned int club_room_id);
	void set_timer(unsigned int event_id, unsigned int elapse);
	void kill_timer(unsigned int event_id);
	void on_timer(unsigned int event_id);

	void sendToUserManger(bool on_off, unsigned int desk_index);
	//设置桌子玩家信息
	void setuserdeskinfo(unsigned int desk_index, const DCGR::RobotDeskInfo& deskinfo);	
	//尾部插入元素
	void add_idle_player(idle_player_info ipi);
	//获取第一个元素，并且删除
	bool get_first_idle_player(idle_player_info &ipi);
	//检测真实玩家，安排机器人坐桌
	void player_match_desk();
	//检测机器人是否可以自行坐桌
	void robot_match_desk();
private:
	std::recursive_mutex				m_table_mutex;						//可重入锁
};

