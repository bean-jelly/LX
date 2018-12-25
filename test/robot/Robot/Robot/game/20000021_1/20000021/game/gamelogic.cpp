#include "gamelogic.h"
#include "../user/usermanager.h" 
#include "basiclog\basicloginterface.h" 
#include "gamemsg.h"

gamelogic::gamelogic()
{
	BASIC_LOG_INIT("dachuan/log/20000021_AI_logic.log");
	m_game_base.gameconfig.InitConfig();

	memset(m_UserCard, 0, sizeof(m_UserCard));					//�������
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));		//�����������
	memset(m_SelectCard, 0, sizeof(m_SelectCard));				//������ѡ�����
	memset(m_TurnCardData, 0, sizeof(m_TurnCardData));			//��ǰ���ֳ���

	m_CardDroit			=		255;							//��ǰ�������
	m_TurnCardCount		=		0;								//��ǰ���ֳ�������
	m_byCurrentOut		=		255;							//��ǰ�������
	m_Banker			=		255;							//ׯ��λ��
	m_curCallScore		=		0;								//��ǰ�зַ�ֵ
	game_count			=		0;								//��Ϸ����
	m_rabot_state		=		0;								//������״̬ 0 ���� ��1 ��Ϸ��
	m_user_mgr			=		NULL;
	m_user_index		=		255;
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
		}
	}
	break;
	case GAME_MSG_NOTIFY_INFO:
	{
		BASIC_LOG_INFO("gamenotify deal_game_notify_interface, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		deal_game_notify_interface(buf, len);
	}
	break;
	case GAME_MSG_SEND_ALL_CARD:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_new_turn, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_send_hand_card(buf, len);
	}
	break;
	case GAME_MSG_SEND_CARD_TO_ROBOT:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_get_other_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_get_other_card(buf, len);
	}
	case GAME_MSG_NEW_TURN:
	{
		BASIC_LOG_INFO("gamenotify deal_new_turn, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_new_turn(buf, len);
	}
	break;
	case GAME_MSG_CHANGE_CARD:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_change_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_change_card(buf, len);
	}
	break;
	case GAME_MSG_BACK_CARD_EX:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_get_back_card, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
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
		BASIC_LOG_INFO("gamenotify notifymsg_out_card_result, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_out_card_result(buf, len);
	}
	break;
	case GAME_MSG_SET_GAEM_END:
	{
		BASIC_LOG_INFO("gamenotify notifymsg_game_end, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		notifymsg_game_end(buf, len);
	}
	break;
	case DC_ASS_GR_GAME_FINISH:
	{
		BASIC_LOG_INFO("gamenotify game_finish, m_user_mgr=%0x  sub_id=%d   code_id=%d", m_user_mgr, sub_id, code_id);
		game_count = 0;
	}
	break;
	case DC_ASS_GR_USER_READY:
	{
		//��Ҳ�����Ϣ
		DCGR::MSG_UserAgreeResult* param = reinterpret_cast<DCGR::MSG_UserAgreeResult*>(buf);
		if (param->bSuccess == false && param->iErrCode == agree_err_type_2)
		{
			//��Ҳ��㣬�޷�׼��
			BASIC_LOG_INFO("��Ҳ��㣬�޷�׼�� param->dwUserID = %d", param->dwUserID);
			::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_LACK, (LPARAM)param);
		}
		if (param->bSuccess == true && param->iErrCode == 0 && param->dwUserID == m_user_mgr->m_my_userid)
		{
			//׼���ɹ���������Ϸ����
			m_user_mgr->m_user_action_mgr->send_get_game_info();
		}
	}
	break;
	default:
	{
		break;
	}
	}

	return true;
}

//������Ϣ
void  gamelogic::notifymsg_send_hand_card(char* buf, int len)
{
	SendAllStruct* param = reinterpret_cast<SendAllStruct*>(buf);
	if (param != NULL)
	{
		if ((m_user_mgr->m_my_station >= 0) && (m_user_mgr->m_my_station < PLAY_COUNT))
		{
			memcpy(m_UserCard[m_user_mgr->m_my_station], param->byCardList, sizeof(param->byCardList));
			memcpy(m_UserCardCount, param->byCardCount, sizeof(m_UserCardCount));

			m_RobotAI.SetUserCard(m_user_mgr->m_my_station, param->byCardList, param->byCardCount[m_user_mgr->m_my_station]);
			memcpy(m_RobotAI.m_UserCardCount, param->byCardCount, sizeof(param->byCardCount));

			BASIC_LOG_INFO(" robot  userid(%d) my_station(%d)  len(%d)", m_user_mgr->m_my_userid, m_user_mgr->m_my_station, m_UserCardCount[m_user_mgr->m_my_station]);
		}
	}
}

//�����˻�ȡ�����˵���
void gamelogic::notifymsg_get_other_card(char* buf, int len)
{
	SendAllCardInfoStruct* param = reinterpret_cast<SendAllCardInfoStruct*>(buf);
	if (param != NULL)
	{
		//�¼�λ��
		BYTE UnderSideMeStation = (m_user_mgr->m_my_station + 1) % GAME_PLAYER;
		//�ϼ�λ��
		BYTE UpsideMeStation = (UnderSideMeStation + 1) % GAME_PLAYER;

		m_RobotAI.SetUserCard(UnderSideMeStation, param->byPostCardCount, 17);
		m_RobotAI.SetUserCard(UpsideMeStation, param->byPreCardCount, 17);
	}
}

//��һ�ֿ�ʼ
void  gamelogic::notifymsg_new_turn(char* buf, int len)
{
	NewTurnStruct* param = reinterpret_cast<NewTurnStruct*>(buf);
	if (param != NULL)
	{
		memset(m_TurnCardData, 0, sizeof(m_TurnCardData));
		m_TurnCardCount = 0;
		m_CardDroit = param->bDeskStation;

		deal_out_card(param->bDeskStation);
	}
	return;
}

//������
void  gamelogic::notifymsg_change_card(char* buf, int len)
{
	ChangeCardStruct* param = reinterpret_cast<ChangeCardStruct*>(buf);
	if (param != NULL)
	{
		if (param->byDeskPos == m_user_mgr->m_my_station)
		{
			m_RobotAI.RemoveCards(param->byChooseCardList, 3, m_UserCard[m_user_mgr->m_my_station], m_UserCardCount[m_user_mgr->m_my_station]);

			m_RobotAI.RemoveCards(param->byChooseCardList, 3, m_RobotAI.m_UserCardData[m_user_mgr->m_my_station], m_RobotAI.m_UserCardCount[m_user_mgr->m_my_station]);

			int pos = m_UserCardCount[m_user_mgr->m_my_station] - 3;
			memcpy(&m_UserCard[m_user_mgr->m_my_station][pos], param->byGetCardList, sizeof(BYTE) * 3);
			memcpy(&m_RobotAI.m_UserCardData[m_user_mgr->m_my_station][pos], param->byGetCardList, sizeof(BYTE) * 3);
		}
	}
}

//��ȡ����
void  gamelogic::notifymsg_get_back_card(char* buf, int len)
{
	BackCardExStruct* param = reinterpret_cast<BackCardExStruct*>(buf);
	if (param != NULL)
	{
		m_byCurrentOut = param->iGiveBackPeople;
		m_Banker = param->iGiveBackPeople;
		
		memcpy(&m_UserCard[m_byCurrentOut][m_UserCardCount[m_byCurrentOut]], param->iBackCard, 3);

		m_RobotAI.SetBackCard(m_byCurrentOut, param->iBackCard, param->iBackCardCount);
		m_RobotAI.SetBanker(m_byCurrentOut);
	}
}

//���ƽ����Ϣ
void  gamelogic::notifymsg_out_card_result(char* buf, int len)
{
	OutCardMsg* param = reinterpret_cast<OutCardMsg*>(buf);
	if (param != NULL)
	{
		if (param->byCardCount > 0)
		{
			m_CardDroit = param->bDeskStation;
			m_TurnCardCount = param->byCardCount;
			memset(m_TurnCardData, 0, m_TurnCardCount);
			memcpy(m_TurnCardData, param->iCardList, param->byCardCount);
			
			//���ñ���
			m_RobotAI.RemoveUserCardData(param->bDeskStation, param->iCardList, param->byCardCount);
			
			m_RobotAI.RemoveCards(param->iCardList, param->byCardCount, m_RobotAI.m_AllCardData, m_RobotAI.m_AllCardCount);
			m_RobotAI.m_AllCardCount -= param->byCardCount;
			
			BASIC_LOG_INFO("[%d]�ѳ��� ����Ϊ[%d]", param->bDeskStation, param->byCardCount);
		}
		else
		{
			BASIC_LOG_INFO("Ҫ����");
		}
	}
}

//������Ϣ
void  gamelogic::notifymsg_game_end(char* buf, int len)
{
	m_RobotAI.init();
	m_rabot_state = 0;
	game_count++;
	long long current_time = time(0);

	notify_game_result*	param = reinterpret_cast<notify_game_result*>(buf);

	/*�����еĽ��ˢ��*/
	gold_change gold_change_param;
	gold_change_param.player_id = m_user_mgr->m_my_userid;
	gold_change_param.gold = param->iTurePoint[m_user_mgr->m_my_station] - m_game_base.tax;
	::SendMessage(m_user_mgr->m_hWnd, WM_USER + 100, USER_STATE_TO_MFC_GOLD_CHANGE, (LPARAM)&gold_change_param);

	if (game_count >= m_user_mgr->m_play_game_count)
	{
		BASIC_LOG_INFO("deal_game_end user_left");
		game_count = 0;
		m_user_mgr->deal_notify_user_left_from_MFC(false);
	}
	else
	{
		BASIC_LOG_INFO("deal_game_end user ready");
		set_timer(TIME_USER_SEND_READY, 3000);
	}
}

//�����ɢ
void  gamelogic::notifymsg_free_game(char* buf, int len)
{
	DCGR::MSG_NotifyGameFinish* param = reinterpret_cast<DCGR::MSG_NotifyGameFinish*>(buf);
	Sleep(2000);
	m_user_mgr->user_change_desk(m_user_mgr->m_my_userid);
	return;
}

//��Ҳ�����Ϣ
void  gamelogic::deal_res_interface(char* buf, int len)
{
	DealUserOperRes* param = reinterpret_cast<DealUserOperRes*>(buf);
	if (param != NULL)
	{
		switch (param->byFlag)
		{
		case STATUS_FLAG_CALL_SCORE:
		{
			deal_res_call_score(param->byChairIDEx, param->byValueEx);
		}
		break;
		default:
		{
			break;
		}
		}
	}
	return;
}

//�зֽ��
void  gamelogic::deal_res_call_score(BYTE byChairID, BYTE byValueEx)
{
	if (byValueEx > m_curCallScore)
	{
		m_curCallScore = byValueEx;
	}
	return;
}

//֪ͨ����Ϣ�ܽӿ�
void  gamelogic::deal_game_notify_interface(char* buf, int len)
{
	NotifyUserOperData*	param = reinterpret_cast<NotifyUserOperData*>(buf);
	srand((UINT)time(0));
	unsigned int ran = (rand() % 3) + 1;

	if (param != NULL)
	{
		switch (param->byFlag)
		{
		case STATUS_FLAG_GAME_BEGIN:
		{
			deal_start_game(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
		break;
		case STATUS_FLAG_CALL_SCORE:
		{
			Sleep(ran * 1000);
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
			deal_out_card(param->byChairIDEx, param->byFlagEx);
		}
		break;
		case STATUS_FLAG_GAME_FINISH:
		{
		}
		break;
		case STATUS_FLAG_CHANGE_CARD:
		{
			Sleep(ran * 1000);
			deal_change_card(param->byChairID, param->byChairIDEx, param->byFlagEx);
		}
		break;
		default:
		{
			break; 
		}
		}
	}
}

//��Ϸ��ʼ
void  gamelogic::deal_start_game(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	BASIC_LOG_INFO("GAME BEGIN");
	memset(m_UserCard, 0, sizeof(m_UserCard));
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));
	memset(m_SelectCard, 0, sizeof(m_SelectCard));
	m_byCurrentOut = 255;
	m_rabot_state = 1;
	m_curCallScore = 0;

	kill_timer(TIME_USER_READY_CHECK_START);
	kill_timer(TIME_USER_READY);
}

//������
void  gamelogic::deal_change_card(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	BASIC_LOG_INFO("CHANGE CARD");
	if (byFlagEx != NULL)
	{
		memcpy(m_SelectCard, byFlagEx, sizeof(m_SelectCard));
	}

	//����ѡ�����
	UserOperStruct param;
	param.byChairID = m_user_mgr->m_my_station;
	param.byFlag = STATUS_FLAG_CHANGE_CARD;
	memcpy(param.byFlagEx, m_SelectCard, sizeof(param.byFlagEx));
	m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
}

//������
void  gamelogic::deal_rob_nt(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	//m_game_base.gameconfig.byplaymode[3] == 2ʱΪ������
	if (m_game_base.gameconfig.byplaymode[3] == 2)
	{
		if (byChairIDEx == m_user_mgr->m_my_station)
		{
			int rob_nt_count = 1;
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (byFlagEx[i] != 0xFF)
				{
					rob_nt_count = 1;								//������������ʱ�ǵ�һ��
					break;
				}
			}
			if (byFlagEx[byChairIDEx] != 0xFF)
			{
				rob_nt_count = 2;									//�Լ���������ʱΪ�ڶ���
			}
			BYTE call_nt = robort_logic_call_score(rob_nt_count);	//����������

			UserOperStruct param;
			param.byChairID = m_user_mgr->m_my_station;
			param.byFlag = STATUS_FLAG_ROB_NT;
			param.byOperValue = call_nt;
			m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
		}
	}
}

//�з�
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
			param.byOperValue = call_score;//�м���
			m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
		}
	}
}

//�з��߼�
BYTE  gamelogic::robort_logic_call_score(int RobNTtime)
{
	BYTE ChairID = m_user_mgr->m_my_station;

	tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
	memset(MeOutCardTypeResult, 0, sizeof(MeOutCardTypeResult));
	m_RobotAI.AnalyseOutCardType(m_RobotAI.m_UserCardData[ChairID], m_RobotAI.m_UserCardCount[ChairID], MeOutCardTypeResult);

	if (m_game_base.gameconfig.byplaymode[0] > 0)
	{
		//˫���ĸ����ؽ����֣����߱�������
		
		if (MeOutCardTypeResult[CT_MISSILE_CARD].CardTypeCount > 0 ||
			m_RobotAI.GetCardLogicValue(MeOutCardTypeResult[CT_BOMB_CARD].CardData[0][0]) == 15)
		{
			return (m_game_base.gameconfig.byplaymode[3] != 2) ? 3 : 1;
		}
	}

	int CallScore = 0;
	int Point = m_RobotAI.LandScore(m_user_mgr->m_my_station);
	if (m_game_base.gameconfig.byplaymode[3] != 2)
	{
		if (Point >= 14)
		{
			CallScore = 3;
		}
		else if (Point >= 10)
		{
			CallScore = 2;
		}
		else if (Point >= 6)
		{
			CallScore = 1;
		}
	}
	else
	{
		srand((UINT)time(0));
		unsigned int RanValue = (rand() % 100);
		if (Point >= 6)
		{
			//���ڵ���8��,80%��һ�ε�����40%�����ε���
			if ((RanValue > 20 && RobNTtime == 1) || (RanValue > 60 && RobNTtime == 2))
			{
				CallScore = 1;
			}
		}
		if (Point >= 10)
		{
			//���ڵ���10��100%��һ�ε�����90%�����ε���
			if ((RobNTtime == 1) || (RanValue > 10 && RobNTtime == 2))
			{
				CallScore = 1;
			}
		}
	}
	return CallScore;
}

//����
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

//�����ӱ�
void  gamelogic::deal_nt_add_double(BYTE byChairID, BYTE byChairIDEx, BYTE byFlagEx[])
{
	if (byChairIDEx == m_user_mgr->m_my_station)
	{
		BASIC_LOG_INFO("NT ADD DOUBLE");

		UserOperStruct param;
		param.byChairID = m_user_mgr->m_my_station;
		param.byFlag = STATUS_FLAG_NT_ADD_DOUBLE;
		param.byOperValue = 0;
		m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
	}
}

//��Ҽӱ�
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
		param.byOperValue = 0;
		m_user_mgr->send_msg(GAME_MSG_USER_OPER_INFO, (char*)&param, sizeof(UserOperStruct));
	}
}

//��ҳ���/����
void  gamelogic::deal_out_card(BYTE byChairID, BYTE byFlagEx[])
{
	if (byChairID == 255)
	{
		return;
	}

	if (byChairID == m_user_mgr->m_my_station)
	{
		tagOutCardResult OutCardRes;
		m_RobotAI.SortCardList(m_TurnCardData, m_TurnCardCount, ST_ORDER);

		try
		{
			m_RobotAI.SearchOutCard(
				m_RobotAI.m_UserCardData[byChairID],
				m_RobotAI.m_UserCardCount[byChairID],
				m_TurnCardData,
				m_TurnCardCount,
				m_CardDroit,
				byChairID,
				OutCardRes);
		}
		catch (...)
		{
			memset(OutCardRes.ResultCard, 0, sizeof(OutCardRes.ResultCard));
			OutCardRes.CardCount = 0;
		}
		if (OutCardRes.CardCount > 0 && m_RobotAI.GetCardType(OutCardRes.ResultCard, OutCardRes.CardCount) == CT_ERROR)
		{
			memset(OutCardRes.ResultCard, 0, sizeof(OutCardRes.ResultCard));
			OutCardRes.CardCount = 0;
		}

		if (m_TurnCardCount == 0)
		{
			if (OutCardRes.CardCount == 0)
			{
				//��Сһ��
				OutCardRes.CardCount = 1;
				OutCardRes.ResultCard[0] = m_RobotAI.m_UserCardData[byChairID][m_RobotAI.m_UserCardCount[byChairID] - 1];
			}
		}
		else
		{
			if (!m_RobotAI.CompareCard(m_TurnCardData, OutCardRes.ResultCard, m_TurnCardCount, OutCardRes.CardCount))
			{
				memset(OutCardRes.ResultCard, 0, sizeof(OutCardRes.ResultCard));
				OutCardRes.CardCount = 0;
			}
		}

		OutCardStruct param;
		param.byDeskStation = byChairID;
		param.byCardCount	= OutCardRes.CardCount;
		memcpy(param.byCardList, OutCardRes.ResultCard, OutCardRes.CardCount);

		m_user_mgr->send_msg(GAME_MSG_OUT_CARD, (char*)&param, sizeof(OutCardStruct));
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
			BASIC_LOG_INFO("������ user_id = %d ׼������ʱ�� m_rabot_ready_time = %d������Ϸδ��ʼ,�����뿪�����ڿ��ж����еȴ�", user_id, m_rabot_ready_time);
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