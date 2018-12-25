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
		m_heart_break_count = 0;				//��������ʱ�Զ��رռ���
		m_login_port		= 0;
		m_club_room_id		= 0;;
		m_club_rule_id		= 0;
		m_game_id			= 0;
		m_club_room_desk_no	= 0;
		m_room_id			= 0;
		m_recharge_limit	= 0;
		m_recharge_money	= 0;
		m_windos_handle		= NULL;
		m_robot_user_id		= 0;                //��������ʵID
		m_robot_chair_id	= 0;                //����������λ��
		m_desk_index		= 0;                //��������
		m_play_game_count	= 0;				//���پֺ���
		m_play_game_count_min	= 0;			//����������
		m_play_game_count_max	= 0;			//���پ�������			
		m_robot_state			= false;		//���״̬ fasle ����Ϸ״̬��true ��Ϸ״̬
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