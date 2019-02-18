#include "RobotAction.h"
#include "basiclog/basicloginterface.h"

//发送登录信息
int	RobotManager::RobotAction::send_login_msg(DCGR::MSG_UserLoginRoom &param)
{
	return on_send_event(MID_USER_LOGIN_ROOM, SUB_USER_LOGIN_ROOM_ASK, &param, sizeof(DCGR::MSG_UserLoginRoom), 0);
}

//发送坐桌消息
int	 RobotManager::RobotAction::send_sit_msg(DCGR::MSG_UserSit &param)
{
	return on_send_event(DC_GR_USER_ACTION, DC_ASS_GR_USER_SIT, &param, sizeof(DCGR::MSG_UserSit), 0);
}

//
int	RobotManager::RobotAction::send_agree_msg(DCGR::MSG_UserAgree &param)
{
	return on_send_event(DC_GR_GAMM_NOTIFY, DC_ASS_GR_USER_READY, &param, sizeof(DCGR::MSG_UserAgree), 0);
}

//
int	RobotManager::RobotAction::send_get_game_info(DCGR::StuRequestGameInfor &param)
{
	return on_send_event(DC_GR_GAMM_NOTIFY, DC_ASS_GR_GAMEINFOR, &param, sizeof(DCGR::StuRequestGameInfor), 0);
}

//
int	RobotManager::RobotAction::user_change_desk(DCGR::MSG_UserRequestChangeDesk &param)
{
	return on_send_event(DC_GR_USER_ACTION, DC_ASS_GR_REQUES_CHANGE, &param, sizeof(DCGR::MSG_UserRequestChangeDesk), 0);
}

//
int	RobotManager::RobotAction::user_ask_left(DCGR::MSG_UserLeft &param)
{
	return on_send_event(DC_GR_USER_ACTION, DC_ASS_GR_USER_LEFT, &param, sizeof(DCGR::MSG_UserLeft), 0);
}

//
int	RobotManager::RobotAction::send_game_msg(unsigned int sub_id, char* buf, unsigned int len)
{
	return on_send_event(DC_GR_GAMM_NOTIFY, sub_id, buf, len, 0);
}