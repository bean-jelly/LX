#ifndef __ROBOTACTION_H_
#define __ROBOTACTION_H_
#include "..\datastruct.h"
#include "utility/BasicType.h"
#include "..\BasicHttpContext.h" 
#include "json\json.h" 

#include "basiccode\utility\BasicMutex.h"
#include "basiccode\utility\BasicCriticalSection.h"
#include "basiccode\utility\BasicThreadPool.h"
#include "basiccode\utility\BasicBuffer.h"

#include "../tcp/TcpConManager.h"

namespace RobotManager
{
	class RobotAction : public TcpConManager
	{
	public:
		//发送登录信息
		int		send_login_msg(DCGR::MSG_UserLoginRoom &msg);
		//发送坐桌消息
		int		send_sit_msg(DCGR::MSG_UserSit &param);
		//
		int		send_agree_msg(DCGR::MSG_UserAgree &param);
		//
		int		send_get_game_info(DCGR::StuRequestGameInfor &param);
		//
		int		user_change_desk(DCGR::MSG_UserRequestChangeDesk &param);
		//
		int		user_ask_left(DCGR::MSG_UserLeft &param);
	public:
		int		send_game_msg(unsigned int sub_id, char* buf, unsigned int len);
	public:
		virtual int	deal_user_sit(DCGR::MSG_UserSitResult &param, unsigned int code_id) = 0;
	public:
		unsigned int	m_socket;
	};
}

#endif