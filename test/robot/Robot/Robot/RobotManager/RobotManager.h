#ifndef _ROBOTMANAGER_H_
#define _ROBOTMANAGER_H_

#include <stdio.h>
#include "GameRoomMsg.h"
#include "RobotAction.h"
#include "gameloadhelper.h"

namespace RobotManager
{
	class RobotManager : public RobotAction
	{
	public:
		RobotManager();
		~RobotManager();
	public:
		void									init_data(UserInitData* param);
	public:
		virtual void							on_recv_event(unsigned int socket, char* buf, int len);
		virtual void							on_close_event(unsigned int socket, char* buf, int len);
		virtual void							on_connect_event(unsigned int socket);
	public:
		void									send_tcp_msg(unsigned int sub_id, char* buf, int len);
		void									send_mfc_msg(unsigned int sub_id, char* buf, int len);
	public:
		void									set_timer(unsigned int event_id, unsigned int elapse);
		void									on_timer(unsigned int event_id);
		void									kill_timer(unsigned int event_id);
		void									kill_all_timer();
	public:
		void									add_robot_list(int robot_index, unsigned int robot_state);
	public:
		//业务逻辑
		void									on_deal_robot_login_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id);
		void									on_deal_robot_action_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id);
		void									set_desk_info(const DCGR::RobotDeskInfo* deskinfo);
		void									updateUserInfo(unsigned int user_id);
		bool									check_robot_ready();
		void									set_game_play_count(int play_count);
		void									userReConnet();
	public:
		void									add_heart_break_count()			{ m_heart_break_count++; }
		int										get_heart_break_count() const	{ return m_heart_break_count; }
	public:
		int										m_heart_break_count;			//心跳包超时自动关闭计数
		DCGR::RobotDeskInfo                     m_desk_info;					//当前桌子信息
		std::string								m_php_address;
		std::string								m_login_ip;
		std::string								m_password;
		std::string								m_player_token;
		unsigned int							m_login_port;
		unsigned int							m_club_room_id;
		unsigned int							m_club_rule_id;
		int										m_game_id;
		unsigned int							m_club_room_desk_no;
		unsigned int							m_room_id;
	private:
		int										m_recharge_limit;
		int										m_recharge_money;
	public:
		HWND									m_windos_handle;
	public:
		unsigned int							m_robot_user_id;                //机器人真实ID
		unsigned char							m_robot_chair_id;               //机器人桌子位置
		unsigned int							m_desk_index;                   //桌子索引
		int                                     m_play_game_count;				//多少局后换桌
		int                                     m_play_game_count_min;			//最大局数换桌
		int                                     m_play_game_count_max;			//最少局数换桌			
	public:
		bool									m_robot_state;					//玩家状态 fasle 非游戏状态，true 游戏状态
	public:
		unsigned int							m_robot_manage_index;
	};
}

#endif	_ROBOTMANAGER_H_