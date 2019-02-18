#include "gamelogic.h"
#include "basiclog\basicloginterface.h" 
#include "usermanager.h"
#include "useractionmanager.h"



gamelogic::gamelogic()
{
	BASIC_LOG_INIT("dachuan/log/game_logic.log");
	m_hand_card.ClearAllData();
	game_count = 0;
}


gamelogic::~gamelogic()
{
	BASIC_LOG_CLOSE();
	game_count = 0;
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
		case 1:
		{//游戏状态消息
			
			Stu_GameStation * param = reinterpret_cast<Stu_GameStation*>(buf);
			memcpy(m_chip, param->chip, sizeof(m_chip));
		}
			break;
		case GAME_MSG_SEND_RESULT:
		{//牌： 小局结束
			
			Stu_ChangeMoney* param = reinterpret_cast<Stu_ChangeMoney*>(buf);
			if (param != NULL)
			{//发送金币结果给MFC界面
				BASIC_LOG_INFO("GAME_MSG_SEND_RESULT   小局结束 玩家 = %d,输赢金币 = （%d） 服务费 m_ipayserver = %d ", m_user_mgr->m_my_userid, param->changemoney[m_user_mgr->m_my_station], m_ipayserver);
				gold_change gc;
				gc.player_id = m_user_mgr->m_my_userid;
				gc.gold = param->changemoney[m_user_mgr->m_my_station] - m_ipayserver;
				/***********************************************************************************/
				if (m_handle != NULL)
				{
					::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_CHANGE, (LPARAM)&gc);
				}
				else
				{
					BASIC_LOG_INFO("请求桌子信息 + 坐桌 找不到 Window ");
				}

			}
		}
			break;
		case GAME_MSG_SEND_GAME_OVER:
		{
			m_rabot_state = 0;
			if (game_count >= m_user_mgr->m_play_game_count)
			{//局数到，请求离开
				game_count = 0;
				m_user_mgr->deal_notify_user_left_from_MFC(false);
			}
			else
			{
				deal_count_socre(buf, len);
			}
		}
			break;
		case GAME_MSG_SEND_CALLBANKER:
		{//玩家抢庄消息
			BASIC_LOG_INFO("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  牌： 抢庄消息 ", m_user_mgr, sub_id, code_id);
		
		}
			break;
		case GAME_MSG_SEND_BANKER:
		{
			//发送庄家消息
			BASIC_LOG_INFO("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  牌： 发送庄家消息 ", m_user_mgr, sub_id, code_id);
		}
			break;

		case GAME_MSG_STATUS_CALLBANKER:
		{//通知玩家抢庄
			BASIC_LOG_INFO("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  牌： 通知玩家抢庄 ", m_user_mgr, sub_id, code_id);
			deal_send_qiang_info(buf, len);
			
		}
			break;
		case GAME_MSG_SEND_USERCARD:
		{//玩家牌消息
			m_rabot_state = 1; 
			kill_timer(TIME_USER_READY_CHECK_START);
			m_rabot_ready_time = 0;
			deal_send_cards_info(buf, len);
			
		}
			break;
		case GAME_MSG_STATUS_XIAZHU:
		{//玩家下注状态消息
			deal_send_xiazhu_info(buf, len);
		}
			break;
		case GAME_MSG_STATUS_OPEN:
		{//开牌状态消息

		}
			break;
		case GAME_MSG_CUOPAI_INFO:
		{//搓牌结果消息

		}
			break;
		case DC_ASS_GR_USER_READY:
		{//金币不足消息
			DCGR::MSG_UserAgreeResult* param = reinterpret_cast<DCGR::MSG_UserAgreeResult*>(buf);
			if (param->bSuccess == false && param->iErrCode == agree_err_type_2)
			{
				//金不足，无法准备
				BASIC_LOG_INFO("金不足，无法准备 param->dwUserID = %d", param->dwUserID);

				::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_LACK, (LPARAM)param);
			}
		}
			break;
		default:
		{

		}
			break;
	}

	return true;
}


void  gamelogic::deal_send_hand_card(char* buf, int len)
{
	hand_card_info*	param = reinterpret_cast<hand_card_info*>(buf);
	if (param != NULL)
	{ 
		for (int i = 0; i < param->card_count[m_user_mgr->m_my_station]; i++)
		{
			m_hand_card.OnAddHandCard(param->hand_card[m_user_mgr->m_my_station][i]);
		} 		
	}
}
void  gamelogic::deal_draw_hand_card(char* buf, int len)
{
	user_touch_card* param = reinterpret_cast<user_touch_card*>(buf);
	if (param != NULL && m_user_mgr->m_my_station == param->chair_id)
	{
		m_draw_card = param->card_value;
		m_hand_card.OnAddHandCard(param->card_value);
		BASIC_LOG_INFO(" robot  userid=%d my_station=%d  drawcard=%d  len=%d ", m_user_mgr->m_my_userid, m_user_mgr->m_my_station, param->card_value, m_hand_card.GetHandCardLength());
	}
}
void  gamelogic::deal_out_card(char* buf, int len)
{
		
	user_out_card_result* param = reinterpret_cast<user_out_card_result*>(buf);
	if (param != NULL && m_user_mgr->m_my_station == param->chair_id)
	{ 
		m_hand_card.OnDelHandCard(param->card_value);
		BASIC_LOG_INFO(" robot  userid=%d my_station=%d  outcard=%d  len=%d ", m_user_mgr->m_my_userid, m_user_mgr->m_my_station, param->card_value, m_hand_card.GetHandCardLength());

	}
}
void  gamelogic::deal_notify_out_card(char* buf, int len)
{
	notify_out_card* param = reinterpret_cast<notify_out_card*>(buf);
	if (param != NULL && param->chair_id == m_user_mgr->m_my_station)
	{
		user_out_card  param;
		param.chair_id = m_user_mgr->m_my_station;
		m_hand_card.CardSort();
		if (m_draw_card != 0x00)
		{
			param.card_value = m_draw_card;
			m_draw_card = 0x00;
		}
		else
		{
			param.card_value = m_hand_card.OnGetHandCard(0);
		}


		BASIC_LOG_INFO(" robot  userid=%d my_station=%d    card=%d  len=%d", m_user_mgr->m_my_userid, m_user_mgr->m_my_station, param.card_value, m_hand_card.GetHandCardLength());

		m_user_mgr->send_msg(CTS_RECV_OUT_CARD, (char*)&param, sizeof(user_out_card));
	}
}
void  gamelogic::deal_block(char* buf, int len)
{ 
	notify_block_info* blockparam = reinterpret_cast<notify_block_info*>(buf);
	if (blockparam != NULL)
	{ 
		user_block_operate param;
		param.chair_id = m_user_mgr->m_my_station;
		param.opreate_type = 0x00;
		param.out = blockparam->out;
		param.card_value[0] = blockparam->card_value;
		param.block_index = blockparam->block_index;
		m_user_mgr->send_msg(CTS_RECV_BLOCK_OPERATE, (char*)&param, sizeof(user_block_operate));
	}
}
void  gamelogic::deal_block_result(char* buf, int len)
{

}
void  gamelogic::deal_count_socre(char* buf, int len)
{
	user_agree param;
	param.bagree = true;
	param.chair_id = m_user_mgr->m_my_station;
	param.userid = m_user_mgr->m_my_userid;
	
	BASIC_LOG_INFO(" robot 准备定时器 TIME_USER_NIUNIU_READY  userid=%d  ", param.userid);
	//Sleep(5000);
	set_timer(TIME_USER_NIUNIU_READY, 2000);
	//m_user_mgr->user_aready(true);		
}

void  gamelogic::deal_send_cards_info(char* buf, int len)
{
	stu_usercard* param = reinterpret_cast<stu_usercard*>(buf);
	if (param != NULL && m_user_mgr->m_my_station == param->chairid)
	{
			
		memcpy(m_UserCard[param->chairid], param->byUsercard, sizeof(m_UserCard[param->chairid]));
		memcpy(m_UserSecCard[param->chairid], param->byUserSecCard, sizeof(m_UserSecCard[param->chairid]));
		m_bHaveSecCard = param->bHaveSecCard;

		if (param->bServeMoney[param->chairid] > 0)
		{
			m_ipayserver = param->bServeMoney[param->chairid];
		}
		
		if (param->status != 17)
		{//不是第一次发牌，最后一次发牌收开牌
			stu_userOpenCard c_param;
			c_param.chairid = m_user_mgr->m_my_station;
			c_param.openflag = 1;
			c_param.openflag = 1;
			memcpy(c_param.cardlist, m_UserCard[param->chairid], sizeof(c_param.cardlist));
			game_count++;

			srand(time(0) + m_user_mgr->m_my_userid);
			unsigned int ra = (rand() % 3) + 1;
			//Sleep(ra * 1000);
			memcpy(&m_tan_pai_param, &c_param, sizeof(stu_userOpenCard));
			set_timer(TIME_USER_TAN_CARDS, ra * 1000);
			//m_user_mgr->send_msg(GAME_MSG_RECV_OPENCARD, (char*)&c_param, sizeof(stu_userOpenCard));
		}
	}

}
/*
抢庄(牛n)	n >= 5         4倍: 40%  随机2-3倍60%
			1 <= n < 5     随机1-3倍
			0 == n         不抢
*/
void  gamelogic::deal_send_qiang_info(char* buf, int len)
{
	stu_status_qiang* param = reinterpret_cast<stu_status_qiang*>(buf);
	if (param != NULL && param->bcan[m_user_mgr->m_my_station])
	{
		srand(time(0) + m_user_mgr->m_my_userid);
		unsigned int ran = (rand() % 100);
		 
		int OX = check_has_Ox(m_UserCard[m_user_mgr->m_my_station]);
		BASIC_LOG_INFO("deal_send_qiang_info 抢庄 userid = %d ，OX = %d", m_user_mgr->m_my_userid, OX);
		if (OX == 0)
		{
			ran = 0;
		}
		else if (OX >= 1 && OX < 5)
		{
			ran = (rand() % 3) + 1;
			if (param->byxianzhi[m_user_mgr->m_my_station] < ran)
			{
				ran = param->byxianzhi[m_user_mgr->m_my_station];
			}
		}
		else if (OX >= 5)
		{
			if (ran < 40)
			{
				ran = 4;
				if (param->byxianzhi[m_user_mgr->m_my_station] < ran)
				{
					ran = param->byxianzhi[m_user_mgr->m_my_station];
				}
			}
			else if (ran > 40)
			{
				ran = (rand() % 2) + 2;
				if (param->byxianzhi[m_user_mgr->m_my_station] < ran)
				{
					ran = param->byxianzhi[m_user_mgr->m_my_station];
				}
			}
		}
		stu_callBanker c_param;
		c_param.byValue = ran;
		c_param.chairid = m_user_mgr->m_my_station;
		while (true)
		{
			srand(time(0) + m_user_mgr->m_my_userid);
			unsigned int rat = (rand() % 3) + 1;
			memcpy(&m_qiang_banker_param, &c_param, sizeof(stu_callBanker));
			set_timer(TIME_USER_QIANG_BANKER, rat * 1000);
			break;
		}
			
	}
	
}
//玩家下注
/*
下注(牛n) ：n >= 5        2-4项(随机)
			1<= n< 5      1-2项(随机)
			0 == n        1项
*/
void   gamelogic::deal_send_xiazhu_info(char* buf, int len)
{
	stu_status_XZ* param = reinterpret_cast<stu_status_XZ*>(buf);
	stu_userXiaZhu c_param;
	if (param != NULL )
	{
		srand(time(0) + m_user_mgr->m_my_userid);
		unsigned int ra = 0;
		c_param.chairid = m_user_mgr->m_my_station;
		int OX = check_has_Ox(m_UserCard[m_user_mgr->m_my_station]);
		BASIC_LOG_INFO("deal_send_xiazhu_info 下注 userid = %d ，OX = %d", m_user_mgr->m_my_userid, OX);
		
		if (OX >= 5)
		{//四张牌有牛
			ra = (rand() % 3) + 1;
			c_param.xiazhumoney = m_chip[ra];
		}
		else if (OX < 5 && OX >= 1)

		{
			ra = (rand() % 2);
			c_param.xiazhumoney = m_chip[ra];
		}
		else
		{
			c_param.xiazhumoney = m_chip[0];
		}
		unsigned int rant = rand() % 2;
		int xiazhuTime = (rant + 1) * 1000;
		//BASIC_LOG_INFO("机器人  m_user_mgr->m_my_userid = %d 下注定时器  = %d deal_send_xiazhu_info ", m_user_mgr->m_my_userid, xiazhuTime);
		set_timer(TIME_USER_XIA_ZHU, xiazhuTime);
		memcpy(&m_xia_zhu_param, &c_param, sizeof(stu_userXiaZhu));
		//m_user_mgr->send_msg(GAME_MSG_RECV_XIAZHU, (char*)&c_param, sizeof(stu_userXiaZhu));
	}

}
//检测4张牌中是否有牛
int   gamelogic::check_has_Ox(BYTE cardlist[])
{

	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (cardlist[i] == 0)continue;
		for (int j = i + 1; j < MAX_COUNT; j++)
		{
			if (cardlist[j] == 0)continue;
			for (int k = j + 1; k < MAX_COUNT; k++)
			{
				if (cardlist[k] == 0)continue;
				BYTE temp1 = (cardlist[i] & 0x0F) > 10 ? 10 : (cardlist[i] & 0x0F);
				BYTE temp2 = (cardlist[j] & 0x0F) > 10 ? 10 : (cardlist[j] & 0x0F);
				BYTE temp3 = (cardlist[k] & 0x0F) > 10 ? 10 : (cardlist[k] & 0x0F);
				if ((temp1 + temp2 + temp3) % 10 == 0)
				{
					BYTE num = 0;
					for (int n = 0; n < MAX_COUNT; n++)
					{
						if (n == i || n == j || n == k)
						{
							continue;
						}
						num += ((cardlist[n] & 0x0F) > 10 ? 10 : (cardlist[n] & 0x0F));
						BASIC_LOG_INFO("gamelogic::check_has_Ox  i(%d)j(%d)k(%d) cardlist[%d] & 0x0F = %d ",i,j,k, n, num);
					}

					return num;
				}
			}

		}

	}

	return 0;
	
}

void  gamelogic::deal_send_change_desk(unsigned int chair_id)
{
	m_user_mgr->user_change_desk(chair_id);
	
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
		BASIC_LOG_INFO("m_rabot_ready_time ==================================== %d ",m_rabot_ready_time);
		if (m_rabot_ready_time >= 60 && m_rabot_state == 0)
		{
			m_rabot_ready_time = 0;
			kill_timer(TIME_USER_READY_CHECK_START);
			BASIC_LOG_INFO("机器人 user_id = %d 准备超过时间 m_rabot_ready_time = %d并且游戏未开始,请求离开，归于空闲队列中等待", user_id, m_rabot_ready_time);
			m_user_mgr->deal_notify_user_left_from_MFC(false);
		}
	}
		break;
	case TIME_USER_XIA_ZHU:
	{
		kill_timer(event_id);
		BASIC_LOG_INFO("机器人 user_id = %d 下注 aaaaaaaaaaaaaa m_xia_zhu_param.chairid = %d, m_xia_zhu_param.xiazhumoney = %d ", user_id, m_xia_zhu_param.chairid, m_xia_zhu_param.xiazhumoney);
		m_user_mgr->send_msg(GAME_MSG_RECV_XIAZHU, (char*)&m_xia_zhu_param, sizeof(stu_userXiaZhu));

	}
		break;
	case TIME_USER_TAN_CARDS:
	{
		kill_timer(event_id);
		m_user_mgr->send_msg(GAME_MSG_RECV_OPENCARD, (char*)&m_tan_pai_param, sizeof(stu_userOpenCard));
	}
		break;
	case TIME_USER_QIANG_BANKER:
	{
		kill_timer(event_id);
		m_user_mgr->send_msg(GAME_MSG_USER_ROBNT_INFO, (char*)&m_qiang_banker_param, sizeof(stu_callBanker));
	}
		break;
	case TIME_USER_NIUNIU_READY:
	{
		kill_timer(event_id);
		BASIC_LOG_INFO(" 发送准备消息 启动准备检测 m_user_mgr->m_my_userid = %d", m_user_mgr->m_my_userid);

		if (NULL != m_user_mgr)
		{
			if (m_user_mgr->OncheckUserready())
			{
				m_user_mgr->user_aready(true);
				kill_timer(TIME_USER_READY_CHECK_START);
				m_rabot_ready_time = 0;
				set_timer(TIME_USER_READY_CHECK_START, 1000);
			}
			else
			{
				m_user_mgr->deal_notify_user_left_from_MFC(false);
			}
		}	
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