#pragma once
#include "usermanager.h"
#include "basictable\GameRoomMsg.h"
#include "basictcp/basicnetdef.h" 
#include "../datastruct.h" 
#include "basiclog\basicloginterface.h"  
#include <atlstr.h>
#include <sstream>
#include <string>

namespace userserver
{
	usermanager::usermanager()
	{
		m_my_userid = 0;
		m_login_port = 0;
		m_user_action_mgr = NULL;
		m_game_mgr = NULL;
		m_my_station = 255;
		m_user_statue = false;
		m_offline_flag = false;
	}

	usermanager::~usermanager()
	{
		delete m_user_action_mgr;
		m_user_action_mgr = NULL;
	}

	void usermanager::init(char* param)
	{
		UserInitData* pParam = reinterpret_cast<UserInitData*>(param);
		if (NULL == pParam)
		{
			return;
		}
		m_my_userid					= pParam->user_id;
		m_room_id					= pParam->room_id;
		m_real_room_id				= pParam->real_room_id;
		m_desk_index				= pParam->index;
		m_private_id				= pParam->private_id;
		m_private_no				= pParam->private_no;
		m_basic_point				= pParam->basic_point;
		m_play_game_count			= pParam->play_game_count;
		m_play_game_count_min		= pParam->play_game_count_min;
		m_play_game_count_max		= pParam->play_game_count_max;
		club_desk_club_room_desk_no = pParam->club_desk_club_room_desk_no;
		m_rule_id					= pParam->rule_id;
		m_game_id					= pParam->player_game_id;
		m_hWnd						= pParam->handle;
		m_usermgr_index				= pParam->usermgr_index;
		m_login_time				= pParam->login_time;
		m_play_time					= pParam->play_time;
		m_start_time				= 0;
		m_end_time					= 0;
		m_rabot_off_player_count	= pParam->robot_off_player_count;
		m_recharge_limit			= pParam->recharge_limit;
		m_recharge_money			= pParam->recharge_money;
		m_login_port				= pParam->login_port;
		memcpy(m_player_token, &pParam->player_token, sizeof(m_player_token));
		memcpy(m_game_name, &pParam->game_name, sizeof(m_game_name));
		memcpy(m_php_address, &pParam->php_address, sizeof(m_php_address));
		memcpy(m_password, &pParam->user_password, sizeof(m_password));
		memcpy(m_login_ip, &pParam->login_ip, sizeof(m_login_ip));

		if (m_user_action_mgr == NULL)
		{
			m_user_action_mgr = new userserver::useractionmanager();
		}
		else
		{
			delete m_user_action_mgr;
			m_user_action_mgr = NULL;
			m_user_action_mgr = new userserver::useractionmanager();
		}

		if (NULL != m_user_action_mgr)
		{
			m_user_action_mgr->add_user_mgr(this);
			m_user_action_mgr->set_roomid(m_room_id);
			m_user_action_mgr->set_real_roomid(m_real_room_id);
			m_user_action_mgr->set_userid(m_my_userid);
			m_user_action_mgr->set_userpass(m_password);
			m_user_action_mgr->set_basicPoint(m_basic_point);
			m_user_action_mgr->set_php_address(m_php_address);
			m_user_action_mgr->set_game_name(m_game_name);
			m_user_action_mgr->set_play_game_count(m_play_game_count);
			m_user_action_mgr->set_play_game_count_min(m_play_game_count_min);
			m_user_action_mgr->set_play_game_count_max(m_play_game_count_max);
			m_user_action_mgr->set_club_room_desk_no(club_desk_club_room_desk_no);
			m_user_action_mgr->set_private_info(m_private_id, m_private_no, m_desk_index);
			m_user_action_mgr->set_handle(m_hWnd);
			m_user_action_mgr->set_user_index(m_usermgr_index);
			m_user_action_mgr->set_user_login_time(m_login_time);
			m_user_action_mgr->set_user_play_time(m_play_time);
			m_user_action_mgr->set_user_start_time(m_start_time);
			m_user_action_mgr->set_user_end_time(m_end_time);
		}
		if (m_game_mgr == NULL)
		{
			std::string szdllname = std::to_string(m_game_id);
			szdllname.append("_AI").append(".dll");
			::gameloadhelper::get_instance()->init_load(szdllname.c_str());
			m_game_mgr = (basegamelogic*)::gameloadhelper::get_instance()->create_logic();		
			if (m_game_mgr != NULL)
			{
				m_game_mgr->add_user_mgr(this);
				m_game_mgr->set_g_handle(m_hWnd);
				m_game_mgr->set_g_user_index(m_usermgr_index);
			}
		}
		std::string ip = pParam->login_ip;
		BASIC_LOG_INFO("init 初始化  handle=%d  userid=%d ,Ip = (%s),post = (%d)", this, m_my_userid, ip.c_str(), pParam->login_port);
		int reinit = init_tcp(pParam->login_ip, pParam->login_port);
		int restart = start_tcp();
		BASIC_LOG_INFO("返回结果 reinit = %d, restart = %d", reinit, restart);
	}

	//deal tcp callback
	void usermanager::on_recv_event(unsigned int socket, char* buf, int len)
	{
		DCGR::NetMessageHead* header = reinterpret_cast<DCGR::NetMessageHead*>(buf);
		if (NULL == header)
		{
			return;
		}
		int data_len = len - sizeof(basicnet::_BasicCommandHeader);

		if (data_len < 0)
		{
			return;
		}
		char buf_data[SOCKET_BUFFER] = { 0 };
		memcpy(buf_data, buf + sizeof(basicnet::_BasicCommandHeader), data_len);
		m_heart_break_count = 0;
		switch (header->bMainID)
		{
			case 1:
			{
				if (header->bAssistantID == 3)
				{
					basicnet::SBasicConnectSuccess* connet = reinterpret_cast<basicnet::SBasicConnectSuccess*>(buf + sizeof(basicnet::_BasicCommandHeader));
					if (NULL != connet)
					{
						BASIC_LOG_INFO("usermanager::on_recv_event  处理 1 3 connet->m_time = %I64d", connet->m_time);
						this->m_time = connet->m_time;
						set_time(connet->m_time);
						m_user_action_mgr->send_login_msg();
					}
					set_timer(TIME_USER_CHECK_SERVOCE, 5000);
				}
			}
				break;
			case MID_USER_LOGIN_ROOM:
			{
				on_user_login_msg(header->bAssistantID, buf_data, data_len, header->bHandleCode);
			}
				break;
			case MID_ROOM_MSG:
			{

			}
				break;
			case DC_GR_USER_ACTION:
			{
				on_user_action_msg(header->bAssistantID, buf_data, data_len, header->bHandleCode);
			}
				break;
			case DC_GR_ROOM:
			{

			}
				break;
			case DC_GR_GAMM_FRAME:
			{
				m_game_mgr->gameframe(header->bAssistantID, buf_data, data_len, header->bHandleCode);
			}
				break;
			case DC_GR_GAMM_NOTIFY:
			{
				m_game_mgr->gamenotify(header->bAssistantID, buf_data, data_len, header->bHandleCode);
			}
				break;
			default:
				break;
		}
	}

	void usermanager::on_close_event(unsigned int socket, char* buf, int len)
	{
		BASIC_LOG_INFO("on_close_event  handle=%d  userid=%d", this, m_my_userid);
	}

	void usermanager::on_connect_event(unsigned int socket)
	{
		BASIC_LOG_INFO("on_connect_event 处理连接成功  handle=%d  userid=%d  socket=%d ", this, m_my_userid, socket);
		if (NULL != m_user_action_mgr)
		{
			BASIC_LOG_INFO("on_connect_event  handle=%d  userid=%d    socket=%d ", this, m_my_userid, socket);
			m_user_action_mgr->set_socket(socket);
		}
	}

	bool usermanager::on_user_action_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id)
	{
		if (NULL != m_user_action_mgr)
		{
			BASIC_LOG_INFO("usermanager::on_user_action_msg  handle=%d  userid=%d    sub_id=%d ", this, m_my_userid, sub_id);
			switch (sub_id)
			{
				//离开结果， 每一个玩家都能收到，注意玩家id区别
				case DC_ASS_GR_USER_LEFT_RESULT:
				{
					m_user_statue = false;
					DCGR::MSG_UserLeftResult *UserSit = reinterpret_cast <DCGR::MSG_UserLeftResult *>(buf);
					BASIC_LOG_INFO("usermanager::on_user_action_msg  玩家UserSit->dwUserID = %d 离开结果 原因 = %d, m_user_action_mgr->m_userid = %d", UserSit->dwUserID, UserSit->dwCode, m_my_userid);
					if (UserSit->dwUserID == m_my_userid)
					{
						//离开成功
						if (RES_UP_RESULT_SUCCESS == code_id)
						{
							if (UserSit->dwCode == RES_UP_RESULT_CODE_9)
							{
								//金币过多
								BASIC_LOG_INFO("usermanager::on_user_action_msg  玩家 m_my_userid  = %d，金币过多 关闭socket", UserSit->dwUserID, UserSit->dwCode);
								::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)UserSit);
								//关闭连接
								m_user_action_mgr->on_kill_timer(TIME_USER_CHECK_SERVOCE);
								close_client(m_socket);
							}
							else if (UserSit->dwCode == RES_UP_RESULT_CODE_4)
							{
								//金币不足
								BASIC_LOG_INFO("usermanager::on_user_action_msg  玩家 m_my_userid  = %d，金币过多 关闭socket", UserSit->dwUserID, UserSit->dwCode);
								::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)UserSit);
								if (m_recharge_money > 0)
								{
									addUserToIdleList(m_my_userid, 1);
								}
								else
								{
									//关闭连接
									m_user_action_mgr->on_kill_timer(TIME_USER_CHECK_SERVOCE);
									close_client(m_socket);
								}
							}
							else
							{
								//其余的离开，全部归于空闲队列中等待
								addUserToIdleList(m_my_userid, 1);
								//离开消息
								::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)UserSit);
								BASIC_LOG_INFO("usermanager::on_user_action_msg  玩家 m_my_userid  = %d 回归空闲队列等待, 离开 原因 = %d ", UserSit->dwUserID, UserSit->dwCode);

							}
						}
					}
					break;
				}
				case DC_ASS_GR_USER_SIT:
				case DC_ASS_GR_USER_SIT_RESULT:
				{
					DCGR::MSG_UserSitResult* pUserSit = reinterpret_cast<DCGR::MSG_UserSitResult*>(buf);
					if (ERR_GR_SIT_SUCCESS == code_id)
					{
						if (pUserSit->userinfo.iUserid == m_my_userid)
						{
							m_user_action_mgr->user_sit(buf, len, code_id);							
							//设定局数
							srand(time(0) + m_my_userid);
							int range = m_play_game_count_max - m_play_game_count_min + 1;
							int count = rand() % range + m_play_game_count_min;
							set_game_play_count(count);

							m_user_statue = true;
							m_game_mgr->kill_timer(TIME_USER_READY_CHECK_START);
							m_game_mgr->set_timer(TIME_USER_READY_CHECK_START, 1000);
							m_user_action_mgr->send_get_game_info();
							//发送准备消息
							user_aready(true);
							::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LOGIN_RESULT, (LPARAM)pUserSit);

							BASIC_LOG_INFO("on_user_action_msg robot sit sucess m_my_userid = %d", m_my_userid);
						}
					}
					else
					{
						if (ERR_GR_MONEY_LIMIT == code_id)  //金币不足特殊处理
						{
							m_user_statue = false;
							m_user_action_mgr->on_kill_timer(TIME_USER_CHECK_SERVOCE);
							close_client(m_socket);

							DCGR::MSG_UserLeftResult UserSit;
							UserSit.dwUserID = m_my_userid;
							UserSit.dwCode = RES_UP_RESULT_CODE_4;

							::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)&UserSit);
							BASIC_LOG_INFO("usermanager::on_user_action_msg 坐桌成功结果 金币不足离开，关闭socket userid = %d,", m_my_userid);
						}
						else
						{
							//坐桌失败,直接归于空闲队列中等待
							m_user_statue = false;
							BASIC_LOG_INFO("usermanager::on_user_action_msg 坐桌结果 失败 机器人 m_my_userid = %d 归于空闲队列中等待 ", m_my_userid);
							addUserToIdleList(m_my_userid, 1);
						}
					}
					break;
				}				
				case DC_ASS_GR_USER_CUT:
				{
					m_user_action_mgr->user_cut(buf, len, code_id);
				}
					break;
				case DC_ASS_GR_USER_LEFT:
				{
					m_user_action_mgr->user_left(buf, len, code_id);
				}
					break;
				case DC_ASS_GR_REQUES_CHANGE_RESULT:
				{
					//请求换桌结果
					m_user_action_mgr->user_change_desk_result(buf, len, code_id);
				}
					break;
				default:
					break;
			}
		}
		return true;
	}

	bool usermanager::on_user_login_msg(unsigned int sub_id, char* buf, int len, unsigned int code_id)
	{
		switch (sub_id)
		{
		case SUB_USER_LOGIN_ROOM_ACK:
		{
			if (code_id == ERR_LOGIN_RM_SUCCESS)
			{
				DCGR::MSG_UserLoginRoomResult* LoginResult = reinterpret_cast<DCGR::MSG_UserLoginRoomResult*>(buf);
				if (LoginResult->userinfo.iChairID != 255)
				{
					//playing, recome
					BASIC_LOG_INFO("usermanager::on_user_action_msg  机器人 id = %d   返回登录房间结果  sussess  已经在游戏中,属于重连 请求游戏信息 ", LoginResult->userinfo.iUserid);
					//设定局数
					srand(time(0) + m_my_userid);
					int count = rand() % 4;
					set_game_play_count(count);
					m_user_action_mgr->send_get_game_info();
				}
				else if (NULL != LoginResult)
				{
					//new come
					BASIC_LOG_INFO("on_user_login_msg  handle=%d  返回登录房间结果 userid=%d  login success  ", this, m_my_userid);
					DCGR::MSG_UserLeftResult  sit;
					sit.dwUserID = m_my_userid;
					sit.dwCode = 1;
					BASIC_LOG_INFO("usermanager::on_user_action_msg  机器人 m_my_userid = %d 成功登录包间 登录成功的机器人的信息归于空闲列表中, m_socket = %d", m_my_userid, m_socket);
					::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)&sit);
					//to MFC --> realCheck 
					addUserToIdleList(m_my_userid, 1);
				}
			}
			else
			{
				BASIC_LOG_ERROR("on_user_login_msg  handle=%d  userid=%d  login fail  code_id ", this, m_my_userid, code_id);
			}
		}
		break;
		default:
			break;
		}
		return true;
	}

	void usermanager::send_msg(unsigned int sub_id, char* buf, int len)
	{
		if (NULL != m_user_action_mgr)
		{
			m_user_action_mgr->send_game_msg(sub_id, buf, len);
		}
	}

	void usermanager::set_my_station(unsigned char chair)
	{
		m_my_station = chair;
	}

	void usermanager::user_aready(bool ready)
	{
		if (NULL != m_user_action_mgr)
		{
			BASIC_LOG_INFO("user_aready  handle=%d  userid=%d  ", this, m_my_userid);
			m_user_action_mgr->send_agree_msg(ready);
		}
	}

	void usermanager::user_change_desk(unsigned int  chair_id)
	{
		if (NULL != m_user_action_mgr)
		{
			BASIC_LOG_INFO("user_change_desk  handle=%d  userid=%d  ", this, m_my_userid);
			m_user_action_mgr->user_change_desk(chair_id);
		}
	}

	void usermanager::set_timer(unsigned int event_id, unsigned int elapse)
	{
		SetTimer(m_hWnd, m_usermgr_index + MFC_TIME_START_ID * event_id, elapse, NULL);
	}

	void usermanager::kill_timer(unsigned int event_id)
	{
		KillTimer(m_hWnd, m_usermgr_index + MFC_TIME_START_ID * event_id);
	}

	void usermanager::on_user_timer(unsigned int event_id)
	{
		switch (event_id)
		{
		case TIME_USER_MGR_START:
		{

		}
		break;

		default:
			break;
		}
	}

	void usermanager::kill_all_timer()
	{
		kill_timer(TIME_USER_MGR_START);
	}

	void usermanager::on_user_gl_timer(unsigned int event_id, unsigned int user_id)
	{
		m_game_mgr->OnTimer(event_id, user_id);
	}

	void usermanager::set_swap_desk(bool swap)
	{
		m_swap_desk = swap;
	}

	void usermanager::set_game_play_count(int play_count)
	{
		m_play_game_count = play_count;
	}

	//处理MFC下线通知(机器人坐的桌子没真人)
	void usermanager::deal_notify_user_left_from_MFC(bool on_off)
	{
		if (m_socket > 0)
		{
			if (!on_off)
			{
				//下线通知
				if (m_user_statue == true)  //游戏中,下线不成功
				{
					//游戏中
					BASIC_LOG_INFO("usermanager::deal_notify_user_left_from_MFC 机器人（%d）下线通知, 游戏中 状态", m_my_userid);
				}
				else
				{
					if (!m_offline_flag)  //防止玩家多次向服务端发送离开消息
					{
						//不在游戏中，请求离开游戏
						BASIC_LOG_INFO("usermanager::deal_notify_user_left_from_MFC 机器人（%d）下线通知, 不在游戏中，请求离开游戏 ", m_my_userid);
						m_user_action_mgr->user_ask_left(m_my_userid);
						m_offline_flag = true;
					}
				}
			}
			else
			{
				m_offline_flag = false;
			}			
		}
		else
		{
			m_offline_flag = true;
		}
	}

	void usermanager::sendLeftInfoTOMFC()
	{
		m_user_statue = false;
		DCGR::MSG_UserLeftResult  sit;
		sit.dwCode = 1;
		sit.dwUserID = m_user_action_mgr->m_userid;
		::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_LEFT_RESULT, (LPARAM)&sit);
	}

	int usermanager::userSitDesk(idle_player_info deskinfo)
	{
		if (!deskinfo.socket_cnt)
		{
			return -1;
		}
		m_desk_index = deskinfo.sit.iDeskID;
		m_user_action_mgr->set_club_room_desk_no(deskinfo.sit.iClubDeskID);
		m_user_action_mgr->set_private_info(m_private_id, m_private_no, m_desk_index);
		m_user_action_mgr->set_roomid(m_room_id);
		m_user_action_mgr->send_sit_msg(0, 0);
		BASIC_LOG_INFO("usermanager::userSitDesk userid = %d [%d], m_desk_index = %d, m_socket = %d", deskinfo.sit.dwUserID, m_my_userid, m_desk_index, m_socket);

		return m_desk_index;
	}

	//把机器人归于空闲队列中
	void usermanager::addUserToIdleList(unsigned int muserid, unsigned int userstate)
	{
		DCGR::MSG_UserLoginRoomResult LoginResult;
		LoginResult.userinfo.iUserid = muserid;
		LoginResult.userinfo.iUserState = userstate;
		::SendMessage(m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_USER_INFO, (LPARAM)&LoginResult);
		BASIC_LOG_INFO("usermanager::addUserToIdleList 把机器人归于空闲队列中 muserid = %d, userstate = %d, ", muserid, userstate);
	}

	void usermanager::updateUserInfo(unsigned int userId)
	{
		DCGR::MSG_UserRequestUpdataInfo param;
		param.iUserID = userId;
		on_send_event(m_socket, DC_GR_USER_ACTION, DC_ASS_GR_UPDATA_USER_INFO, &param, sizeof(DCGR::MSG_UserRequestUpdataInfo), 0);
		BASIC_LOG_INFO("usermanager::updateUserInfo 通知更新玩家信息  DC_ASS_GR_UPDATA_USER_INFO userId = %d, m_my_userid = %d, ", userId, m_my_userid);
	}

	void usermanager::userReConnet()
	{
		int reinit = init_tcp(m_login_ip, m_login_port);
		int restart = start_tcp();
		BASIC_LOG_INFO(" usermanager::userReConnet() 返回结果 reinit = %d, restart = %d", reinit, restart);
	}

	//玩家是否可以开始
	bool usermanager::OncheckUserready()
	{
		int real_play_num = m_deskuserinfo.cur_desk_member - m_deskuserinfo.cur_robot_member;
		int robot_num = m_deskuserinfo.cur_robot_member;
		if (robot_num <= 1 && real_play_num == 0)
		{
			char dstr[256] = { 0 };
			sprintf_s(dstr, 256, "[Robot] OncheckUserready ==> iDeskID[%d] cur_desk_member[%d] cur_robot_member[%d] return false", m_deskuserinfo.iDeskID, m_deskuserinfo.cur_desk_member, m_deskuserinfo.cur_robot_member);
			OutputDebugStringA(dstr);
			return false;
		}

		/*if (m_deskuserinfo.cur_desk_member <= m_deskuserinfo.cur_robot_member) //没有真人
		{
			return false;
		}*/

		return true;
	}
}