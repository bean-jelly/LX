#pragma once 
#ifndef __USERACTIONMANAGER_H_
#define __USERACTIONMANAGER_H_
#include "utility/BasicType.h"
#include "..\BasicHttpContext.h" 
#include "json\json.h" 
#include "..\datastruct.h"
#include "basiccode\utility\BasicMutex.h"
#include "basiccode\utility\BasicCriticalSection.h"
#include "basiccode\utility\BasicThreadPool.h"
#include "basiccode\utility\BasicBuffer.h"
#include "..\basichttphelper.h"
#include <sstream>

namespace userserver
{
	class usermanager;
	class useractionmanager
	{
	public:
		useractionmanager();
		~useractionmanager();

	public:
		void add_user_mgr(usermanager* mgr);
		void set_userid(unsigned int userid);
		void set_socket(unsigned int socket);
		void set_roomid(unsigned int roomid);
		void set_real_roomid(unsigned int realroomid);
		void set_gameid(unsigned int gameid);
		void set_private_info(unsigned int id, unsigned int no, unsigned int index);
		void set_userpass(char pass[]);
		void set_basicPoint(unsigned int basicpoint);
		void set_rule_id(unsigned int rule_id);
		void set_player_token(char player_token[]);
		void set_php_address(char phpadd[]);
		void set_game_name(char gamename[]);
		void set_play_game_count(int gamecount);
		void set_play_game_count_min(int gamecount_min);
		void set_play_game_count_max(int gamecount_max);
		void set_club_room_desk_no(unsigned int deskno);
		void set_handle(HWND handle);
		void set_user_index(unsigned int index);
		void set_user_login_time(long long time);
		void set_user_play_time(int time);
		void set_user_start_time(int starttime);
		void set_user_end_time(int endtime);
		bool restart_get_desk_info(DCGR::MSG_UserSit  &param);
		void CharToTchar(const char * _char, TCHAR * tchar);

	public:
		int send_login_msg();
		int send_sit_msg(unsigned int deskindex, unsigned int deskstation);
		int send_agree_msg(bool bagree);
		int send_get_game_info();
		int send_game_msg(unsigned int sub_id, char* buf, unsigned int len);

	public:
		int user_sit(char* buf, int len, unsigned int code_id);
		int user_up(char* buf, int len, unsigned int code_id);
		int user_cut(char* buf, int len, unsigned int code_id);
		int user_left(char* buf, int len, unsigned int code_id);
		int user_change_desk(unsigned int chair_id);
		int user_ask_left(unsigned int user_id);
		int user_change_desk_result(char* buf, int len, unsigned int code_id);
		void user_relogin(unsigned int	dwUserID);
		void get_desk_info();
		bool bPlayTimeOver();
		bool bOnOffLimitTimeOver();
		int	user_agree(char* buf, int len);

	public:
		bool on_set_timer(unsigned int timeid, unsigned int interval);
		bool on_kill_timer(unsigned int timeid);
		bool on_timer(unsigned int timeid);

	public:
		unsigned char on_get_my_staiton();

	public:
		unsigned int			m_userid;
		unsigned char			m_user_chair;
		unsigned int			m_socket;
		unsigned int			m_roomid;
		unsigned int			m_real_roomid;
		unsigned int			m_gameid;
		unsigned int			m_desk_index;
		unsigned int			m_private_id;
		unsigned int			m_private_no;
		unsigned char			m_password[MAX_BUFFER];
		unsigned char           m_room_pass[20];
		unsigned int            m_basic_point;
		int						m_player_game_count;
		int						m_player_game_count_min;
		int						m_player_game_count_max;
		unsigned int			m_club_room_desk_no;
		unsigned int			m_rule_id;
		char					m_player_token[MAX_BUFFER];
		char					php_address[MAX_PATH];
		char					game_name[MAX_BUFFER];
		usermanager*			m_user_mgr;
		HWND					m_handle;
		unsigned int			m_user_index;
		long long               m_login_time;
		int						m_play_time;
		int						m_start_time;
		int						m_end_time;
		DCGR::MSG_UserSit		m_param;						//保存请求换桌数据			
	};
}



#endif //__USERACTIONMANAGER_H_