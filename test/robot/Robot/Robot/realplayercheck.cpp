#include "realplayercheck.h"
#include "basiclog\basicloginterface.h"  

CheckC::CheckC()
{
	m_real_player_count = 0;
	m_real_check_time = 10;
	memset(m_desk_real_player, 0, sizeof(m_desk_real_player));
}

CheckC::~CheckC()
{

}

void CheckC::on_recv_event(unsigned int socket, char* buf, int len)
{
	utility::BasicCriticalSection lock(m_table_mutex);
	DCGR::NetMessageHead* header = reinterpret_cast<DCGR::NetMessageHead*>(buf);
	BASIC_LOG_INFO(" RealCheck::on_recv_event  handle=%d  bMainID=%d, socket=%d", this, header->bMainID, socket);
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
	m_real_heart_count = 0;
	switch (header->bMainID)
	{
		case 1:
		{
			if (header->bAssistantID == 3)
			{
				basicnet::SBasicConnectSuccess* connet = reinterpret_cast<basicnet::SBasicConnectSuccess*>(buf + sizeof(basicnet::_BasicCommandHeader));
				if (NULL != connet)
				{
					BASIC_LOG_INFO("RealCheck::on_recv_event  真人检测登录 处理 1 3 connet->m_time = %I64d, m_room_id = %d,m_real_check_time = %d", connet->m_time, m_club_room_id, m_real_check_time);
					DCGR::stuRealCheckLoginclub param;
					param.iclubroomid = m_club_room_id;
					param.handcode = 953468219;
					set_time(connet->m_time);
					on_send_event(m_socket, DC_GR_ROOM, DC_ASS_REAL_CHECKER_LOGIN, &param, sizeof(DCGR::stuRealCheckLoginclub), 0);

					set_timer(TIME_REAL_CHECK_STAER, 10000);
				}
				set_timer(TIME_REAL_CHECK_HEART_PAGE, 5000);
			}
			break;
		}
		case DC_GR_ROOM:
		{
			if (header->bAssistantID == DC_ASS_GET_CLUB_ROOM_COUNT_RES)
			{

			}
			else if (header->bAssistantID == DC_ASS_GET_CLUB_ROOM_DESKS_RES)
			{
				DCGR::stuAllDeskInfo param;
				utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
				szGetBuffer.write(buf_data, data_len);

				int re = gameservice::basichttphelper::parse_desks_info((char*)szGetBuffer.buffer(), 0, param);
				if (re == 0)
				{
					memcpy(&m_all_desk_info, &param, sizeof(DCGR::stuAllDeskInfo));
				}
			}
			else if (header->bAssistantID == DC_ASS_GET_ONE_CLUB_ROOM_DESKS_RES)
			{
				DCGR::RobotDeskInfo* param = reinterpret_cast<DCGR::RobotDeskInfo*>(buf_data);

				setuserdeskinfo(param->iDeskID, *param);

				bool hasdesk = false;
				for (int i = 0; i < MAX_DESK_NUM; i++)
				{
					if (m_all_desk_info.desk_info[i].iDeskID == param->iDeskID)
					{
						memcpy(m_all_desk_info.desk_info[i].chairid, param->chairid, sizeof(m_all_desk_info.desk_info[i].chairid));
						m_all_desk_info.desk_info[i].iClubRoomID = param->iClubRoomID;
						m_all_desk_info.desk_info[i].iClubDeskID = param->iClubDeskID;

						m_all_desk_info.desk_info[i].desk_count = param->desk_count;
						int local_realplayer = m_all_desk_info.desk_info[i].cur_desk_member - m_all_desk_info.desk_info[i].cur_robot_member;
						int remote_realplayer = param->cur_desk_member - param->cur_robot_member;
						if (local_realplayer < remote_realplayer)
						{
							m_all_desk_info.realPlyerCount++;
						}
						else if (local_realplayer > remote_realplayer)
						{
							m_all_desk_info.realPlyerCount--;
						}

						m_all_desk_info.desk_info[i].cur_desk_member = param->cur_desk_member;

						m_all_desk_info.desk_info[i].cur_robot_member = param->cur_robot_member;

						//更新机器人桌子信息
						if (m_desk_for_robot.find(i) != m_desk_for_robot.end())
						{
							m_desk_for_robot[i] = param->cur_robot_member;
						}

						char dstr[256] = { 0 };
						sprintf_s(dstr, 256, "[Robot] DC_ASS_GET_ONE_CLUB_ROOM_DESKS_RES ==> iDeskID[%d] cur_desk_member[%d] cur_robot_member[%d]", param->iDeskID, param->cur_desk_member, param->cur_robot_member);
						OutputDebugStringA(dstr);

						if (param->cur_desk_member == 0)
						{
							memset(&m_all_desk_info.desk_info[i], 0, sizeof(m_all_desk_info.desk_info[i]));
							//如果是机器人的桌子，则删除掉
							auto iter = m_desk_for_robot.find(i);
							if (iter != m_desk_for_robot.end())
							{
								char dstr[256] = { 0 };
								sprintf_s(dstr, 256, "[Robot] DC_ASS_GET_ONE_CLUB_ROOM_DESKS_RES ==> m_desk_for_robot[%d] erase", i);
								OutputDebugStringA(dstr);
								m_desk_for_robot.erase(iter);
							}
							continue;
						}

						hasdesk = true;
						break;
					}
				}
				if (hasdesk == false)
				{
					m_all_desk_info.desk_info[param->iDeskID].iDeskID = param->iDeskID;
					m_all_desk_info.desk_info[param->iDeskID].iClubRoomID = param->iClubRoomID;
					m_all_desk_info.desk_info[param->iDeskID].iClubDeskID = param->iClubDeskID;
					m_all_desk_info.desk_info[param->iDeskID].desk_count = param->desk_count;

					int local_realplayer = m_all_desk_info.desk_info[param->iDeskID].cur_desk_member - m_all_desk_info.desk_info[param->iDeskID].cur_robot_member;
					int remote_realplayer = param->cur_desk_member - param->cur_robot_member;
					if (local_realplayer < remote_realplayer)
					{
						m_all_desk_info.realPlyerCount++;
					}
					else if (local_realplayer > remote_realplayer)
					{
						m_all_desk_info.realPlyerCount--;
					}
					m_all_desk_info.desk_info[param->iDeskID].cur_desk_member = param->cur_desk_member;
					m_all_desk_info.desk_info[param->iDeskID].cur_robot_member = param->cur_robot_member;
				}
			}
			break;
		}
		default:
			break;
	}
}

void CheckC::on_close_event(unsigned int socket, char* buf, int len)
{
	BASIC_LOG_INFO(" RealCheck on_close_event  socket=%d  ", socket);
}

void CheckC::on_connect_event(unsigned int socket)
{
	BASIC_LOG_INFO(" RealCheck on_connect_event  socket=%d  ", socket);
}

void CheckC::set_hwnd(HWND  hWnd)
{
	m_hWnd = hWnd;
}

void CheckC::set_room_id(unsigned int club_room_id)
{
	m_club_room_id = club_room_id;
}

void CheckC::set_timer(unsigned int event_id, unsigned int elapse)
{
	SetTimer(m_hWnd, event_id*MFC_TIME_START_ID, elapse, NULL);
}

void CheckC::kill_timer(unsigned int event_id)
{
	KillTimer(m_hWnd, event_id*MFC_TIME_START_ID);
}

void CheckC::on_timer(unsigned int event_id)
{
	switch (event_id)
	{
	case TIME_REAL_CHECK_HEART_PAGE:
	{
		m_real_heart_count++;
		if (m_real_heart_count > 6)
		{
			BASIC_LOG_ERROR("RealCheck  真人检测类超过30s 没有接收到服务端消息 关闭真人检测，连接断开 ");
			kill_timer(TIME_REAL_CHECK_HEART_PAGE);
			close_client(m_socket);
			m_socket = 0;
		}
		else
		{
			on_send_event(m_socket, 1, 1, NULL, 0, 0);
		}
		break;
	}
	
	case TIME_REAL_CHECK_STAER:
	{
		BASIC_LOG_INFO("RealCheck::on_timer TIME_REAL_CHECK_STAER:  桌子检测，用于安排玩家坐桌 10s 匹配一次  m_club_room_id = %d", m_club_room_id);
		
		player_match_desk();
		set_timer(TIME_ROBBOT_CHECK_STA, 2000);
		
		break;
	}
	case TIME_ROBBOT_CHECK_STA:
	{
		kill_timer(TIME_ROBBOT_CHECK_STA);

		robot_match_desk();
		break;
	}
	break;
	default:
		break;
	}
}

/*
club_room_id : club_room_id
room_real_host : php_address
real_port : php_port
real_off : the num of real player when robot offline
deskrobot : 
*/
int CheckC::init(unsigned int club_room_id, char room_real_host[], int real_port, unsigned int real_off, unsigned int max_robot)
{
	m_max_robot = max_robot;
	set_room_id(club_room_id);

	//父类的方法
	init_tcp(room_real_host, real_port);
	start_tcp();

	return 1;
}

//发送玩家上线离线
void CheckC::sendToUserManger(bool on_off, unsigned int desk_index)
{
	for (int i = 0; i < MAX_USER_PLAY; i++)
	{
		if (m_pUserMgr[i] == NULL)
		{
			continue;
		}
		if (desk_index == m_pUserMgr[i]->m_desk_index)
		{
			m_pUserMgr[i]->deal_notify_user_left_from_MFC(on_off);
		}
	}
}

//设置桌子玩家信息
void CheckC::setuserdeskinfo(unsigned int desk_index, const DCGR::RobotDeskInfo& deskinfo)
{
	for (int i = 0; i < MAX_USER_PLAY; i++)
	{
		if (m_pUserMgr[i] == NULL)
		{
			continue;
		}
		if (desk_index == m_pUserMgr[i]->m_desk_index)
		{
			m_pUserMgr[i]->setdeskuserinfo(deskinfo);
		}
	}
}

//尾部插入元素
void CheckC::add_idle_player(idle_player_info ipi)
{
	m_idle_player_list.push_back(ipi);
}

//获取第一个元素，并且删除
bool CheckC::get_first_idle_player(idle_player_info &ipi)
{
	if (m_idle_player_list.size() == 0)
	{
		BASIC_LOG_INFO("RealCheck::get_first_idle_player 没有机器人");
		return false;
	}
	
	auto it = m_idle_player_list.begin();
	ipi = *it;
	m_idle_player_list.erase(it);
	return true;
}

//检测是否匹配真实玩家
void CheckC::player_match_desk()
{
	BASIC_LOG_INFO("RealCheck::player_match_desk 总真人数 = %d", m_all_desk_info.realPlyerCount);
	
	if (m_all_desk_info.realPlyerCount >= m_rabot_off_player_count)
	{
		BASIC_LOG_INFO("RealCheck::player_match_desk 真实玩家总在线数量已到达，不再配对机器人	m_all_desk_info.realPlyerCount=%d	m_rabot_off_player_count=%d", m_all_desk_info.realPlyerCount, m_rabot_off_player_count);
		return;
	}

	for (int i = 0; i < MAX_DESK_NUM; i++)
	{
		if (m_all_desk_info.desk_info[i].cur_desk_member <= m_all_desk_info.desk_info[i].cur_robot_member)
		{
			//当前桌子没有真人，不坐
			continue;
		}
		if (m_all_desk_info.desk_info[i].desk_count == m_all_desk_info.desk_info[i].cur_desk_member)
		{
			//当前桌子已经坐满
			continue;
		}
		if (m_all_desk_info.desk_info[i].cur_robot_member >= m_max_robot)
		{
			//当前桌子的机器人数量已经足够
			continue;
		}
		if (m_all_desk_info.desk_info[i].cur_desk_member >= (m_desk_real_player_no_robat + m_all_desk_info.desk_info[i].cur_robot_member))
		{
			//当前桌子的真实玩家数量已经足够
			continue;
		}
		idle_player_info ipi;
		if (get_first_idle_player(ipi))
		{
			BASIC_LOG_INFO("RealCheck::player_match_desk 机器人匹配坐桌 桌子id=%d		ipi.sit.dwUserID=%d", i, ipi.sit.dwUserID);
			for (unsigned int j = 0; j < m_all_desk_info.desk_info[i].desk_count; j++)
			{
				if (m_all_desk_info.desk_info[i].chairid[j] == 0)
				{
					ipi.sit.iChairID = j;
					break;
				}
			}
			ipi.sit.iClubDeskID = m_all_desk_info.desk_info[i].iClubDeskID;
			ipi.sit.iClubRoomID = m_all_desk_info.desk_info[i].iClubRoomID;
			ipi.sit.iDeskID = m_all_desk_info.desk_info[i].iDeskID;
			for (int k = 0; k < MAX_USER_PLAY; k++)
			{
				if (m_pUserMgr[k] == NULL)
				{
					continue;
				}
				if (m_pUserMgr[k]->m_my_userid == ipi.sit.dwUserID)
				{
					BASIC_LOG_INFO("RealCheck::player_match_desk 通知玩家 ipi.sit.dwUserID = %d 坐桌", ipi.sit.dwUserID);
					m_pUserMgr[k]->m_offline_flag = false;
					m_pUserMgr[k]->userSitDesk(ipi);

					char dstr[256] = { 0 };
					sprintf_s(dstr, 256, "[Robot] RealCheck ==>robot[%d] userSitDesk[%d]", m_pUserMgr[k]->m_my_userid, ipi.sit.iDeskID);
					OutputDebugStringA(dstr);
					break;
				}
			}
		}
		else
		{
			BASIC_LOG_INFO("RealCheck::player_match_desk 米有空闲的机器人");
			return;
		}
	}

	return;
}

void CheckC::robot_match_desk()
{
	BASIC_LOG_INFO("RealCheck::robot_match_desk 总真人数 = %d", m_all_desk_info.realPlyerCount);

	if (m_all_desk_info.realPlyerCount >= m_rabot_off_player_count)
	{
		BASIC_LOG_INFO("RealCheck::robot_match_desk 真实玩家总在线数量已到达，不再配对机器人	m_all_desk_info.realPlyerCount=%d	m_rabot_off_player_count=%d", m_all_desk_info.realPlyerCount, m_rabot_off_player_count);
		return;
	}

	//再次检测并更新机器人桌子
	for (auto it = m_desk_for_robot.begin(); it != m_desk_for_robot.end(); )
	{
		int desk_index = it->first;
		if (m_all_desk_info.desk_info[desk_index].cur_desk_member == 0)
		{
			char dstr[256] = { 0 };
			sprintf_s(dstr, 256, "[Robot] robot_match_desk ==> erase m_desk_for_robot[%d][%d]", desk_index, it->second);
			OutputDebugStringA(dstr);

			m_desk_for_robot.erase(it++);
		}
		else
		{
			it->second = m_all_desk_info.desk_info[desk_index].cur_robot_member;
			it++;
		}
	}

	for (auto it = m_desk_for_robot.begin(); it != m_desk_for_robot.end();it++)
	{
		char dstr[256] = { 0 };
		sprintf_s(dstr, 256, "[Robot] robot_match_desk ==> before m_desk_for_robot[%d] = [%d]", it->first, it->second);
		OutputDebugStringA(dstr);
	}

	if (m_desk_for_robot.size() < m_desk_for_robot_count)
	{
		//可以创建机器人桌子
		idle_player_info ipi;
		if (get_first_idle_player(ipi))
		{
			for (int robot_index = 0; robot_index < MAX_USER_PLAY; robot_index++)
			{
				if (m_pUserMgr[robot_index] == NULL) continue;
				if (m_pUserMgr[robot_index]->m_my_userid == ipi.sit.dwUserID)
				{
					m_pUserMgr[robot_index]->m_offline_flag = false;

					std::string output_str;
					DCGR::MSG_UserSit sit_param;
					m_pUserMgr[robot_index]->m_user_action_mgr->restart_get_desk_info(sit_param);

					OutputDebugStringA("[Robot] creating robot desk...");

					if (m_all_desk_info.desk_info[sit_param.iDeskID].cur_desk_member != 0)
					{
						m_pUserMgr[robot_index]->set_swap_desk(true);
						m_pUserMgr[robot_index]->m_user_action_mgr->set_club_room_desk_no(sit_param.iDeskID);
						m_pUserMgr[robot_index]->m_user_action_mgr->restart_get_desk_info(sit_param);

						OutputDebugStringA("[Robot] creating robot fisrt not empty...");
					}

					//判断机器人创建的桌子是否为空，不为空的话，要重新创建桌子
					if (m_all_desk_info.desk_info[sit_param.iDeskID].cur_desk_member == 0)
					{
						ipi.sit.iClubDeskID = sit_param.iClubDeskID;
						ipi.sit.iClubRoomID = sit_param.iClubRoomID;
						ipi.sit.iDeskID = sit_param.iDeskID;
						ipi.sit.iChairID = sit_param.iChairID;
						std::pair<int, int> pTmp;
						pTmp.first = m_pUserMgr[robot_index]->userSitDesk(ipi);
						pTmp.second = 1;
						m_desk_for_robot.insert(pTmp);
						char dstr[256] = { 0 };
						sprintf_s(dstr, 256, "[Robot] robot_match_desk ==> create [%d] == [%d]", pTmp.first, sit_param.iDeskID);
						OutputDebugStringA(dstr);

						Sleep(200);
						break;
					}
					else
					{
						OutputDebugStringA("[Robot] creating robot second not empty...");

						add_idle_player(ipi);
						break;
					}
				}
			}
		}
	}

	while (!m_idle_player_list.empty())
	{
		OutputDebugStringA("[Robot] m_idle_player_list Loop");
		int NotSitNum = 0;
		//不可创建机器人桌子，检查机器人是否可以坐桌
		for (auto desk_index : m_desk_for_robot)
		{
			if (m_all_desk_info.desk_info[desk_index.first].desk_count == m_all_desk_info.desk_info[desk_index.first].cur_desk_member ||
				m_all_desk_info.desk_info[desk_index.first].cur_robot_member >= m_max_robot || 
				m_all_desk_info.desk_info[desk_index.first].cur_desk_member - m_all_desk_info.desk_info[desk_index.first].cur_robot_member >= m_desk_real_player_no_robat	//当前桌子的真实玩家数量已经足够
				)
			{
				//限制机器人坐下
				char dstr[256] = { 0 };
				sprintf_s(dstr, 256, "[Robot] robot_match_desk ==> desk full m_desk_for_robot[%d] = [%d]", desk_index.first, desk_index.second);
				OutputDebugStringA(dstr);
				NotSitNum++;
				continue;
			}
			else
			{
				//该桌子可以坐机器人
				idle_player_info ipi;
				if (get_first_idle_player(ipi))
				{
					ipi.sit.iClubDeskID = m_all_desk_info.desk_info[desk_index.first].iClubDeskID;
					ipi.sit.iClubRoomID = m_all_desk_info.desk_info[desk_index.first].iClubRoomID;
					ipi.sit.iDeskID = m_all_desk_info.desk_info[desk_index.first].iDeskID;
					ipi.sit.iChairID = 255;
					for (int robot_index = 0; robot_index < MAX_USER_PLAY; robot_index++)
					{
						if (m_pUserMgr[robot_index] == NULL) continue;
						if (m_pUserMgr[robot_index]->m_my_userid == ipi.sit.dwUserID)
						{
							m_pUserMgr[robot_index]->m_offline_flag = false;
							int tmp = m_pUserMgr[robot_index]->userSitDesk(ipi);
							m_desk_for_robot[tmp]++;
							
							Sleep(200);
							
							char dstr[256] = { 0 };
							sprintf_s(dstr, 256, "[Robot] robot_match_desk ==> desk add ipi.sit.iDeskID[%d] desk_index[%d] m_pUserMgr[robot_index]->userSitDesk(ipi)[%d]", ipi.sit.iDeskID, desk_index.first, tmp);
							OutputDebugStringA(dstr);

							break;
						}
					}
				}
				else
				{
					continue;
				}
			}
		}
		if (NotSitNum == m_desk_for_robot.size())
		{
			break;
		}
	}

	for (auto it = m_desk_for_robot.begin(); it != m_desk_for_robot.end();it++)
	{
		char dstr[256] = { 0 };
		sprintf_s(dstr, 256, "[Robot] robot_match_desk after ==> m_desk_for_robot[%d] = [%d]", it->first, it->second);
		OutputDebugStringA(dstr);
	}

	return;
}