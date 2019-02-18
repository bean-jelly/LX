#pragma once 
#ifndef __USERMANAGER_H_
#define __USERMANAGER_H_

#include <stdio.h> 
#include "basictable/GameRoomMsg.h"
#include "../tcp/tcpClientmanager.h" 
#include "useractionmanager.h" 
#include "../basegamelogic.h"
#include "../gameloadhelper.h"

class gamelogic;
namespace  userserver
{
	class usermanager :public tcpClientmanager
	{
	public:
		usermanager();
		virtual ~usermanager();
	public:
		void			init(char* param);
		void			user_aready(bool ready);
		void			user_change_desk(unsigned int chair_id);
	public:
		//deal tcp  callback
		virtual void	on_recv_event(unsigned int socket, char* buf, int len);
		virtual void	on_close_event(unsigned int socket, char* buf, int len);
		virtual void	on_connect_event(unsigned int socket);
	public:
		bool			on_user_login_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id);
		bool			on_user_action_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id);
	public:
		void			send_msg(unsigned int sub_id, char* buf, int len);
		void			set_my_station(unsigned char chair);
		void			set_swap_desk(bool swap);
		void			set_timer(unsigned int event_id, unsigned int elapse);
		void			kill_timer(unsigned int event_id);
		void			on_user_timer(unsigned int event_id);
		void			kill_all_timer();
		void			on_user_gl_timer(unsigned int event_id, unsigned int user_id);
		void			add_heart_break_count(){ m_heart_break_count++; }					//增加心跳包超时自动关闭计数
		int				get_heart_break_count() const { return m_heart_break_count; }
		void			set_game_play_count(int play_count);
	public:
		//处理MFC下线通知(桌子没有真人)
		void			deal_notify_user_left_from_MFC(bool on_off);
		void			sendLeftInfoTOMFC();
		int				userSitDesk(idle_player_info deskinfo);
		void			addUserToIdleList(unsigned int muserid, unsigned int userstate);
		void			updateUserInfo(unsigned int userId);
		void			userReConnet();
		//设置桌子信息
		void			setdeskuserinfo(const DCGR::RobotDeskInfo& deskinfo){ m_deskuserinfo = deskinfo; }
		//是否可以开始
		bool			OncheckUserready();
	public:
		unsigned int							m_my_userid;
		unsigned char							m_my_station;
		char                                    m_password[MAX_BUFFER];
		unsigned int							m_login_port;
		unsigned int							m_room_id;
		unsigned int							m_real_room_id;
		unsigned int							m_desk_index;
		unsigned int							m_private_id;
		unsigned int							m_private_no;
		userserver::useractionmanager*			m_user_action_mgr;
		unsigned int		                    m_basic_point;
		int                                     m_play_game_count;				//多少局后换桌
		int                                     m_play_game_count_min;			//最大局数换桌
		int                                     m_play_game_count_max;			//最少局数换桌
		int										m_recharge_limit;
		int										m_recharge_money;
		int										m_game_id;
		unsigned int							club_desk_club_room_desk_no;
		unsigned int							m_rule_id;
		char									m_player_token[MAX_BUFFER];
		char	                                m_php_address[MAX_PATH];
		char	                                m_game_name[MAX_BUFFER];
		basegamelogic*							m_game_mgr;
		unsigned long long						m_time;							//服务器时间
		unsigned int							m_usermgr_index;
		unsigned int                            m_rabot_ready_time;				//机器人已经准备了的时间
		bool									m_swap_desk;					//换桌 true ,搓桌 false
		long long								m_login_time;
		int                                     m_play_time;
		int                                     m_start_time;
		int										m_end_time;
		bool									m_user_statue;					//玩家状态 fasle 非游戏状态，true 游戏状态
		bool									m_offline_flag;					//false:机器人上线，true:机器人下线 （坐的桌子真人人数结果）
		int										m_rabot_off_player_count;		//真实玩家总在线数量,到达时不再配对机器人
		char									m_login_ip[MAX_BUFFER];
		HWND									m_hWnd;
	private:
		int										m_heart_break_count;			//心跳包超时自动关闭计数
	private:
		DCGR::RobotDeskInfo                     m_deskuserinfo;                 //当前桌子信息
	};
}



#endif //__USERMANAGER_H_
