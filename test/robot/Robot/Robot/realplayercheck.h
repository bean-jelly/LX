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
	unsigned int						m_real_heart_count;					//������,δ���յ��������Ϣ����
	HWND								m_hWnd;
	unsigned int						m_club_room_id;						//����ID
	unsigned int						m_real_player_count;				//��������
	unsigned int						m_real_check_time;					//���˼��ʱ����
	userserver::usermanager*			m_pUserMgr[MAX_USER_PLAY];

	DCGR::stuAllDeskInfo                m_all_desk_info;					//���е�������Ϣ
	std::vector <idle_player_info>		m_idle_player_list;					//���л������б�
	unsigned int						m_desk_real_player[MAX_DESK_NUM];	//����������
	unsigned int                        m_desk_real_player_no_robat;		//ÿ�������ж�������ʱ�������Ӳ���Ҫ������
	unsigned int                        m_rabot_off_player_count;			//���������ߵĵ��������ֵ
	unsigned int						m_max_robot;						//ÿ����������Ļ���������
	unsigned int						m_desk_for_robot_count;				//�����������������������
	std::map<int, int>					m_desk_for_robot;					//����������/���������ƻ���������(�ɲ���������)

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
	//�������������Ϣ
	void setuserdeskinfo(unsigned int desk_index, const DCGR::RobotDeskInfo& deskinfo);	
	//β������Ԫ��
	void add_idle_player(idle_player_info ipi);
	//��ȡ��һ��Ԫ�أ�����ɾ��
	bool get_first_idle_player(idle_player_info &ipi);
	//�����ʵ��ң����Ż���������
	void player_match_desk();
	//���������Ƿ������������
	void robot_match_desk();
private:
	std::recursive_mutex				m_table_mutex;						//��������
};

