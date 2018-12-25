#include "useractionmanager.h"
#include "usermanager.h"  
#include "basiclog\basicloginterface.h"  
#include <sstream>

namespace userserver
{
	userserver::useractionmanager::useractionmanager()
	{
		m_userid = 0;
		m_user_chair = 255;
		m_user_mgr = NULL;
	}

	useractionmanager::~useractionmanager()
	{		
		m_user_mgr = NULL;
	}

	void useractionmanager::add_user_mgr(usermanager* mgr)
	{
		m_user_mgr = mgr;
	} 

	void useractionmanager::set_userid(unsigned int userid)
	{
		m_userid = userid;
	}

	void useractionmanager::set_socket(unsigned int socket)
	{
		m_socket = socket;
	}

	void useractionmanager::set_roomid(unsigned int roomid)
	{
		m_roomid = roomid;
	}

	void useractionmanager::set_real_roomid(unsigned int realroomid)
	{
		m_real_roomid = realroomid;
	}

	void useractionmanager::set_gameid(unsigned int gameid)
	{
		m_gameid = gameid;
	}

	void useractionmanager::set_userpass(char pass[])
	{
		memcpy(m_password, pass, sizeof(m_password));
	}

	void useractionmanager::set_basicPoint(unsigned int basicpoint)
	{
		m_basic_point = basicpoint;
	}

	void useractionmanager::set_rule_id(unsigned int rule_id)
	{
		m_rule_id = rule_id;
	}

	void useractionmanager::set_player_token(char player_token[])
	{
		memcpy(m_player_token, player_token, sizeof(m_player_token));
	}
	
	void useractionmanager::set_php_address(char phpadd[])
	{
		memcpy(php_address, phpadd, sizeof(php_address));
	}

	void useractionmanager::set_game_name(char gamename[])
	{
		memcpy(game_name, gamename, sizeof(game_name));
	}

	void useractionmanager::set_play_game_count( int gamecount)
	{
		m_player_game_count = gamecount;
	}

	void useractionmanager::set_play_game_count_min(int gamecount_min)
	{
		m_player_game_count_min = gamecount_min;
	}

	void useractionmanager::set_play_game_count_max(int gamecount_max)
	{
		m_player_game_count_max = gamecount_max;
	}

	void useractionmanager::set_club_room_desk_no(unsigned int deskno)
	{
		m_club_room_desk_no = deskno;
	}

	void useractionmanager::set_private_info(unsigned int id, unsigned int no,unsigned int index)
	{
		m_desk_index = index;
		m_private_id = id;
		m_private_no = no;
	}

	int useractionmanager::send_login_msg()
	{
		BASIC_LOG_INFO("useractionmanager::send_login_msg  handle=%0x  m_userid(%d) ", m_user_mgr, m_userid);
		//发送登录消息
		DCGR::MSG_UserLoginRoom   param;
		param.dwRoomID = m_real_roomid;
		param.dwUserID = m_userid;
		memcpy(param.szPWD, &m_password, sizeof(param.szPWD));
		BASIC_LOG_INFO("useractionmanager::send_login_msg 发送登录包间消息 m_real_roomid =%d, m_userid =%d ，param.szPWD = %s", m_real_roomid, m_userid, param.szPWD);
		if (NULL != m_user_mgr )
		{
			BASIC_LOG_INFO("useractionmanager::send_login_msg  包间消息 ");
			return m_user_mgr->on_send_event(m_socket, MID_USER_LOGIN_ROOM, SUB_USER_LOGIN_ROOM_ASK, &param, sizeof(DCGR::MSG_UserLoginRoom), 0);
		}
		return -1;
	}

	int useractionmanager::send_sit_msg(unsigned int deskindex, unsigned int deskstation)
	{
		DCGR::MSG_UserSit  param;
		param.dwUserID = m_userid;
		param.iDeskID = m_desk_index;
		param.iChairID = 255;
		param.iClubRoomID = m_roomid;
		param.iClubDeskID = m_club_room_desk_no;
		param.iClubDeskStation = 255;
		param.iClubID = 0;
		memcpy(param.szPassword, m_password, sizeof(m_password));
		BASIC_LOG_INFO("useractionmanager::send_sit_msg  handle= %0x  m_userid(%d) m_desk_index=%d m_private_id=%d m_private_no=%d , param.iClubDeskID  = %d", m_user_mgr, m_userid, m_desk_index, m_private_id, m_private_no,param.iClubDeskID);
		 
		if (NULL != m_user_mgr)
		{ 
			return m_user_mgr->on_send_event(m_socket, DC_GR_USER_ACTION, DC_ASS_GR_USER_SIT, &param, sizeof(DCGR::MSG_UserSit), 0);
		}
		return -1;
	}

	int useractionmanager::send_agree_msg(bool bagree)
	{
		DCGR::MSG_UserAgree param;
		param.dwUserID = m_userid;
		param.bDeskNO = m_desk_index;
		param.bDeskStation = m_user_chair;
		param.bAgreeGame = true;
		BASIC_LOG_INFO("send_agree_msg  handle=%0x  m_userid(%d) m_desk_index=%d bagree=%d", m_user_mgr, m_userid, m_desk_index, bagree);
 
		if (NULL != m_user_mgr)
		{
			return m_user_mgr->on_send_event(m_socket, DC_GR_GAMM_NOTIFY, DC_ASS_GR_USER_READY, &param, sizeof(DCGR::MSG_UserAgree), 0);
		}
		return -1;
	}

	int useractionmanager::send_get_game_info()
	{ 
		DCGR::StuRequestGameInfor param;
		param.uUserID = m_userid;
		BASIC_LOG_INFO("send_get_game_info  handle=%0x  m_userid(%d)  ", m_user_mgr, m_userid );
		if (NULL != m_user_mgr)
		{ 
			return  m_user_mgr->on_send_event(m_socket, DC_GR_GAMM_NOTIFY, DC_ASS_GR_GAMEINFOR, &param, sizeof(DCGR::StuRequestGameInfor), 0);
		}
		return -1;
	}

	int useractionmanager::user_sit(char* buf, int len, unsigned int code_id)
	{
		if (ERR_GR_SIT_SUCCESS == code_id)
		{
			DCGR::MSG_UserSitResult* pUserSit = reinterpret_cast<DCGR::MSG_UserSitResult*>(buf);
			if (NULL != pUserSit && NULL != m_user_mgr)
			{   
				if (m_userid == pUserSit->userinfo.iUserid)
				{ 
					m_user_chair = pUserSit->userinfo.iChairID; 
					m_user_mgr->set_my_station(m_user_chair);
					BASIC_LOG_INFO("useractionmanager::user_sit  handle=%0x  m_userid(%d) sit success m_user_desk_index=%d,m_user_chair=%d", m_user_mgr, m_userid, m_desk_index, m_user_chair);
				}
			}
		}
		else
		{
			BASIC_LOG_ERROR("useractionmanager::user_sit  handle=%0x  m_userid(%d) sit fail code_id=%d, ", m_user_mgr, m_userid, code_id); 
		}
		return 0;
	}

	int useractionmanager::user_up(char* buf, int len, unsigned int code_id)
	{
		
		if (ERR_GR_SIT_SUCCESS == code_id)
		{ 
			BASIC_LOG_INFO("user_up  handle=%0x  m_userid(%d) sit up  ", m_user_mgr, m_userid);
		}
		return 0;
	}

	int useractionmanager::user_cut(char* buf, int len, unsigned int code_id)
	{ 
		return 0;
	}

	int useractionmanager::user_left(char* buf, int len, unsigned int code_id)
	{
		DCGR::MSG_UserLeft*  pUserLeft = reinterpret_cast<DCGR::MSG_UserLeft*>(buf);
		  
		return 0;
	}

	int useractionmanager::user_change_desk_result(char* buf, int len, unsigned int code_id)
	{
		DCGR::MSG_NoitfyRequestChangeDeskResult*  pUserChangeDesk = reinterpret_cast<DCGR::MSG_NoitfyRequestChangeDeskResult*>(buf);
		
		if (NULL != pUserChangeDesk)
		{
			if (pUserChangeDesk->bSuccess)
			{
				BASIC_LOG_INFO("useractionmanager::user_change_desk_result  m_userid = (%d), pUserChangeDesk->iUserID = %d 请求换桌结果 可以换桌", m_userid, pUserChangeDesk->iUserID);
				if (pUserChangeDesk->iUserID == m_userid)
				{
					
				}
				
			}
			else
			{
				BASIC_LOG_INFO("user_change_desk_result m_userid = (%d) pUserChangeDesk->iUserID = %d请求换桌结果 不可以换桌", m_userid, pUserChangeDesk->iUserID);
			}

		}
		return 0;
	}
	
	int useractionmanager::user_change_desk(unsigned int chair_id)
	{
		DCGR::MSG_UserRequestChangeDesk  param;
		param.iUserID = chair_id;

		if (NULL != m_user_mgr)
		{
			BASIC_LOG_INFO("user_change_desk pUserChange->iUserID = %d m_user_mgr  m_user_mgr->m_my_userid = %d", chair_id, m_user_mgr->m_my_userid);

			return m_user_mgr->on_send_event(m_socket, DC_GR_USER_ACTION, DC_ASS_GR_REQUES_CHANGE, &param, sizeof(DCGR::MSG_UserRequestChangeDesk), 0);
		}
		return -1;
	}

	int useractionmanager::user_ask_left(unsigned int user_id)
	{
		DCGR::MSG_UserLeft  param;
		param.dwUserID = user_id;
		BASIC_LOG_INFO("useractionmanager::user_ask_left pparam.dwUserID = %d ", user_id);

		char dstr[256] = { 0 };
		sprintf_s(dstr, 256, "[Robot] user_ask_left ==> ask_left[%d]", user_id);
		OutputDebugStringA(dstr);

		if (NULL != m_user_mgr)
		{
			return m_user_mgr->on_send_event(m_socket, DC_GR_USER_ACTION, DC_ASS_GR_USER_LEFT, &param, sizeof(DCGR::MSG_UserLeft), 0);
		}
		return -1;
	}

	int useractionmanager::user_agree(char* buf, int len)
	{
		DCGR::MSG_UserAgree* pUserAgree = reinterpret_cast<DCGR::MSG_UserAgree*>(buf);

		if (NULL != pUserAgree)
		{
			if (pUserAgree->dwUserID == m_userid && pUserAgree->bAgreeGame == 1)
			{
				BASIC_LOG_INFO("user_agree  handle=%0x  m_userid(%d) agree ", m_user_mgr, m_userid);
				send_get_game_info();
			} 
		}
		return 0;
	}
	
	int useractionmanager::send_game_msg(unsigned int sub_id, char* buf, unsigned int len)
	{
		return m_user_mgr->on_send_event(m_socket, DC_GR_GAMM_NOTIFY, sub_id, buf, len, 0);
	}

	bool useractionmanager::on_set_timer(unsigned int timeid, unsigned int interval)
	{ 
		SetTimer(m_handle, m_user_index + MFC_TIME_START_ID * timeid, interval, NULL);
		return true;
	}

	bool useractionmanager::on_kill_timer(unsigned int timeid)
	{ 
		KillTimer(m_handle, m_user_index + MFC_TIME_START_ID * timeid);
		return true;
	}

	bool useractionmanager::on_timer(unsigned int timeid)
	{
		switch (timeid)
		{
		case TIME_USER_RESTART_DESK_INFO:
		{
			//玩家重新请求桌子信息
			on_kill_timer(TIME_USER_RESTART_DESK_INFO);
			BASIC_LOG_INFO("TIME_USER_RESTART_DESK_INFO player_id = %d 玩家重新请求桌子信息 ", m_userid);
			if (NULL != m_user_mgr )
			{
				return 0;
			}
			break;
		}	
		case TIME_USER_CHECK_SERVOCE:
		{
			//每隔五秒發送新跳包
			m_user_mgr->add_heart_break_count();
			if (m_user_mgr->get_heart_break_count() > 6) //超过6个心跳包（30秒）没反应
			{
				BASIC_LOG_ERROR("TIME_USER_CHECK_SERVOCE  Close Client id = %d , m_socket(%d)", m_user_mgr->m_my_userid, m_socket);
				on_kill_timer(TIME_USER_CHECK_SERVOCE);
				m_user_mgr->sendLeftInfoTOMFC();
				m_user_mgr->close_client(m_socket);
			}
			else
			{
				m_user_mgr->on_send_event(m_socket, 1, 1, NULL, 0, 0);
			}
			break;
		}
		default:
			break;
		}
		return true;
	}

	unsigned char useractionmanager::on_get_my_staiton()
	{
		return m_user_chair;
	}

	bool useractionmanager::restart_get_desk_info(DCGR::MSG_UserSit  &userSit)
	{
		BASIC_LOG_INFO("restart_get_desk_info 重新请求桌子信息 m_user_mgr->m_my_userid = %d,userSit.dwUserID = %d", m_user_mgr->m_my_userid,userSit.dwUserID );
		Json::Value param;
		Json::Value data;
		//请求获取桌子信息
		param["action"] = "clubdeskinfo";
		param["version"] = "v10001";
		param["key_value"] = 1;
		param["flag_value"] = 1;
		std::istringstream iss(std::to_string(time(0)));
		int num;
		iss >> num;
		param["sign_value"] = num;
		data["club_room_id"] = m_user_mgr->m_room_id;
		data["club_room_club_id"] = 0;
		data["club_desk_id"] = (m_user_mgr->m_swap_desk ?  m_club_room_desk_no : -1);//-1 随机搓桌 
		data["club_desk_rule_id"] = m_user_mgr->m_rule_id;
		data["club_desk_param"] = "";
		data["player_id"] = m_user_mgr->m_my_userid;
		std::string token = m_user_mgr->m_player_token;
		data["player_token"] = token.c_str();

		param["data_value"] = data;

		Json::FastWriter writer;
		std::string  keyvalue = "param=" + writer.write(param);
		utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
		utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
		utility::BasicHttpContext http;		
		
		TCHAR wc[MAX_PATH];
		CharToTchar(m_user_mgr->m_php_address,wc);
		int re = 0;

		http.HttpSendConnectEx(wc, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
		std::string  sz((char*)szBuffer.buffer());
		std::string  result((char*)szGetBuffer.buffer());
		BASIC_LOG_INFO("restart_get_desk_info playerid = (%d),重新请求桌子信息路径 = %s", m_user_mgr->m_my_userid, sz.c_str());
		BASIC_LOG_INFO("restart_get_desk_info  playerid = (%d),重新请求桌子信息结果 result = %s", m_user_mgr->m_my_userid, result.c_str());
		HttpClubDeskInfo clubDeskInfo;
		re = gameservice::basichttphelper::parse_matching_info((char*)szGetBuffer.buffer(), 0, clubDeskInfo);
		if (0 == re)
		{
			userSit.iClubID = clubDeskInfo.desk_info.club_desk_club_id;
			userSit.iClubRoomID = clubDeskInfo.desk_info.club_desk_club_room_id;
			userSit.iClubDeskID = clubDeskInfo.desk_info.club_desk_club_room_desk_no;
			userSit.iClubDeskStation = 255;
			userSit.iDeskID = clubDeskInfo.desk_info.club_desk_desk_no;
			userSit.iChairID = 255;
			BASIC_LOG_INFO("restart_get_desk_info 重新请求桌子信息结果 解析后 playerid = (%d) userSit.iClubID = %d,userSit.iClubRoomID = %d,userSit.iClubDeskID = %d,userSit.iDeskID = %d", m_user_mgr->m_my_userid, userSit.iClubID, userSit.iClubRoomID, userSit.iClubDeskID, userSit.iDeskID);

			return true;
		}
		else if (-1 == re)
		{
			BASIC_LOG_INFO("restart_get_desk_info  playerid = (%d),请求结果错误，隔1s后重新请求桌子信息", m_user_mgr->m_my_userid);
			on_set_timer(TIME_USER_RESTART_DESK_INFO,1000);
		}
	
		return false;
	}

	void useractionmanager::CharToTchar(const char * _char, TCHAR * tchar)
	{
		int iLength;
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
	}

	void useractionmanager::set_handle(HWND handle)
	{
		m_handle = handle;
	}

	void useractionmanager::set_user_index(unsigned int index)
	{
		m_user_index = index;
	}

	void useractionmanager::set_user_login_time(long long time)
	{
		m_login_time = time;
	}

	void useractionmanager::set_user_play_time(int time)
	{
		m_play_time = time;
	}
	
	void useractionmanager::set_user_start_time(int starttime)
	{
		m_start_time = starttime;
	}

	void useractionmanager::set_user_end_time(int endtime)
	{
		m_end_time = endtime;
	}

	void useractionmanager::user_relogin(unsigned int	dwUserID)
	{
		BASIC_LOG_INFO("user_relogin 离开后重新登录桌子 dwUserID = %d", dwUserID);

		Json::Value param;
		Json::Value data;
		//请求获取桌子信息
		param["action"] = "clubdeskinfo";
		param["version"] = "v10001";
		param["key_value"] = 1;
		param["flag_value"] = 1;
		std::istringstream iss(std::to_string(time(0)));
		int num;
		iss >> num;
		param["sign_value"] = num;

		data["club_room_id"] = m_user_mgr->m_room_id;
		data["club_room_club_id"] = 0;
		data["club_desk_id"] = (m_user_mgr->m_swap_desk ? m_club_room_desk_no : -1);
		data["club_desk_rule_id"] = m_user_mgr->m_rule_id;
		data["club_desk_param"] = "";
		data["player_id"] = m_user_mgr->m_my_userid;
		std::string token = m_user_mgr->m_player_token;
		data["player_token"] = token.c_str();

		param["data_value"] = data;

		Json::FastWriter writer;
		std::string  keyvalue = "param=" + writer.write(param);
		utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
		utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
		utility::BasicHttpContext http;

		TCHAR wc[MAX_PATH];
		CharToTchar(m_user_mgr->m_php_address, wc);
		int re = 0;

		http.HttpSendConnectEx(wc, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
		std::string  sz((char*)szBuffer.buffer());
		std::string  result((char*)szGetBuffer.buffer());
		BASIC_LOG_INFO("user_relogin playerid = (%d),离开后重新请求桌子信息路径 = %s", m_user_mgr->m_my_userid, sz.c_str());
		BASIC_LOG_INFO("user_relogin  playerid = (%d),离开后重新请求桌子信息结果 result = %s", m_user_mgr->m_my_userid, result.c_str());
		HttpClubDeskInfo clubDeskInfo;
		re = gameservice::basichttphelper::parse_matching_info((char*)szGetBuffer.buffer(), 0, clubDeskInfo);
		if (0 == re)
		{
			DCGR::MSG_UserSit  userSit;
			userSit.dwUserID = dwUserID;

			userSit.iClubID = clubDeskInfo.desk_info.club_desk_club_id;
			userSit.iClubRoomID = clubDeskInfo.desk_info.club_desk_club_room_id;
			userSit.iClubDeskID = clubDeskInfo.desk_info.club_desk_club_room_desk_no;
			userSit.iClubDeskStation = 255;
			userSit.iDeskID = clubDeskInfo.desk_info.club_desk_desk_no;
			userSit.iChairID = 255;
			BASIC_LOG_INFO("restart_get_desk_info 离开后重新重新请求桌子信息结果 解析后 发坐桌 playerid = (%d) userSit.iClubID = %d,userSit.iClubRoomID = %d,userSit.iClubDeskID = %d,userSit.iDeskID = %d", m_user_mgr->m_my_userid, userSit.iClubID, userSit.iClubRoomID, userSit.iClubDeskID, userSit.iDeskID);
			m_user_mgr->on_send_event(m_socket, DC_GR_USER_ACTION, DC_ASS_GR_USER_SIT, &userSit, sizeof(DCGR::MSG_UserSit), 0);

			return;
		}
		else if (-1 == re)
		{
			BASIC_LOG_INFO("restart_get_desk_info  playerid = (%d),离开后重新请求结果错误，隔1s后重新请求桌子信息", m_user_mgr->m_my_userid);
			on_set_timer(TIME_USER_RESTART_DESK_INFO, 1000);
		}

		return;
	}

	//机器人游戏时间是否结束
	bool useractionmanager::bPlayTimeOver()
	{
		long long current_times = time(0);
		if (current_times > m_login_time + m_play_time)
		{	BASIC_LOG_INFO("机器人游戏时间结束 id = %d",m_userid);
			return  true;
		}

		return false;
	}

	//机器人上下线时间是否到了
	bool useractionmanager::bOnOffLimitTimeOver()
	{
		time_t rawtime;
		time(&rawtime);
		struct tm  timeinfo;
		localtime_s(&timeinfo, &rawtime);
		int current_time = timeinfo.tm_hour * 10000 + timeinfo.tm_min * 100 + timeinfo.tm_sec;

		if (m_start_time > m_end_time )
		{
			if (current_time > m_end_time && current_time < m_start_time)
			{
				BASIC_LOG_INFO("机器人上下线时间到了 id = %d", m_userid);
				return true;
			}
		}
		else
		{
			if (current_time > m_end_time || current_time < m_start_time)
			{
				BASIC_LOG_INFO("机器人上下线时间到了 id = %d", m_userid);
				return true;
			}
		}
		return false;
	}
}
