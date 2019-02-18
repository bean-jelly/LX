#include "stdafx.h"
#include "usertask.h" 
usertask::usertask()
{ 
}

usertask::~usertask()
{

}

void usertask::add_user_mgr(userserver::usermanager* mgr)
{
	m_user_mgr = mgr;
}

void usertask::add_user_initdata(char* param)
{
	UserInitData*  pParam = reinterpret_cast<UserInitData*>(param);
	if (NULL != pParam)
	{
		m_UserInitData.clear();
		m_UserInitData.user_id = pParam->user_id;
		m_UserInitData.room_id = pParam->room_id;
		m_UserInitData.index = pParam->index;
		m_UserInitData.private_id = pParam->private_id;
		m_UserInitData.private_no = pParam->private_no;
		m_UserInitData.login_port = pParam->login_port;
		m_UserInitData.real_room_id = pParam->real_room_id;
		m_UserInitData.basic_point = pParam->basic_point;
		m_UserInitData.play_game_count = pParam->play_game_count;
		m_UserInitData.play_game_count_min = pParam->play_game_count_min;
		m_UserInitData.play_game_count_max = pParam->play_game_count_max;
		m_UserInitData.player_game_id = pParam->player_game_id;
		m_UserInitData.club_desk_club_room_desk_no = pParam->club_desk_club_room_desk_no;
		m_UserInitData.recharge_limit = pParam->recharge_limit;
		m_UserInitData.recharge_money = pParam->recharge_money;
		m_UserInitData.rule_id = pParam->rule_id;
		m_UserInitData.handle = pParam->handle;
		m_UserInitData.usermgr_index = pParam->usermgr_index;
		m_UserInitData.login_time = pParam->login_time;
		m_UserInitData.play_time = pParam->play_time;
		m_UserInitData.robot_off_player_count = pParam->robot_off_player_count;

		memcpy(m_UserInitData.login_ip, pParam->login_ip, sizeof(m_UserInitData.login_ip));
		memcpy(m_UserInitData.user_password, pParam->user_password, sizeof(m_UserInitData.user_password));
		memcpy(m_UserInitData.player_token, pParam->player_token, sizeof(m_UserInitData.player_token));
		memcpy(m_UserInitData.php_address, pParam->php_address, sizeof(m_UserInitData.php_address));
		memcpy(m_UserInitData.game_name, pParam->game_name, sizeof(m_UserInitData.game_name));
	}
}
      
void usertask::add_agree(bool bagree)
{
	m_agree = bagree;
}

void usertask::add_free(bool bfree)
{
	m_free = bfree;
}

void usertask::add_type(unsigned int tasktype)
{
	m_taks_type = tasktype;
}

void usertask::DoThreadJob()
{
	if (  NULL != m_user_mgr)
	{ 
		if (m_taks_type == 0)
		{
			//登录包间 
			m_user_mgr->init((char*)&m_UserInitData);
		}
		else if (m_taks_type == 2)
		{
			//发送同意消息 
			m_user_mgr->user_aready(m_free);
		}
	}
}

void usertask::OnFinal()
{  
	delete this;
}

void usertask::set_user_mgr_index(unsigned int index)
{
	m_user_mgr_index = index;
}