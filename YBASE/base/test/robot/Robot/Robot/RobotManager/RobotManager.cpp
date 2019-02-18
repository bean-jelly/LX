#include <atlstr.h>
#include <sstream>
#include <string>
#include "RobotManager.h"
#include "datastruct.h"
#include "basictable/GameRoomMsg.h"
#include "basictcp/basicnetdef.h"
#include "basiclog/basicloginterface.h"

namespace RobotManager
{
	RobotManager::RobotManager()
	{
		m_heart_break_count = 0;				//心跳包超时自动关闭计数
		m_login_port		= 0;
		m_club_room_id		= 0;;
		m_club_rule_id		= 0;
		m_game_id			= 0;
		m_club_room_desk_no	= 0;
		m_room_id			= 0;
		m_recharge_limit	= 0;
		m_recharge_money	= 0;
		m_windos_handle		= NULL;
		m_robot_user_id		= 0;                //机器人真实ID
		m_robot_chair_id	= 0;                //机器人桌子位置
		m_desk_index		= 0;                //桌子索引
		m_play_game_count	= 0;				//多少局后换桌
		m_play_game_count_min	= 0;			//最大局数换桌
		m_play_game_count_max	= 0;			//最少局数换桌			
		m_robot_state			= false;		//玩家状态 fasle 非游戏状态，true 游戏状态
		m_robot_manage_index	= -1;
	}

	RobotManager::~RobotManager()
	{

	}

	void RobotManager::init_data(UserInitData* param)
	{
		UserInitData* pParam = reinterpret_cast<UserInitData*>(param);
		if (NULL == pParam)
		{
			return;
		}
		m_robot_user_id = pParam->user_id;

	}
}