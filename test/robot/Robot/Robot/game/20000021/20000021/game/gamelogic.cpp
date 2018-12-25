#include "gamelogic.h"
#include "../user/usermanager.h" 
#include "basiclog\basicloginterface.h" 
#include "gamemsg.h"

gamelogic::gamelogic()
{
	BASIC_LOG_INIT("dachuan/log/20000021_AI_logic.log");
	m_game_base.gameconfig.InitConfig();
	memset(m_UserCard, 0, sizeof(m_UserCard));
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));
	memset(m_SelectCard, 0, sizeof(m_SelectCard));
	m_byCurrentOut = 255;

	memset(m_DeskCard, 0, sizeof(m_DeskCard));  //桌面上的牌
	m_DeskCount = 0x00;							//桌面上的牌数据
	m_DeskPositon = 0x00;						//桌面上的牌玩家
	m_BigOutUser = 0x00;
	memset(m_SelectCard, 0, sizeof(m_SelectCard));//换三张选择的牌	
	m_byCurrentOut = 0x00;						//当前出牌玩家
	m_byNTpos = 0x00;							//庄家位置
	m_curCallScore = 0;							//当前叫分分值
	game_count = 0;								//游戏局数
	m_rabot_state = 0;							//机器人状态 0 空闲 ，1 游戏中
	m_user_mgr = NULL;
	m_user_index = 255;
}


gamelogic::~gamelogic()
{
}

bool gamelogic::gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{

	BASIC_LOG_INFO("gameframe  m_user_mgr=%0x userid=%d   code_id=%d   ", m_user_mgr, sub_id, code_id);

	return true;
}

bool gamelogic::gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{
	utility::BasicCriticalSection lock(m_game_mutex);
	switch (sub_id)
	{
	case DC_ASS_GR_GAMEINFOR:
	{
		GSBaseInfo*	param = reinterpret_cast<GSBaseInfo*>(buf);
		if (param != NULL && sizeof(GSBaseInfo) == len)
		{
			m_game_base = *param;
			BASIC_LOG_INFO("gamenotify gamerule bTakePair(%d) byplaymode(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d), m_user_mgr=%0x,sub_id=%d,code_id=%d"
				, m_game_base.gameconfig.bTakePair, m_game_base.gameconfig.byplaymode[0], m_game_base.gameconfig.byplaymode[1]
				, m_game_base.gameconfig.byplaymode[2], m_game_base.gameconfig.byplaymode[3], m_game_base.gameconfig.byplaymode[4]
				, m_game_base.gameconfig.byplaymode[5], m_game_base.gameconfig.byplaymode[6], m_game_base.gameconfig.byplaymode[7]
				, m_game_base.gameconfig.byplaymode[8], m_game_base.gameconfig.byplaymode[9], m_user_mgr, sub_id, code_id);
		}
	}
		break;
	case GAME_MSG_NOTIFY_INFO:
	{
		BASIC_LOG_INFO("gamenotify deal game notify msg, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		deal_game_notify_interface(buf, len);
	}
		break;
	case GAME_MSG_SEND_ALL_CARD:
	{
		BASIC_LOG_INFO("gamenotify deal_send_hand_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_send_hand_card(buf, len);
	}
		break;
	case GAME_MSG_SEND_CARD_TO_ROBOT:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_all_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_all_card(buf, len);
	}
		break;
	case GAME_MSG_NEW_TURN:
	{
		BASIC_LOG_INFO("gamenotify deal_new_turn, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_new_turn(buf, len);
	}
		break;
	case GAME_MSG_CHANGE_CARD:
	{
		BASIC_LOG_INFO("gamenotify deal_change_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_change_card(buf, len);
	}
		break;
	case GAME_MSG_BACK_CARD_EX:
	{
		BASIC_LOG_INFO("gamenotify deal_get_back_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_get_back_card(buf, len);
	}
		break;
	case GAME_MSG_DEAL_RES:
	{
		BASIC_LOG_INFO("gamenotify deal_res_interface, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		deal_res_interface(buf, len);
	}
		break;
	case GAME_MSG_OUT_CARD_RESULT:
	{
		BASIC_LOG_INFO("gamenotify deal_out_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_out_card(buf, len);
	}
		break;
	case GAME_MSG_SET_GAEM_END:
	{
		BASIC_LOG_INFO("gamenotify deal_game_end, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_game_end(buf, len);
	}
		break;
	case DC_ASS_GR_GAME_FINISH:
	{
		BASIC_LOG_INFO("gamenotify deal_free_game, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		game_count = 0;
		//deal_free_game(buf, len);
	}
		break;
	case DC_ASS_GR_USER_READY:
	{//金币不足消息
		DCGR::MSG_UserAgreeResult* param = reinterpret_cast<DCGR::MSG_UserAgreeResult*>(buf);
		if (param->bSuccess == false && param->iErrCode == agree_err_type_2)
		{
			//金币不足，无法准备
			gold_change gc;
			gc.player_id = param->dwUserID;
			gc.gold = -1;
			BASIC_LOG_INFO("金币不足，无法准备 param->dwUserID = %d", param->dwUserID);
			::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_LACK, (LPARAM)&gc);
		}
		if (param->bSuccess == true && param->iErrCode == 0 && param->dwUserID == m_user_mgr->m_my_userid)
		{//准备成功，请求游戏规则
			m_user_mgr->m_user_action_mgr->send_get_game_info();
		}
	}
		break;
	default:
	{
		BASIC_LOG_ERROR("gamenotify error, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
	}
		break;
	}

	return true;
}

//发牌消息
void  gamelogic::notifymsg_send_hand_card(char* buf, int len)
{
	SendAllStruct* param = reinterpret_cast<SendAllStruct*>(buf);
	if (param != NULL)
	{
		if (m_user_mgr->m_my_station < PLAY_COUNT)
		{
			memcpy(m_UserCard[m_user_mgr->m_my_station], param->byCardList, sizeof(param->byCardList));
			memcpy(m_UserCardCount, param->byCardCount, sizeof(m_UserCardCount));

			BASIC_LOG_INFO(" robot  userid(%d) my_station(%d)  len(%d)", m_user_mgr->m_my_userid, m_user_mgr->m_my_station, m_UserCardCount[m_user_mgr->m_my_station]);
		}
	}

}
void  gamelogic::notifymsg_all_card(char* buf, int len)
{
	SendAllCardInfoStruct* param = reinterpret_cast<SendAllCardInfoStruct*>(buf);
	if (param != NULL)
	{
		if (m_user_mgr->m_my_station < PLAY_COUNT)
		{
			memcpy(m_UserCard[(m_user_mgr->m_my_station + PLAY_COUNT - 1) % PLAY_COUNT], param->byPreCardCount, HAND_CARD_MAX * sizeof(BYTE));
			memcpy(m_UserCard[(m_user_mgr->m_my_station + 1) % PLAY_COUNT], param->byPostCardCount, HAND_CARD_MAX * sizeof(BYTE));
			memcpy(m_Back_card, param->byBackCard, BACK_CARD_COUNT * sizeof(BYTE));

			BASIC_LOG_INFO("Get All Card Info!");
		}
	}
}

//新一轮开始
void  gamelogic::notifymsg_new_turn(char* buf, int len)
{
	NewTurnStruct* param = reinterpret_cast<NewTurnStruct*>(buf);
	if (param != NULL)
	{
		m_BigOutUser = param->bDeskStation;
		m_byCurrentOut = param->bDeskStation;
		memset(m_DeskCard, 0, sizeof(m_DeskCard));
		m_DeskCount = 0;
		BYTE temp[PLAY_COUNT] = { 0 };
		deal_out_card(0, m_byCurrentOut, temp);
	}
}
//换三张
void  gamelogic::notifymsg_change_card(char* buf, int len)
{
	ChangeCardStruct* param = reinterpret_cast<ChangeCardStruct*>(buf);
	if (param != NULL)
	{
		if (param->byDeskPos == m_user_mgr->m_my_station)
		{
			m_GameLogic.RemoveCard(param->byChooseCardList, 3, m_UserCard[m_user_mgr->m_my_station], m_UserCardCount[m_user_mgr->m_my_station]);
			int pos = m_UserCardCount[m_user_mgr->m_my_station] - 3;
			memcpy(&m_UserCard[m_user_mgr->m_my_station][pos], param->byGetCardList, (3 * sizeof(BYTE)));
			BASIC_LOG_INFO("robot id(%d) pos(%d) change card!, remove card(%02x,%02x,%02x),get card (%02x,%02x,%02x)",
				m_user_mgr->m_my_userid, m_user_mgr->m_my_station, param->byChooseCardList[0], param->byChooseCardList[1], param->byChooseCardList[2],
				param->byGetCardList[0], param->byGetCardList[1], param->byGetCardList[2]);
		}
	}
}
//获取底牌
void  gamelogic::notifymsg_get_back_card(char* buf, int len)
{
	BackCardExStruct* param = reinterpret_cast<BackCardExStruct*>(buf);
	if (param != NULL)
	{
		m_BigOutUser = param->iGiveBackPeople;
		m_byCurrentOut = param->iGiveBackPeople;
		m_byNTpos = param->iGiveBackPeople;

		if (m_UserCardCount[m_byCurrentOut] == 17)
		{
			memcpy(&m_UserCard[m_byCurrentOut][(m_UserCardCount[m_byCurrentOut])], param->iBackCard, 3);
			m_UserCardCount[m_byCurrentOut] = m_UserCardCount[m_byCurrentOut] + 3;

			BASIC_LOG_INFO("back_card isself(%d) pos(%d) ,card(%02x,%02x,%02x)", m_user_mgr->m_my_userid == param->iGiveBackPeople, m_user_mgr->m_my_station,
				param->iBackCard[0], param->iBackCard[1], param->iBackCard[2]);
		}		
	}
}
//出牌消息
void  gamelogic::notifymsg_out_card(char* buf, int len)
{
	OutCardMsg* param = reinterpret_cast<OutCardMsg*>(buf);
	if (param != NULL)
	{
		if (param->byCardCount > 0)
		{
			BYTE out_type = m_GameLogic.GetCardShape(param->iCardList, param->byCardCount);
			BYTE first = 0x00;
			if (m_BigOutUser == m_byCurrentOut)
			{//先手
				first = 0x01;
			}
			else
			{//后手
				first = 0x02;
			}
			m_User_Operate[param->bDeskStation].AddOutCardInfo(param->iCardList, param->byCardCount, out_type, first);

			m_BigOutUser = param->bDeskStation;
			memset(m_DeskCard, 0, sizeof(m_DeskCard));
			m_DeskCount = 0;
			memcpy(m_DeskCard, param->iCardList, sizeof(m_DeskCard));
			m_DeskCount = param->byCardCount;
			m_DeskPositon = param->bDeskStation;
			
			//删除出掉的牌
			m_GameLogic.RemoveCard(param->iCardList, m_DeskCount, m_UserCard[m_DeskPositon], m_UserCardCount[m_DeskPositon]);
			m_UserCardCount[m_DeskPositon] -= param->byCardCount;

			BASIC_LOG_INFO("deal_out_card outcardlist(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x), byCardCount(%d)"
				, param->iCardList[0], param->iCardList[1], param->iCardList[2], param->iCardList[3], param->iCardList[4], param->iCardList[5]
				, param->iCardList[6], param->iCardList[7], param->iCardList[8], param->iCardList[9], param->iCardList[10], param->iCardList[11]
				, param->iCardList[12], param->iCardList[13], param->iCardList[14], param->iCardList[15], param->iCardList[16], param->iCardList[17]
				, param->iCardList[18], param->iCardList[19], param->byCardCount);
		}
		else
		{
			BYTE pass_type = m_GameLogic.GetCardShape(m_DeskCard, m_DeskCount);
			BYTE byfeiend = 0x00;//0队友  1 对手
			if (param->bDeskStation == m_byNTpos)
			{//自己是庄家并且不出牌
				byfeiend = 0x01;
			}
			if (param->bDeskStation != m_byNTpos && m_byNTpos == m_BigOutUser)
			{//自己不是庄家并且最大者是庄家,且自己不出牌
				byfeiend = 0x01;
			}

			m_User_Operate[param->bDeskStation].AddPassCardInfo(param->iCardList, param->byCardCount, pass_type, byfeiend);
		}
	}
}
//结算消息
void  gamelogic::notifymsg_game_end(char* buf, int len)
{
	m_rabot_state = 0;
	long long current_time = time(0);
	game_count++;
	
	notify_game_result*	param = reinterpret_cast<notify_game_result*>(buf);

	int pos = m_user_mgr->m_my_station;

	BASIC_LOG_INFO("deal_game_end  user_id(%d), remancardlist(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x), byCardCount(%d)",m_user_mgr->m_my_userid
		, param->iUserCard[pos][0], param->iUserCard[pos][1], param->iUserCard[pos][2], param->iUserCard[pos][3], param->iUserCard[pos][4], param->iUserCard[pos][5]
		, param->iUserCard[pos][6], param->iUserCard[pos][7], param->iUserCard[pos][8], param->iUserCard[pos][9], param->iUserCard[pos][10], param->iUserCard[pos][11]
		, param->iUserCard[pos][12], param->iUserCard[pos][13], param->iUserCard[pos][14], param->iUserCard[pos][15], param->iUserCard[pos][16], param->iUserCard[pos][17]
		, param->iUserCard[pos][18], param->iUserCard[pos][19], param->iUserCardCount[pos]);

	BASIC_LOG_INFO("deal_game_end  user_id(%d), game_count(%d)，玩家随机局数(%d) ,current_time(%lld) ,mgr.m_login_time(%lld), act.m_login_time(%lld),m_play_time(%d)",
		m_user_mgr->m_my_userid, game_count, m_user_mgr->m_play_game_count, current_time, m_user_mgr->m_login_time, m_user_mgr->m_user_action_mgr->m_login_time,
		m_user_mgr->m_user_action_mgr->m_play_time);
	/***********************************************************************************/
	gold_change gc;
	gc.player_id = m_user_mgr->m_my_userid;
	gc.gold = param->iTurePoint[m_user_mgr->m_my_station] - m_game_base.tax;
	::SendMessage(m_user_mgr->m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_GOLD_CHANGE, (LPARAM)&gc);

	if (game_count >= m_user_mgr->m_play_game_count)
	{
		BASIC_LOG_INFO("deal_game_end user_left");
		game_count = 0;
		m_user_mgr->deal_notify_user_left_from_MFC(false);
	}
	else
	{
		BASIC_LOG_INFO("deal_game_end user ready");
		set_timer(TIME_USER_SEND_READY, 5000);
	}
	//清除数据
	for (UINT i = 0; i < PLAY_COUNT; i++)
	{
		m_User_Operate[i].ClearAllData();
	}
	memset(m_UserCard, 0, sizeof(m_UserCard));
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));
	memset(m_SelectCard, 0, sizeof(m_SelectCard));
	m_byCurrentOut = 255;
	m_GameLogic.InitAllData();
	m_curCallScore = 0;
}
//房间解散
void  gamelogic::notifymsg_free_game(char* buf, int len)
{
	DCGR::MSG_NotifyGameFinish* param = reinterpret_cast<DCGR::MSG_NotifyGameFinish*>(buf);
	Sleep(2000);
	m_user_mgr->user_change_desk(m_user_mgr->m_my_userid);
	if (sizeof(DCGR::MSG_NotifyGameFinish) == len && param != NULL)
	{
		
	}
}

//玩家操作消息
void  gamelogic::deal_res_interface(char* buf, int len)
{
	DealUserOperRes*	param = reinterpret_cast<DealUserOperRes*>(buf);
	if (param != NULL)
	{
		BASIC_LOG_INFO("RES_INTERFASE REV MSG byFlag(%d)-byChairID(%d)-byChairIDEx(%d), byFlagEx(%d)",
			param->byFlag, param->byChairIDEx, param->byValueEx);
		switch (param->byFlag)
		{
		case STATUS_FLAG_CALL_SCORE:
		{
			deal_res_call_score(param->byChairIDEx, param->byValueEx);
		}
			break;
		default:
		{}
			break;
		}
	}
}
//叫分结果
void  gamelogic::deal_res_call_score(BYTE byChairIDEx, BYTE byValueEx)
{
	if (byValueEx > m_curCallScore)m_curCallScore = byValueEx;
}
	

//通知类消息总接口
void  gamelogic::deal_game_notify_interface(char* buf, int len)
{
	NotifyUserOperData*	param = reinterpret_cast<NotifyUserOperData*>(buf);
	if (param != NULL)
	{
		srand((UINT)time(0));
		unsigned int ran = (rand() % 3) + 1;
		BASIC_LOG_INFO("NOTIFY_INTERFASE REV MSG byFlag(%d)-byChairID(%d)-byChairIDEx(%d), byFlagEx(%d,%d,%d)",
			param->byChairID, param->byChairIDEx, param->byFlagEx[0], param->byFlagEx[1], param->byFlagEx[2]);
		switch (param->byFlag)
		{
		case STATUS_FLAG_GAME_BEGIN:
		{
			deal_start_game(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_CALL_SCORE:
		{
			Sleep(ran*1000);
			deal_call_score(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_ROB_NT:
		{
			Sleep(ran * 1000);
			deal_rob_nt(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_ADD_DOUBLE:
		{
			Sleep(ran * 1000);
			deal_user_add_double(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_SHOW_CARD:
		{
			//Sleep(ran * 1000);
			deal_show_card(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_NT_ADD_DOUBLE:
		{
			deal_nt_add_double(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_OUT_CARD:
		{
			Sleep(ran * 1000);
			deal_out_card(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		case STATUS_FLAG_GAME_FINISH:
		{

		}
			break;
		case STATUS_FLAG_CHANGE_CARD:
		{
			Sleep((ran + 1) * 1000);
			deal_change_card(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
			break;
		//case STATUS_FLAG_SHOW_FIRST:
		//{

		//}
		//	break;

		default:
		{}
			break;
		}
	}
}
	
//游戏开始
void  gamelogic::deal_start_game(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	BASIC_LOG_INFO("GAME BEGIN");
	memset(m_UserCard, 0, sizeof(m_UserCard));
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));
	memset(m_SelectCard, 0, sizeof(m_SelectCard));
	m_byCurrentOut = 255;
	m_rabot_state = 1;//置位状态
	for (UINT i = 0; i < PLAY_COUNT; i++)
	{
		m_User_Operate[i].ClearAllData();
	}
	m_GameLogic.InitAllData();
	m_GameLogic.SetGameRule(m_game_base.gameconfig);
	m_curCallScore = 0;
	kill_timer(TIME_USER_READY_CHECK_START);
	kill_timer(TIME_USER_READY);
}
//换三张
void  gamelogic::deal_change_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	BASIC_LOG_INFO("CHANGE CARD");
	if (byFlagEx != NULL)
	{
		memcpy(m_SelectCard, byFlagEx, sizeof(m_SelectCard));
	}
		
	//发送选择的牌
	UserOperStruct param;
	param.byChairID = m_user_mgr->m_my_station;
	param.byFlag = STATUS_FLAG_CHANGE_CARD;
	memcpy(param.byFlagEx, m_SelectCard, sizeof(param.byFlagEx));
	m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
}
//抢地主
void  gamelogic::deal_rob_nt(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (m_game_base.gameconfig.byplaymode[3] == 2)
	{
		if (byChairIDEx == m_user_mgr->m_my_station)
		{
			BASIC_LOG_INFO("ROB NT");
			int rob_nt_count = 1;
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (byFlagEx[i] != 0xFF)
				{
					rob_nt_count = 1;							//别人抢过，此时是第一次
					break;
				}
			}
			if (byFlagEx[byChairIDEx] != 0xFF) rob_nt_count = 2;//自己抢过，此时为第二次
			BYTE call_nt = robort_logic_call_score(rob_nt_count);//抢地主次数

			UserOperStruct param;
			param.byChairID = m_user_mgr->m_my_station;
			param.byFlag = STATUS_FLAG_ROB_NT;
			param.byOperValue = call_nt;//1抢地主
			m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
		}
	}
}
//叫分
void  gamelogic::deal_call_score(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (m_game_base.gameconfig.byplaymode[3] != 2)
	{
		if (byChairIDEx == m_user_mgr->m_my_station)
		{
			BASIC_LOG_INFO("CALL SCORE");
			BYTE call_score = robort_logic_call_score();
			if (call_score <= m_curCallScore) call_score = 0;
			UserOperStruct param;
			param.byChairID = m_user_mgr->m_my_station;
			param.byFlag = STATUS_FLAG_CALL_SCORE;
			param.byOperValue = call_score;//叫几分
			m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
		}
	}
}
//叫分逻辑
BYTE  gamelogic::robort_logic_call_score(int RobNTtime)
{
	if (m_game_base.gameconfig.byplaymode[0] > 0)
	{//双王四个二必叫三分，或者必抢地主
		if (m_GameLogic.IsMustCallShape(m_UserCard[m_user_mgr->m_my_station], m_UserCardCount[m_user_mgr->m_my_station]))
		{
			return (m_game_base.gameconfig.byplaymode[3] != 2) ? 3 : 1;
		}
	}

	int call_score = 0;
	int card_face = m_GameLogic.InfoForCallScore(m_UserCard[m_user_mgr->m_my_station], m_UserCardCount[m_user_mgr->m_my_station]);
	if (m_game_base.gameconfig.byplaymode[3] != 2)
	{
		if (card_face >= 14) call_score = 3;
		else if (card_face >= 10) call_score = 2;
		else if (card_face >= 6)call_score = 1;
	}
	else
	{
		srand((UINT)time(0));
		unsigned int ran = (rand() % 100);
		if (card_face >= 6)
		{//大于等于8分,80%抢一次地主，40%抢二次地主
			if ((ran > 20 && RobNTtime == 1) || (ran > 60 && RobNTtime == 2)) call_score = 1;
		}
		if (card_face >= 10)
		{//大于等于10分100%抢一次地主，90%抢二次地主
			if ((RobNTtime == 1) || (ran > 10 && RobNTtime == 2)) call_score = 1;
		}
	}
	return call_score;
}

//明牌
void  gamelogic::deal_show_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
#ifdef _DEBUG
	UserOperStruct param;
	param.byChairID = m_user_mgr->m_my_station;
	param.byFlag = STATUS_FLAG_SHOW_CARD;
	param.byOperValue = 1;
	m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
#else
	if (byChairIDEx == m_user_mgr->m_my_station)
	{
		BASIC_LOG_INFO("SHOW CARD");
			
		UserOperStruct param;
		param.byChairID = m_user_mgr->m_my_station;
		param.byFlag = STATUS_FLAG_SHOW_CARD;
		param.byOperValue = 0;
		m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
	}
#endif
}
//地主加倍
void  gamelogic::deal_nt_add_double(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (byChairIDEx == m_user_mgr->m_my_station)
	{
		BASIC_LOG_INFO("NT ADD DOUBLE");
			
		UserOperStruct param;
		param.byChairID = m_user_mgr->m_my_station;
		param.byFlag = STATUS_FLAG_NT_ADD_DOUBLE;
		param.byOperValue = 0;//
		m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
	}
}
//玩家加倍
void  gamelogic::deal_user_add_double(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (byChairIDEx != m_user_mgr->m_my_station)
	{
		int add_double = 1;
		if (byFlagEx[m_user_mgr->m_my_station] == 0)
		{
			add_double = 0;
		}
		UserOperStruct param;
		param.byChairID = m_user_mgr->m_my_station;
		param.byFlag = STATUS_FLAG_ADD_DOUBLE;
		param.byOperValue = 0;//是否加倍
		m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
	}
}
	
//玩家出牌/跟牌
void  gamelogic::deal_out_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (byChairIDEx == 255)
	{
		return;
	}

	m_byCurrentOut = byChairIDEx;

	if (m_byCurrentOut == m_user_mgr->m_my_station)
	{//出牌
		BASIC_LOG_INFO("deal_out_card : m_byCurrentOut(%d), m_my_station(%d)", m_byCurrentOut, m_user_mgr->m_my_station);

		int		byCardCount = 0;					//扑克数目
		BYTE		byCardList[HAND_CARD_MAX] = { 0 };	//扑克信息

		Card_Info card_info;

		card_info.NTposition = 0;//庄家相对位置 0：本家
		if (((m_user_mgr->m_my_station + 1) % PLAY_COUNT) == m_byNTpos) card_info.NTposition = 2;//下家
		if (((m_user_mgr->m_my_station + 2) % PLAY_COUNT) == m_byNTpos) card_info.NTposition = 1;//上家
		card_info.DeskPos = 0;//桌面牌玩家相对位置 0：本家
		if (((m_user_mgr->m_my_station + 1) % PLAY_COUNT) == m_DeskPositon) card_info.DeskPos = 2;//下家
		if (((m_user_mgr->m_my_station + 2) % PLAY_COUNT) == m_DeskPositon) card_info.DeskPos = 1;//上家

		memcpy(card_info.UserCard, m_UserCard[m_user_mgr->m_my_station], sizeof(card_info.UserCard));
		card_info.UserCardCount = m_UserCardCount[m_user_mgr->m_my_station];
		memcpy(card_info.UpUserCard, m_UserCard[(m_user_mgr->m_my_station + PLAY_COUNT - 1) % PLAY_COUNT], sizeof(card_info.UpUserCard));
		card_info.UpUserCardCount = m_UserCardCount[(m_user_mgr->m_my_station + PLAY_COUNT - 1) % PLAY_COUNT];
		memcpy(card_info.DownUserCard, m_UserCard[(m_user_mgr->m_my_station + 1) % PLAY_COUNT], sizeof(card_info.DownUserCard));
		card_info.DownUserCardCount = m_UserCardCount[(m_user_mgr->m_my_station + 1) % PLAY_COUNT];

		memcpy(card_info.DeskCard, m_DeskCard, sizeof(m_DeskCard));
		card_info.DeskCardCount = m_DeskCount;

		if (m_user_mgr->m_my_station == m_BigOutUser)
		{//先手
			m_GameLogic.FirstOutCard(card_info, byCardList, byCardCount, m_User_Operate[m_user_mgr->m_my_station]);
		}
		else
		{//跟牌
			m_GameLogic.FellowCard(card_info, byCardList, byCardCount, m_User_Operate[m_user_mgr->m_my_station]);
		}

		OutCardStruct param;
		param.byDeskStation = m_user_mgr->m_my_station;
		param.byCardCount = byCardCount;
		memcpy(param.byCardList, byCardList, sizeof(param.byCardList));
		m_user_mgr->send_msg(GAME_MSG_OUT_CARD, (char*)&param, sizeof(OutCardStruct));

		std::string outcard = (char *)param.byCardList;

		if (byCardCount > 0)
		{
			BASIC_LOG_INFO("deal_out_card outcardlist(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x), byCardCount(%d)"
				, param.byCardList[0], param.byCardList[1], param.byCardList[2], param.byCardList[3], param.byCardList[4], param.byCardList[5]
				, param.byCardList[6], param.byCardList[7], param.byCardList[8], param.byCardList[9], param.byCardList[10], param.byCardList[11]
				, param.byCardList[12], param.byCardList[13], param.byCardList[14], param.byCardList[15], param.byCardList[16], param.byCardList[17]
				, param.byCardList[18], param.byCardList[19], byCardCount);
		}
		else
		{
			BASIC_LOG_INFO("chair_id(%d) pass!", m_user_mgr->m_my_station);
		}
		if (byCardCount == 0 && m_user_mgr->m_my_station == m_BigOutUser)
		{
			BASIC_LOG_ERROR("outcard error outcardlist(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x), byCardCount(%d)"
				, param.byCardList[0], param.byCardList[1], param.byCardList[2], param.byCardList[3], param.byCardList[4], param.byCardList[5]
				, param.byCardList[6], param.byCardList[7], param.byCardList[8], param.byCardList[9], param.byCardList[10], param.byCardList[11]
				, param.byCardList[12], param.byCardList[13], param.byCardList[14], param.byCardList[15], param.byCardList[16], param.byCardList[17]
				, param.byCardList[18], param.byCardList[19], byCardCount);
		}
	}	
}

void  gamelogic::set_timer(unsigned int event_id, unsigned int elapse)
{
	SetTimer(m_handle, m_user_index + MFC_TIME_START_ID * event_id, elapse, NULL);
}
void  gamelogic::OnTimer(unsigned int event_id, unsigned int user_id)
{
	switch (event_id)
	{
	case TIME_USER_READY_CHECK_START:
	{
		m_rabot_ready_time++;
		if (m_rabot_ready_time >= 60 && m_rabot_state == 0)
		{
			m_rabot_ready_time = 0;
			kill_timer(TIME_USER_READY_CHECK_START);
			BASIC_LOG_INFO("机器人 user_id = %d 准备超过时间 m_rabot_ready_time = %d并且游戏未开始,请求离开，归于空闲队列中等待", user_id, m_rabot_ready_time);
			m_user_mgr->deal_notify_user_left_from_MFC(false);
		}
	}
		break;
	case TIME_USER_SEND_READY:
	{
		kill_timer(event_id);
		m_user_mgr->user_aready(true);
	}
		break;
	default:
		break;
	}
}
	
void gamelogic::kill_timer(unsigned int event_id)
{
	KillTimer(m_handle, m_user_index + MFC_TIME_START_ID * event_id);
}