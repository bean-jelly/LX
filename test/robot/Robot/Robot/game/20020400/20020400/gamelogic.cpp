#include "gamelogic.h"
#include "basiclog\basicloginterface.h"

gamelogic::gamelogic()
{
	set_mj_logic();
}

gamelogic::~gamelogic()
{

}

bool gamelogic::gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{
	return true;
}
bool gamelogic::gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{
	utility::BasicCriticalSection lock(m_game_mutex);
	//BASIC_LOG_INFO("gamenotify ��Ϸ֪ͨ��Ϣ sub_id = %d,code_id = %d ", sub_id, code_id);
	switch (sub_id)
	{
	case STC_SEND_GAME_RESULT:
	{//�������
		BASIC_LOG_INFO
		("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  �ƣ� ��Ϸ������׼����ʼ ",
			m_user_mgr, sub_id, code_id
		);
		m_rabot_state = 0;
		long long current_times = time(0);
		BASIC_LOG_INFO
		("gamenotify  ���id = %d, ��Ϸ���� game_count = %d������������ = %d ,current_times = %d ,m_user_mgr->m_login_time = %d, m_user_mgr->m_user_action_mgr->m_login_time = %d,m_user_mgr->m_user_action_mgr->m_play_time =%d",
			m_user_mgr->m_my_userid, 
			game_count,
			m_user_mgr->m_play_game_count,
			current_times,
			m_user_mgr->m_login_time,
			m_user_mgr->m_user_action_mgr->m_login_time, 
			m_user_mgr->m_user_action_mgr->m_play_time
		);

		//������ҽ��
		notify_game_result* game_result_param = reinterpret_cast<notify_game_result*>(buf);
		gold_change gold_change_param;
		gold_change_param.player_id = m_user_mgr->m_my_userid;
		gold_change_param.gold = game_result_param->total_score[m_user_mgr->m_my_station];
		gold_change_param.gold -= m_servermoney;
		::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_CHANGE, (LPARAM)&gold_change_param);

		if (game_count++ >= m_user_mgr->m_play_game_count)
		{
			//�������������뿪
			game_count = 0;
			m_user_mgr->deal_notify_user_left_from_MFC(false);
		}
		else
		{
			deal_count_socre(buf, len);
		}
	}
		break;
	case STC_SEND_MAKER_INFO:
	{//��ׯ��
		maker_info* info = reinterpret_cast<maker_info*>(buf);
		m_makers_station = info->chair_id;
		m_servermoney = info->server_money;
	}
		break;
	case STC_SEND_OUT_CARD:
	{//��ҳ��ƽ��
		user_out_card_result* param = reinterpret_cast<user_out_card_result*>(buf);
		m_has_out[param->card_value]++;
		if (m_has_out[param->card_value] > 4)
		{
			m_has_out[param->card_value] = 4;
		}
	}
		break;
	case STC_SEND_CARD_INFO:
	{
		//��ʼ��Ϸǰ�ĳ�ʼ��
		init_for_game();
		//��������
		hand_card_info* card_info = reinterpret_cast<hand_card_info*>(buf); 
		unsigned int len = sizeof(card_info->hand_card[m_base_info.my_station]);
		m_user_handcard.ClearAllData();
		for (int i = 0; i < len;++i)
		{
			m_user_handcard.OnAddHandCard(card_info->hand_card[m_base_info.my_station][i], 1);
		}
		m_user_handcard.CardSort();

		printhandcard(m_base_info.my_station);

	}
		break;
	case STC_SEND_GET_CARD:
	{//��һ����
		user_touch_card* card_info = reinterpret_cast<user_touch_card*>(buf);
		if (card_info->chair_id != m_base_info.my_station) break;
		m_user_handcard.OnAddHandCard(card_info->card_value, 1);
		
		printhandcard(m_base_info.my_station);
	}
		break;
	case STC_SEND_BLOCK_INFO:
	{//�����Լ���������Ϣ
		notify_block_info* param = reinterpret_cast<notify_block_info*>(buf);
		if (param==NULL) break;
		m_cur_block_info = *param;
		if (param->checked_id == m_base_info.my_station)
		{
			if (param->block_level >= BLOCK_TYPE_HU)
			{//ֱ�Ӻ�
				BASIC_LOG_INFO("�����˺���");
				int randtime = rand() % 2000 + 650;
				set_timer(TIME_USER_HZMJ_HU, randtime);
			}
			else if (param->block_level == BLOCK_TYPE_GANG)
			{
				BASIC_LOG_INFO("�����˰���");
				int randtime = rand() % 2000 + 750;
				set_timer(TIME_USER_HZMJ_GANG, randtime);

			}
		}
		else
		{
			int randtime = rand() % 4000 + 1000;
			set_timer(TIME_USER_HZMJ_BLOCK, randtime);
		}
	}
		break;
	case STC_SEND_BLOCK_RESULT:
	{//֪ͨ����������ƴ�����
		notify_block_reslut* param = reinterpret_cast<notify_block_reslut*>(buf);
		if (param->opreate_type == BLOCK_TYPE_PENG)
		{
			m_has_out[param->card_value[0]] += 2;
			if (m_has_out[param->card_value[0]] >4)
			{
				m_has_out[param->card_value[0]] = 4;
			}
			if (param->chair_id == m_base_info.my_station)
			{
				BASIC_LOG_INFO("���������");
				unsigned int count = m_user_handcard.OnGetCardCount(param->card_value[0]);
				m_user_handcard.OnDelHandCard(param->card_value[0], count);
			}
				
		}
		else if (param->opreate_type == BLOCK_TYPE_GANG)
		{
			m_has_out[param->card_value[0]] += 3;
		
			if (param->chair_id == m_base_info.my_station)
			{
				BASIC_LOG_INFO("�ܲ������");
				if (param->block_subtype == BLOCK_TYPE_GANG_AN)
				{
					m_has_out[param->card_value[0]]++;
				}
				unsigned int count = m_user_handcard.OnGetCardCount(param->card_value[0]);
				m_user_handcard.OnDelHandCard(param->card_value[0], count);
			}

			if (m_has_out[param->card_value[0]] > 4)
			{
				m_has_out[param->card_value[0]] = 4;
			}
		}
		printhandcard(m_base_info.my_station);
	}
		break;
	case STC_SEND_NOTIFY_OUT:
	{//֪ͨ��ҳ���
		notify_out_card* param = reinterpret_cast<notify_out_card*>(buf);
		if (param==NULL)break;
		if (param->chair_id == m_base_info.my_station)
		{
			int randtime = rand() % 3500 + 1500;
			set_timer(TIME_USER_HZMJ_OUT_CARD, randtime);
		}
	}
		break;
	case STC_SEND_OTHER_BLOCK:
	{//�ȴ�������ҵ����Ʋ���
		//do nothing
	}
		break;

	case DC_ASS_GR_USER_READY:
	{//��Ҳ�����Ϣ
		DCGR::MSG_UserAgreeResult* param = reinterpret_cast<DCGR::MSG_UserAgreeResult*>(buf);
		if (param->bSuccess == false && param->iErrCode == agree_err_type_2)
		{
			//���㣬�޷�׼��
			BASIC_LOG_INFO("���㣬�޷�׼�� param->dwUserID = %d", param->dwUserID);

			::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_LACK, (LPARAM)param);
		}
		else if (param->bSuccess == true)
		{
			m_user_mgr->m_user_action_mgr->send_get_game_info();
		}
	}
		break;
	case DC_ASS_GR_GAMEINFOR:
	{//�õ��Լ���Ϣ
		sc_game_status_ready* param_ready = reinterpret_cast<sc_game_status_ready*>(buf);
		if (param_ready != NULL)
		{
			m_base_info = param_ready->base_info;
		}
		sc_game_status_play* param_play = reinterpret_cast<sc_game_status_play*>(buf);
		if (param_play!=NULL)
		{
			m_base_info = param_play->base_info;
		}
	}
		break;
	default:
		break;
	}

	return true;
}

void gamelogic::set_timer(unsigned int event_id, unsigned int elapse)
{
	SetTimer(m_handle, m_user_index + MFC_TIME_START_ID * event_id, elapse, NULL);
}

void gamelogic::kill_timer(unsigned int event_id)
{
	KillTimer(m_handle, m_user_index + MFC_TIME_START_ID * event_id);
}

void gamelogic::OnTimer(unsigned int event_id, unsigned int user_id)
{
	switch (event_id)
	{
	case TIME_USER_READY_CHECK_START:
	{
		m_rabot_ready_time++;
		if (m_rabot_ready_time >= 60 && m_rabot_state == 0)
		{
			m_rabot_ready_time = 0;
			kill_timer(event_id);
			BASIC_LOG_INFO("������ user_id = %d ׼������ʱ�� m_rabot_ready_time = %d������Ϸδ��ʼ,�����뿪�����ڿ��ж����еȴ�", user_id, m_rabot_ready_time);
			m_user_mgr->deal_notify_user_left_from_MFC(false);
		}
	}
		break;
	case TIME_USER_HZMJ_READY:
	{
		kill_timer(event_id);
		if (m_user_mgr != NULL)
		{
			if (m_user_mgr->OncheckUserready())
			{
				m_user_mgr->user_aready(true);
				m_user_mgr->set_timer(TIME_USER_READY_CHECK_START, 1000);
			}
			else
			{
				m_user_mgr->deal_notify_user_left_from_MFC(false);
			}
		}
		
	}
		break;
	case TIME_USER_HZMJ_OUT_CARD:
	{
		kill_timer(event_id);
		deal_user_out_card();
	}
		break;
	case TIME_USER_HZMJ_BLOCK:
	{
		kill_timer(event_id);
		deal_user_block();
	}
		break;
	case TIME_USER_HZMJ_HU:
	{
		kill_timer(event_id);
		deal_send_hu_msg();
	}
		break;
	case TIME_USER_HZMJ_GANG:
	{	
		kill_timer(event_id);
		deal_send_gang_msg();
	}
		break;
	case TIME_USER_HZMJ_LEFT_DESK:
	{
		kill_timer(event_id);
		m_user_mgr->deal_notify_user_left_from_MFC(false);
	}
		break;
	default:
		break;
	}
}

void gamelogic::deal_send_change_desk(unsigned int chair_id)
{
	m_user_mgr->set_swap_desk(true);
	m_user_mgr->user_change_desk(chair_id);
}

void gamelogic::deal_count_socre(char* buf, int len)
{
	user_agree param;
	param.bagree = true;
	param.chair_id = m_user_mgr->m_my_station;
	param.userid = m_user_mgr->m_my_userid;
	int randtime = 0;

	notify_game_result* card_info = reinterpret_cast<notify_game_result*>(buf);
	//��ȡ�Լ�������
	CMJHandCard temphand;
	unsigned int card_len = sizeof(card_info->hand_card[m_base_info.my_station]);
	temphand.ClearAllData();
	for (int i = 0; i < card_len; ++i)
	{
		temphand.OnAddHandCard(card_info->hand_card[m_base_info.my_station][i], 1);
	}
	//��ö�������ʱ��
	int time_view = get_ready_time(card_info);

	if (temphand.GetHandCardLength()>0) //�����Թ۵Ļ�
	{
		randtime = rand() % 7000 + time_view * 1000 + 1500;
	}
	else
	{
		randtime = rand() % 750 + time_view * 1000 + 650;
	}
	set_timer(TIME_USER_HZMJ_READY, randtime);
}

void gamelogic::deal_user_out_card()
{
	//�ҳ���Ҫ������
	int index = find_best_out_card();

	//������Ϣ
	user_out_card param;
	param.chair_id = m_base_info.my_station;
	param.card_value = m_user_handcard.OnGetHandCard(index);
	param.card_index = index;
	m_user_handcard.OnDelHandCard(param.card_value, 1);
	m_user_mgr->send_msg(CTS_RECV_OUT_CARD, (char*)&param, sizeof(user_out_card));

	std::string temp = "";
	temp = findcard(param.card_value);
	BASIC_LOG_INFO("����ƣ�%s", temp.c_str());

	printhandcard(m_base_info.my_station);
}


void gamelogic::deal_user_block()
{
	std::string lv = std::to_string(m_cur_block_info.block_level);
	BASIC_LOG_INFO("�յ�������Ϣ,���Ƶȼ�%s", lv.c_str());
	user_block_operate param;
	param.chair_id = m_base_info.my_station;
	param.block_index = m_cur_block_info.block_index;
	param.out = m_cur_block_info.out;
	int hu_count = 0;
	bool isinting = check_in_ting(hu_count);
	if (m_cur_block_info.card_value == MJ_CARD_ARROW_HZ) //���в�������
	{
		param.opreate_type = BLOCK_TYPE_GAVE;
	}
	else if (isinting) //��������£���Ҫ�ж������Ƿ��и�����ƺ�
	{
		int after_hucount = 0;
		int has_count = 0;
		check_in_ting_after_penggang(m_cur_block_info.card_value, after_hucount, has_count);
		if (after_hucount >= hu_count)
		{
			if (has_count==2)
			{
				param.opreate_type = BLOCK_TYPE_PENG;
				param.card_index[0] = m_user_handcard.OnGetCardPos(m_cur_block_info.card_value);
				param.card_index[1] = param.card_index[0] + 1;
				param.card_index[2] = param.card_index[0] + 2;
				param.card_value[0] = m_cur_block_info.card_value;
				param.card_value[1] = m_cur_block_info.card_value;
				param.card_value[2] = m_cur_block_info.card_value;
			}
			else if (has_count == 3)
			{
				param.opreate_type = BLOCK_TYPE_GANG;
				param.card_index[0] = m_user_handcard.OnGetCardPos(m_cur_block_info.card_value);
				param.card_index[1] = param.card_index[0] + 1;
				param.card_index[2] = param.card_index[0] + 2;
				param.card_index[3] = param.card_index[0] + 3;

				param.card_value[0] = m_cur_block_info.card_value;
				param.card_value[1] = m_cur_block_info.card_value;
				param.card_value[2] = m_cur_block_info.card_value;
				param.card_value[3] = m_cur_block_info.card_value;
			}
			
		}


	}
	else if (!isinting && (m_cur_block_info.block_level & BLOCK_TYPE_GANG) >0) //û���ƵĻ����ܸܾ͸�
	{
		param.opreate_type = BLOCK_TYPE_GANG;
		param.card_index[0] = m_user_handcard.OnGetCardPos(m_cur_block_info.card_value);
		param.card_index[1] = param.card_index[0] + 1;
		param.card_index[2] = param.card_index[0] + 2;
		param.card_index[3] = param.card_index[0] + 3;

		param.card_value[0] = m_cur_block_info.card_value;
		param.card_value[1] = m_cur_block_info.card_value;
		param.card_value[2] = m_cur_block_info.card_value;
		param.card_value[3] = m_cur_block_info.card_value;
	}
	else if (!isinting && (m_cur_block_info.block_level& BLOCK_TYPE_PENG)>0) // ��������
	{
		if (check_need_peng(m_cur_block_info.card_value))
		{//�������Ч�������½�
			param.opreate_type = BLOCK_TYPE_PENG;
			param.card_index[0] = m_user_handcard.OnGetCardPos(m_cur_block_info.card_value);
			param.card_index[1] = param.card_index[0] + 1;
			param.card_index[2] = param.card_index[0] + 2;
			param.card_index[3] = 0x00;

			param.card_value[0] = m_cur_block_info.card_value;
			param.card_value[1] = m_cur_block_info.card_value;
			param.card_value[2] = m_cur_block_info.card_value;
			param.card_value[3] = 0x00;
		}
		else
		{
			param.opreate_type = BLOCK_TYPE_GAVE;
		}
	}
	BASIC_LOG_INFO("�������ƴ�����Ϣ");
	m_user_mgr->send_msg(CTS_RECV_BLOCK_OPERATE, (char*)&param, sizeof(user_block_operate));
	
}

int gamelogic::find_best_out_card()
{
	int index = 0;
	CMJHandCard temphandcards;
	temphandcards.OnAddHandCards(m_user_handcard);
	temphandcards.CardSort();

	//�ҳ���ѵ���
	unsigned char tempvalue = find_best_out_single_card(temphandcards);
	if (tempvalue!=0x00)
	{
		BASIC_LOG_INFO("��ѵ���Ϊ%s", findcard(tempvalue).c_str());
	}


	//���û�е��ƣ��ҳ���Ѵ�
	if (tempvalue == 0x00)
	{
		tempvalue = find_best_out_other_card(temphandcards);
		BASIC_LOG_INFO("��Ѵ�%s,", findcard(tempvalue).c_str());
	}
	if (tempvalue == 0x00)
	{
		BASIC_LOG_ERROR("û���ҵ���Ѵ򷨣��㷨����,������һ������%s", findcard(tempvalue).c_str());
		return m_user_handcard.GetHandCardLength() - 1;
	}

	return	m_user_handcard.OnGetCardPos(tempvalue);
}

unsigned char gamelogic::find_best_out_single_card(CMJHandCard& temphandcards)
{
	unsigned char temp = MJ_CARD_EMPTY;
	unsigned char temphandcard[HAND_CARD_COUNT];
	memset(temphandcard, 0 , sizeof(temphandcard));
	temphandcards.GetAllHandCard(temphandcard);
	unsigned char max_out_count = 0x00;

	for (int i = 0; i < HAND_CARD_COUNT;++i)
	{
		if (temphandcard[i] == MJ_CARD_EMPTY || temphandcard[i] == MJ_CARD_ARROW_HZ)
			continue;
		if (temphandcards.OnGetCardCount(temphandcard[i]) > 1)
			continue;

		unsigned char color = (temphandcard[i] & MASK_COLOR) >> 4;
		unsigned char value = temphandcard[i] & MASK_VALUE;
		unsigned char color_before = 0xFF;
		unsigned char color_next = 0xFF;
		unsigned char value_before = 0xFF;
		unsigned char value_next = 0xFF;

		if (i - 1 >= 0)
		{
			color_before = (temphandcard[i - 1] & MASK_COLOR) >> 4;
			value_before = (temphandcard[i - 1] & MASK_VALUE);
		}
		if (i + 1 < HAND_CARD_COUNT)
		{
			color_next = (temphandcard[i + 1] & MASK_COLOR) >> 4;
			value_next = (temphandcard[i + 1] & MASK_VALUE);
		}

		if (color_before == color)
		{
			if ((value_before + 1) == value ||  (value_before + 2) ==value)
			{
				continue;
			}
		}
		if (color_next == color)
		{
			if (value == (value_next - 1) || value == (value_next -2))
			{
				continue;
			}
		}
		
		if (m_has_out[temphandcard[i]] >= max_out_count)
		{
			max_out_count = m_has_out[temphandcard[i]];
			temp = temphandcard[i];
		}
		
	}
	return temp;
}

unsigned char gamelogic::find_best_out_other_card(CMJHandCard& temphandcards)
{
	unsigned char temp = MJ_CARD_EMPTY;
	int max_win_count = 1;

	bool	bTileFalg[CARD_INDEX_NUM];
	memset(bTileFalg, 0, sizeof(bTileFalg));

	for (int i = 0; i < HAND_CARD_COUNT;++i)
	{
		unsigned char temphandcard[HAND_CARD_COUNT] = { 0x00 };
		unsigned int  temp_card_count = temphandcards.GetAllHandCard(temphandcard);
		unsigned char curcard = temphandcard[i];

		if (curcard == 0x00)
			continue;
		if (bTileFalg[curcard])
			continue;
		bTileFalg[curcard] = true;
		m_logic.del_card_value(temphandcard, temp_card_count, curcard, 1);

		unsigned char	temp_win_card[MAX_OUT_CARD] = { 0x00 };
		int  temp_max_win_count = 0;
		if (m_logic.is_ting(temphandcard, temp_card_count - 1, temp_win_card, false) == false) //ȥ��һ����֮�����Ƿ�������
		{
			continue;
		}
		else
		{
			for (int j = 0; j < MAX_OUT_CARD;++j)
			{
				if (temp_win_card[j] == 0x00)
				{
					continue;
				}
				else
				{
					temp_max_win_count += 4 - m_has_out[temp_win_card[j]] - (int)temphandcards.OnGetCardCount(temp_win_card[j]);
					//BASIC_LOG_INFO("��Ҵ��%s,���Ժ�%s,�������Ѿ���%d��,������%d��"
					//	, findcard(curcard).c_str(), findcard(temp_win_card[j]).c_str()
					//	, m_has_out[temp_win_card[j]], (int)temphandcards.OnGetCardCount(temp_win_card[j]));
				}
			}

			if (temp_max_win_count < 0)
			{
				temp_max_win_count = 0;
			}
			BASIC_LOG_INFO("��Ҵ������ƽ׶Σ������ %s�Ļ��� ���ܹ��� %d ���ƿ��Ժ�", findcard(curcard).c_str(), (int)(temp_max_win_count));
			if (temp_max_win_count >= max_win_count)
			{
				max_win_count = temp_max_win_count;
				temp = curcard;
			}
		}
	}
	if (temp!=0x00)
	{
		BASIC_LOG_INFO("�������ƽ׶Σ������ѷ���:%s",findcard(temp).c_str());
		return temp;
	}

	if (temp == 0x00) //����û�������ƽ׶�
	{
		unsigned char temphandcard[HAND_CARD_COUNT];
		memset(temphandcard, 0, sizeof(temphandcard));
		unsigned int	temp_card_count = temphandcards.GetAllHandCard(temphandcard);
		int min_left_cout = 8;
		for (int i = 0; i < HAND_CARD_COUNT; ++i)
		{
			if (temphandcard[i] == MJ_CARD_EMPTY || temphandcard[i] == MJ_CARD_ARROW_HZ)
				continue;

			unsigned char color = (temphandcard[i] & MASK_COLOR) >> 4;
			unsigned char value = temphandcard[i] & MASK_VALUE;

			unsigned char color_before = 0xFF;
			unsigned char value_before = 0xFF;
			unsigned char color_next = 0xFF;
			unsigned char value_next = 0xFF;
			unsigned char color_next_next = 0xFF;
			unsigned char value_next_next = 0xFF;
			int left_count = 0;

			if (i-1>=0)
			{
				color_before = (temphandcard[i-1] & MASK_COLOR) >> 4;
				value_before = temphandcard[i-1] & MASK_VALUE;
			}
			if (i+1<HAND_CARD_COUNT)
			{
				color_next = (temphandcard[i + 1] & MASK_COLOR) >> 4;
				value_next = temphandcard[i + 1] & MASK_VALUE;
			}
			if (i+2<HAND_CARD_COUNT)
			{
				color_next_next = (temphandcard[i + 2] & MASK_COLOR) >> 4;
				value_next_next = temphandcard[i + 2] & MASK_VALUE;
			}

			//˳�Ӳ���
			if (color == color_next && color == color_next_next 
				&& value == (value_next-1)  && value == (value_next_next-2))
			{
				i = i + 2;
				continue;
			}

			//���Ӳ���
			if (color == color_next && color == color_next_next
				&& value == value_next && value == value_next_next)
			{
				i = i + 2;
				continue;
			}
			//�����һ���ƿ��γ�˳��
			if (color == color_next && (value == value_next -1))
			{
				if (value == 0x01)
				{ 
					int temp = 4 - m_has_out[temphandcard[i] + 2] - m_user_handcard.OnGetCardCount(temphandcard[i] + 2);
					left_count += temp>0?temp:0;
				}
				else if (value == 0x08)
				{
					int temp = 4 - m_has_out[temphandcard[i] - 1] - m_user_handcard.OnGetCardCount(temphandcard[i] - 1);
					left_count += temp>0 ? temp : 0;
				}
				else
				{
					int temp = 4 - m_has_out[temphandcard[i] + 2] - m_user_handcard.OnGetCardCount(temphandcard[i] + 2);
					left_count += temp>0 ? temp : 0;
					temp = 4 - m_has_out[temphandcard[i] - 1] - m_user_handcard.OnGetCardCount(temphandcard[i] - 1);
					left_count += temp>0 ? temp : 0;
				}
			}
			//�����һ���ƿ����γɿ���
			if (color == color_next && value == value_next)
			{
				int temp = 4 - m_has_out[temphandcard[i]] - m_user_handcard.OnGetCardCount(temphandcard[i]);
				left_count += temp>0 ? temp : 0;
			}
			//��ǰ��һ���ƿ����γ�˳��
			if (color_before == color && ((value_before+1) ==value))
			{
				if (value == 0x09)
				{
					int temp = 4 - m_has_out[temphandcard[i] - 2] - m_user_handcard.OnGetCardCount(temphandcard[i] - 2);
					left_count += temp>0 ? temp : 0;
				}
				else if (value == 0x02)
				{
					int temp = 4 - m_has_out[temphandcard[i] + 1] - m_user_handcard.OnGetCardCount(temphandcard[i] + 1);
					left_count += temp>0 ? temp : 0;
				}
				else
				{
					int temp = 4 - m_has_out[temphandcard[i] - 2] - m_user_handcard.OnGetCardCount(temphandcard[i] - 2);
					left_count += temp>0 ? temp : 0;
					temp =  4 - m_has_out[temphandcard[i] + 1] - m_user_handcard.OnGetCardCount(temphandcard[i] + 1);
					left_count += temp>0 ? temp : 0;
				}
			}
			//��ǰ��һ���ƿ����γɿ���
			if (color_before == color && value_before == value)
			{
				int temp = 4 - m_has_out[temphandcard[i]] - m_user_handcard.OnGetCardCount(temphandcard[i]);
				left_count += temp>0 ? temp : 0;
			}

			if (left_count<=min_left_cout)
			{
				min_left_cout = left_count;
				temp = temphandcard[i];
			}
		}
	}
	return temp;
}


void gamelogic::deal_send_hu_msg()
{
	user_block_operate param;
	param.chair_id = m_base_info.my_station;
	param.out = m_cur_block_info.out;
	param.opreate_type = BLOCK_TYPE_HU;
	param.block_index = m_cur_block_info.block_index;
	m_user_mgr->send_msg(CTS_RECV_BLOCK_OPERATE, (char*)&param, sizeof(user_block_operate));
}

bool gamelogic::check_in_ting(int& hu_count)
{
	unsigned char  temp_handcard[HAND_CARD_COUNT] = { 0 };
	unsigned int	temp_card_count = m_user_handcard.GetAllHandCard(temp_handcard);
	unsigned char	temp_win_card[MAX_OUT_CARD] = { 0x00 };
	if (m_logic.is_ting(temp_handcard, temp_card_count, temp_win_card, false) == false)
	{
		return false;
	}
	for (int i = 0; i < MAX_OUT_CARD;i++)
	{
		if (temp_win_card[i] != 0x00)
		{
			hu_count += 4 - m_has_out[temp_win_card[i]] - m_user_handcard.OnGetCardCount(temp_win_card[i]);
		}
	}
	return true;
}

bool gamelogic::check_in_ting_after_penggang(unsigned char card_value, int& after_hu_count,int& has_count)
{
	//ɾ���������
	CMJHandCard handcards_after_del_cv;
	handcards_after_del_cv.OnAddHandCards(m_user_handcard);
	int del_count = handcards_after_del_cv.OnGetCardCount(card_value);
	has_count = del_count;
	handcards_after_del_cv.OnDelHandCard(card_value, del_count);
	if (del_count == 2)
	{
		bool	bTileFalg[CARD_INDEX_NUM];
		memset(bTileFalg, 0, sizeof(bTileFalg));

		for (int i = 0; i < HAND_CARD_COUNT; ++i)
		{
			unsigned char temphandcard[HAND_CARD_COUNT] = { 0x00 };
			unsigned int  temp_card_count = handcards_after_del_cv.GetAllHandCard(temphandcard);
			unsigned char curcard = temphandcard[i];

			if (curcard == 0x00)
				continue;
			if (bTileFalg[curcard])
				continue;
			bTileFalg[curcard] = true;
			m_logic.del_card_value(temphandcard, temp_card_count, curcard, 1);

			unsigned char	temp_win_card[MAX_OUT_CARD] = { 0x00 };
			int  temp_max_win_count = 0;
			if (m_logic.is_ting(temphandcard, temp_card_count - 1, temp_win_card, false) == false) //ȥ��һ����֮�����Ƿ�������
			{
				continue;
			}
			else
			{
				for (int j = 0; j < MAX_OUT_CARD; ++j)
				{
					if (temp_win_card[j] == 0x00)
					{
						continue;
					}
					else
					{
						temp_max_win_count += 4 - m_has_out[temp_win_card[j]] - (int)handcards_after_del_cv.OnGetCardCount(temp_win_card[j]);
						//BASIC_LOG_INFO("��Ҵ��%s,���Ժ�%s,�������Ѿ���%d��,������%d��"
						//	, findcard(curcard).c_str(), findcard(temp_win_card[j]).c_str()
						//	, m_has_out[temp_win_card[j]], (int)temphandcards.OnGetCardCount(temp_win_card[j]));
					}
				}

				if (temp_max_win_count < 0)
				{
					temp_max_win_count = 0;
				}
				if (temp_max_win_count >= after_hu_count)
				{
					after_hu_count = temp_max_win_count;
				}
			}
		}
	}
	else if (del_count == 3) //ɾ���ܵ����
	{
		if (handcards_after_del_cv.OnGetCardCount(MJ_CARD_ARROW_HZ) >= 2)
		{
			after_hu_count = 100; //�������еĻ���ȥ��
		}
	}
	return true;
}

bool gamelogic::check_need_peng(unsigned char card_value)
{
	//ɾ���������
	CMJHandCard handcards_after_del_cv;
	handcards_after_del_cv.OnAddHandCards(m_user_handcard);
	int del_count = handcards_after_del_cv.OnGetCardCount(card_value);
	if (del_count == 2)
	{
		unsigned char value = card_value&MASK_VALUE;
		int count = 0;
		if (value == 1)
		{
			if (handcards_after_del_cv.OnGetCardCount(card_value + 1)>0 && handcards_after_del_cv.OnGetCardCount(card_value+2)>0)
			{
				count++;
			}
		}
		else if (value == 2)
		{
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1)>0 && handcards_after_del_cv.OnGetCardCount(card_value + 1)>0)
			{
				count++;
			}
			if (handcards_after_del_cv.OnGetCardCount(card_value + 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value + 2) > 0)
			{
				count++;
			}
		}
		else if (value == 8)
		{
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value + 1) > 0)
			{
				count++;
			}
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value - 2) > 0)
			{
				count++;
			}
		}
		else if (value == 9)
		{
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value - 2) > 0)
			{
				count++;
			}
		}
		else
		{
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value - 2) > 0)
			{
				count++;
			}
			if (handcards_after_del_cv.OnGetCardCount(card_value - 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value + 1) > 0)
			{
				count++;
			}
			if (handcards_after_del_cv.OnGetCardCount(card_value + 1) > 0 && handcards_after_del_cv.OnGetCardCount(card_value + 2) > 0)
			{
				count++;
			}
		}

		if (count>=1)
		{
			return false;
		}
	}
	return true;
}

void gamelogic::deal_send_gang_msg()
{
	user_block_operate param;
	param.chair_id = m_base_info.my_station;
	param.out = m_cur_block_info.out;
	param.block_index = m_cur_block_info.block_index;


	BASIC_LOG_INFO("��ǰ���Ƶ����ǣ�%s",findcard(m_cur_block_info.card_value).c_str());


	BASIC_LOG_INFO("������������%s  %s  %s   %s ", findcard(m_cur_block_info.block_info[0][1]).c_str(), 
		findcard(m_cur_block_info.block_info[0][2]).c_str(),
		findcard(m_cur_block_info.block_info[0][3]).c_str(), findcard(m_cur_block_info.block_info[0][4]).c_str());

	param.opreate_type = BLOCK_TYPE_GANG;
	for (int i = 0; i < m_user_handcard.GetHandCardLength(); ++i)
	{
		unsigned char curcard = m_user_handcard.OnGetHandCard(i);
		if (m_user_handcard.OnGetCardCount(curcard) == 4)
		{
			if (curcard == MJ_CARD_ARROW_HZ)
			{
				BASIC_LOG_INFO("���в����а���");
				param.opreate_type = BLOCK_TYPE_GAVE;
				break;
			}
			param.card_index[0] = i;
			param.card_index[1] = i + 1;
			param.card_index[2] = i + 2;
			param.card_index[3] = i + 3;

			param.card_value[0] = m_user_handcard.OnGetHandCard(i);
			param.card_value[1] = m_user_handcard.OnGetHandCard(i);
			param.card_value[2] = m_user_handcard.OnGetHandCard(i);
			param.card_value[3] = m_user_handcard.OnGetHandCard(i);
			break;
		}
		else if (m_user_handcard.OnGetCardCount(curcard) == 1 && curcard == m_cur_block_info.block_info[0][1])
		{
			if (curcard == MJ_CARD_ARROW_HZ)
			{
				BASIC_LOG_INFO("���в����а���");
				param.opreate_type = BLOCK_TYPE_GAVE;
				break;
			}
			param.card_index[0] = i;
			param.card_value[0] = m_user_handcard.OnGetHandCard(i);
			break;
		}
	}
	
	m_user_mgr->send_msg(CTS_RECV_BLOCK_OPERATE, (char*)&param, sizeof(user_block_operate));

}

void gamelogic::set_mj_logic()
{
	for (int i = 0; i < 9; i++)
	{

		m_logic.add_normal_card(MJ_CARD_WAN_1 + i);
	}

	for (int i = 0; i < 9; i++)
	{

		m_logic.add_normal_card(MJ_CARD_TIAO_1 + i);
	}

	for (int i = 0; i < 9; i++)
	{
		m_logic.add_normal_card(MJ_CARD_TONG_1 + i);
	}


	m_logic.add_normal_card(MJ_CARD_ARROW_HZ);
	m_logic.set_king_over_win(4);
	m_logic.m_allow_laizi = true;
}

void gamelogic::init_for_game()
{
	kill_timer(TIME_USER_READY_CHECK_START);
	kill_timer(TIME_USER_HZMJ_LEFT_DESK);
	m_rabot_ready_time = 0;
	memset(m_has_out, 0, sizeof(m_has_out));
	m_cur_block_info.cleardata();
}

int gamelogic::get_ready_time(notify_game_result* ptr)
{
	int length = 0;
	int time_view = 0;
	for (int j = 0; j < PLAY_COUNT; j++)
	{
		if (ptr->win[j])
		{
			for (int k = 0; k < 108; k++)
			{
				if (ptr->luck_card[k] != 0)
				{
					length++;
				}
			}
			break;
		}
	}

	if (length > 8)
	{
		time_view = length / (float)15 + 2;
	}
	else
	{
		if (1 == length)
		{
			time_view = 4;
		}
		else
		{
			time_view = length / (float)2 + 2;
		}

	}
	return time_view;
}


void gamelogic::printhandcard(unsigned char chair_id)
{
	std::string temp = "";
	switch (chair_id)
	{
	case 0:
	{
		temp = "0000000000000";
	}
		break;
	case 1:
	{
		temp = "1111111111111";
	}
		break;
	case 2:
	{
		temp = "2222222222222";
	}
		break;
	case 3:
	{
		temp = "3333333333333";
	}
		break;
	default:
		break;
	}

	temp+= "��ǰ��:";

	BYTE temphand[HAND_CARD_COUNT] = { 0 };
	m_user_handcard.GetAllHandCard(temphand);

	for (int i = 0; i < m_user_handcard.GetHandCardLength();++i)
	{
		temp +=std::to_string(i) + ":" + findcard(temphand[i]) + "  ";
	}
	BASIC_LOG_INFO(temp.c_str());
}

std::string gamelogic::findcard(unsigned char value)
{
	switch (value)
	{
	case MJ_CARD_WAN_1:
		return "һ��";
	case MJ_CARD_WAN_2:
		return "����";
	case MJ_CARD_WAN_3:
		return "����";
	case MJ_CARD_WAN_4:
		return "����";
	case MJ_CARD_WAN_5:
		return "����";
	case MJ_CARD_WAN_6:
		return "����";
	case MJ_CARD_WAN_7:
		return "����";
	case MJ_CARD_WAN_8:
		return "����";
	case MJ_CARD_WAN_9:
		return "����";
	case MJ_CARD_TIAO_1:
		return "һ��";
	case MJ_CARD_TIAO_2:
		return "����";
	case MJ_CARD_TIAO_3:
		return "����";
	case MJ_CARD_TIAO_4:
		return "����";
	case MJ_CARD_TIAO_5:
		return "����";
	case MJ_CARD_TIAO_6:
		return "����";
	case MJ_CARD_TIAO_7:
		return "����";
	case MJ_CARD_TIAO_8:
		return "����";
	case MJ_CARD_TIAO_9:
		return "����";
	case MJ_CARD_TONG_1:
		return "һͲ";
	case MJ_CARD_TONG_2:
		return "��Ͳ";
	case MJ_CARD_TONG_3:
		return "��Ͳ";
	case MJ_CARD_TONG_4:
		return "��Ͳ";
	case MJ_CARD_TONG_5:
		return "��Ͳ";
	case MJ_CARD_TONG_6:
		return "��Ͳ";
	case MJ_CARD_TONG_7:
		return "��Ͳ";
	case MJ_CARD_TONG_8:
		return "��Ͳ";
	case MJ_CARD_TONG_9:
		return "��Ͳ";
	case MJ_CARD_ARROW_HZ:
		return "����";
	default:
		break;
	}
	return "��";
}

