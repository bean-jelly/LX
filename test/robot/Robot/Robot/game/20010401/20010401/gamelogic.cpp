#include "gamelogic.h"
#include "basiclog\basicloginterface.h"
#include <vector>
#include <random>

#define random(a,b) (rand()%(b-a+1)+a)

static const unsigned char m_CardArray[ALL_CARD_COUNT] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 A - K
	0x4E, 0x4F																	  //小王，大王
};

//随机算法
static int GetRandValue(int iLeft, int iRight)
{
	if (iLeft > iRight)
	{
		iRight = iLeft;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(iLeft, iRight);
	return dis(gen);
}

gamelogic::gamelogic()
{
	init_data();
	game_count = 0;
}

gamelogic::~gamelogic()
{

}

bool gamelogic::gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{
	return true;
}

//清理数据
void gamelogic::init_data()
{
	memset(m_has_out, 0, sizeof(m_has_out));
	memset(m_public_card, CARD_INIT, sizeof(m_public_card));
	memset(m_king_card, 0x00, sizeof(m_king_card));
	memset(m_is_done, 0, sizeof(m_is_done));
	memset(m_is_sp235, 0, sizeof(m_is_sp235));
	memset(m_is_three, 0, sizeof(m_is_three));
	m_current_turn = 255;
	m_public_num = 0;
	m_hand_card_num = 0;
	m_have_king = false;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		memset(&m_hand_card[i], CARD_INIT, sizeof(m_hand_card[i]));
		m_placed_card[i].init();
	}
}

bool gamelogic::gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id)
{
	utility::BasicCriticalSection lock(m_game_mutex);
	switch (sub_id)
	{
	case STC_SEND_GAME_RESULT:
	{
		//更新玩家金币
		notify_game_result* game_result_param = reinterpret_cast<notify_game_result*>(buf);
		gold_change gold_change_param;
		gold_change_param.player_id = m_user_mgr->m_my_userid;
		gold_change_param.gold = game_result_param->total_score[m_user_mgr->m_my_station];
		gold_change_param.gold -= m_base_info.gameconfig.tax;

		::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_CHANGE, (LPARAM)&gold_change_param);
	}
	break;
	case STC_SEND_PUBLIC_INFO:
	{
		read_config_ini();
		init_data();

		//发送公共牌
		public_card_info* public_param = reinterpret_cast<public_card_info*>(buf);
		if (public_param != NULL)
		{
			if (public_param->replacecard[0] != CARD_INIT)
			{
				m_have_king = true;
				memcpy(m_king_card, public_param->replacecard, sizeof(m_king_card));
				m_ppl_logic.SetReplaceCard(true);
			}
			memcpy(m_public_card, public_param->public_card, sizeof(m_public_card));
			m_public_num = public_param->card_count;
			for (int i = 0; i < ROW_COUNT; i++)
			{
				m_has_out[check_pos(m_public_card[i])] = 1;
			}
		}
	}
	break;
	case STC_SEND_HAND_CARD:
	{
		//发送手牌
		hand_card_info* handcard_param = reinterpret_cast<hand_card_info*>(buf);
		if (handcard_param != NULL)
		{
			m_hand_card_num = handcard_param->card_count;
			memset(m_hand_card, CARD_INIT, sizeof(m_hand_card));
			memcpy(m_hand_card, handcard_param->hand_card, sizeof(m_hand_card));
			char buf[100] = { 0 };
			sprintf_s(buf, "PPL chairid(%d) turn(%d) get handcard %s, %s, %s, %s",
				m_user_mgr->m_my_station,
				m_current_turn,
				translate(m_hand_card[0]).c_str(),
				translate(m_hand_card[1]).c_str(), 
				translate(m_hand_card[2]).c_str(), 
				translate(m_hand_card[3]).c_str());
			OutputDebugStringA(buf);
		}
		else
		{
			OutputDebugStringA("PPL SendHandCard To Robot Wrong");
		}
	}
	break;
	case STC_NOTIFY_PICK_CARD:
	{
		//被通知开始选牌
		notify_pickcard *notify_param = reinterpret_cast<notify_pickcard*>(buf);
		if (notify_param != NULL)
		{
			m_current_turn = notify_param->current_turn;
			int num_sec = (m_time_out_card2 - m_time_out_card1) / 500;
			num_sec = GetRandValue(0, num_sec);
			int randtime = num_sec * 500 + m_time_out_card1;
			char buf[100] = { 0 };
			set_timer(TIME_USER_PPL_PICK_CARD, randtime);
		}
	}
	break;
	case STC_SEND_ALL_USER_STATUS:
	{
		notify_user_status *status_param = reinterpret_cast<notify_user_status*>(buf);
		if (status_param != NULL)
		{
			if (status_param->user_status[m_user_mgr->m_my_station] == 1)
			{
			}
		}
	}
	case STC_NOTIFY_PICK_RESULT:
	{
		//收到选牌结果
		pickcard_result *result_param = reinterpret_cast<pickcard_result*>(buf);
		if (result_param != NULL)
		{
			unsigned chair_id = result_param->chair_id;
			if (result_param->pick_card != CARD_INIT && m_placed_card[chair_id].card_value[result_param->choose_row][result_param->choose_col] == CARD_INIT)
			{
				m_placed_card[chair_id].card_value[result_param->choose_row][result_param->choose_col] = result_param->pick_card;
				m_has_out[check_pos(result_param->pick_card)] = 1;
			}

			//检查该行是否为杂花235
			unsigned char tmp_list[ROW_COUNT] = { 0 };
			tmp_list[0] = m_public_card[result_param->choose_row];
			for (int col = 0; col < COL_COUNT; col++)
			{
				tmp_list[col + 1] = m_placed_card[chair_id].card_value[result_param->choose_row][col];
			}
			if (m_ppl_logic.CheckTypeSpecial(tmp_list, MAX_COUNT))
			{
				m_is_sp235[result_param->choose_row] = true;
			}
			if (m_ppl_logic.CheckThree(tmp_list, MAX_COUNT))
			{
				m_is_three[result_param->choose_row] = true;
			}
		}
	}
	break;
	case DC_ASS_GR_USER_READY:
	{
		//金币不足消息
		DCGR::MSG_UserAgreeResult* param = reinterpret_cast<DCGR::MSG_UserAgreeResult*>(buf);
		if (param->bSuccess == false && param->iErrCode == agree_err_type_2)
		{
			//金不足，无法准备
			BASIC_LOG_INFO("沧州机器人 金币不足，无法准备 param->dwUserID = %d", param->dwUserID);
			::SendMessage(m_handle, WM_USER + 100, USER_STATE_TO_MFC_GOLD_LACK, (LPARAM)param);
		}
		else if (param->bSuccess == true)
		{
			m_user_mgr->m_user_action_mgr->send_get_game_info();
		}
	}
	break;
	case DC_ASS_GR_GAMEINFOR:
	{
		//得到自己消息
		sc_game_status_ready* param_ready = reinterpret_cast<sc_game_status_ready*>(buf);
		if (param_ready != NULL)
		{
			m_base_info = param_ready->base_info;
		}
		sc_game_status_play* param_play = reinterpret_cast<sc_game_status_play*>(buf);
		if (param_play != NULL)
		{
			m_base_info = param_play->base_info;
		}
	}
	break;
	case STC_SEND_START_COUNT:
	{
		set_start_game *param = reinterpret_cast<set_start_game*>(buf);
		if (param != NULL)
		{
			m_rabot_state = 0;
			game_count++;
			if (game_count > m_user_mgr->m_play_game_count)
			{
				//局数到，请求离开
				game_count = 0;
				m_user_mgr->deal_notify_user_left_from_MFC(false);
			}
		}
	}
	break;
	case STC_SEND_NOTIFY_PREPARE:
	{
		deal_count_socre(buf, len);
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
	kill_timer(event_id);
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
			game_count = 0;
			m_user_mgr->deal_notify_user_left_from_MFC(false);
		}
	}
	break;
	case TIME_USER_PPL_READY:
	{
		kill_timer(TIME_USER_PPL_READY);
		if (m_user_mgr != NULL)
		{
			if (m_user_mgr->OncheckUserready())
			{
				m_user_mgr->user_aready(true);
				kill_timer(TIME_USER_READY_CHECK_START);
				m_user_mgr->set_timer(TIME_USER_READY_CHECK_START, TIME_ONE_SECOND);
			}
			else
			{
				game_count = 0;
				m_user_mgr->deal_notify_user_left_from_MFC(false);
			}
		}
	}
	break;
	case TIME_USER_PPL_PICK_CARD:
	{
		kill_timer(TIME_USER_PPL_PICK_CARD);
		deal_user_out_card();
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
	init_data();
	user_agree param;
	param.bagree = true;
	param.chair_id = m_user_mgr->m_my_station;
	param.user_id = m_user_mgr->m_my_userid;

	m_user_mgr->user_aready(true);
	m_user_mgr->set_timer(TIME_USER_READY_CHECK_START, TIME_ONE_SECOND);
}

void gamelogic::deal_user_out_card()
{
	//找出需要出的牌
	unsigned char pickcard = CARD_INIT;
	int row = 255;
	int col = 255;
	m_ppl_logic.SortCard(m_hand_card, HAND_CARD_NUM);

	find_best_out_card(pickcard, row, col);
	if (pickcard == CARD_INIT || row == 255 || col == 255)
	{
		char buf[100] = { 0 };
		sprintf_s(buf, "PPL charid(%d)wrong card(%s), row(%d), col(%d)", m_user_mgr->m_my_station, translate(pickcard).c_str(), row, col);
		OutputDebugStringA(buf);
		return;
	}

	//发送消息
	pickcard_info param;
	param.chair_id = m_user_mgr->m_my_station;
	param.current_turn = m_current_turn;
	param.pick_card = pickcard;
	param.choose_row = row;
	param.choose_col = col;
	m_user_mgr->send_msg(CTS_UPDATE_USER_PICK, (char*)&param, sizeof(pickcard_info));

	if (pickcard != CARD_INIT &&
		row != 255 &&
		col != 255 &&
		m_placed_card[m_user_mgr->m_my_station].card_value[row][col] == CARD_INIT)
	{
		m_placed_card[m_user_mgr->m_my_station].card_value[row][col] = pickcard;
		m_has_out[check_pos(pickcard)] = 1;

		if (m_placed_card[m_user_mgr->m_my_station].card_value[row][0] != CARD_INIT && m_placed_card[m_user_mgr->m_my_station].card_value[row][1] != CARD_INIT)
		{
			m_is_done[row] = true;
		}
	}
	else
	{
		BASIC_LOG_ERROR("PPL out card wrong charid[%d] row[%d] col[%d] card[%s]", m_user_mgr->m_my_station, row, col, translate(pickcard).c_str());
	}

	char buf[100] = { 0 };
	sprintf_s(buf, "PPL chairid(%d) correct out card(%s), row(%d), col(%d)", m_user_mgr->m_my_station, translate(pickcard).c_str(), row, col);
	OutputDebugStringA(buf);
}

void gamelogic::find_best_out_card(unsigned char &out_card, int &row, int &col)
{
	//检查是否能组合成杂花235
	if (check_combine_235(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_SPEIAL);
		return;
	}

	//检查是否能组合成三条
	if (check_combine_three(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
		return;
	}

	//检查是否能组合成同花顺
	if (check_combine_straightflush(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_STR_FLUSH);
		return;
	}

	//检查是否能组合成同花
	if (check_combine_flush(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_FLUSH);
		return;
	}

	//检查是否能组合成顺子
	if (check_combine_straight(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_STR);
		return;
	}

	//检查是否能组合成对子
	if (check_combine_pair(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_PAIR);
		return;
	}
	
	//检查是否有公共牌为王牌且该行有两个空格
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]) && m_placed_card[m_user_mgr->m_my_station].card_value[r_num][0] != CARD_INIT && m_placed_card[m_user_mgr->m_my_station].card_value[r_num][1] != CARD_INIT)
		{
			out_card = m_hand_card[0];
			find_blank(r_num, row, col);
			return;
		}
	}

	//挑选高牌
	if (check_combine_high(out_card, row, col))
	{
		BASIC_LOG_INFO("PPL 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_HIGH_CARD);
		return;
	}

	return;
}

//检测牌个数（不包含其他玩家手牌）
int gamelogic::CountCardNum(unsigned char card)
{
	int count = 0;

	return count;
}

void gamelogic::printhandcard()
{
	std::string temp = "拼拼乐机器人 当前牌:";

	BASIC_LOG_INFO(temp.c_str());
}

void gamelogic::read_config_ini()
{
	char chpath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, chpath);
	char chPathFile[MAX_PATH] = { 0 };
	sprintf_s(chPathFile, MAX_PATH, "%s\\20010401_AI.ini", chpath);
	std::string strMainKey = "TIME";
	m_time_out_card1 = GetPrivateProfileIntA(strMainKey.c_str(), "time_out_card1", 1000, chPathFile);
	m_time_out_card2 = GetPrivateProfileIntA(strMainKey.c_str(), "time_out_card2", 3000, chPathFile);
}

//查找牌值位置
int	gamelogic::check_pos(unsigned char card)
{
	int res = 255;
	for (int i = 0; i < ALL_CARD_COUNT; i++)
	{
		if (m_CardArray[i] == card)
		{
			res = i;
			break;
		}
	}
	return res;
}

//查找相同权值牌的数量
//应排除手牌中其他的牌
int gamelogic::find_same_value(unsigned char card)
{
	int res = 0;
	for (int i = 0; i < ALL_CARD_COUNT; i++)
	{
		if (m_has_out[i] == 0 && (m_CardArray[i] & UG_VALUE_MASK) == (card & UG_VALUE_MASK) && m_CardArray[i] != card)
		{
			res++;
		}
	}
	if (m_have_king)
	{
		if (m_has_out[ALL_CARD_COUNT - 3] == 0)
		{
			res++;
		}
		if (m_has_out[ALL_CARD_COUNT - 2] == 0)
		{
			res++;
		}
	}
	return res;
}

//查找相同花色牌的数量
int gamelogic::find_same_color(unsigned char card)
{
	int res = 0;
	for (int i = 0; i < ALL_CARD_COUNT; i++)
	{
		if (m_has_out[i] == 0 && (m_CardArray[i] & UG_HUA_MASK) == (card & UG_HUA_MASK) && m_CardArray[i] != card)
		{
			res++;
		}
	}
	if (m_have_king)
	{
		if (m_has_out[ALL_CARD_COUNT - 3] == 0)
		{
			res++;
		}
		if (m_has_out[ALL_CARD_COUNT - 2] == 0)
		{
			res++;
		}
	}
	return res;
}

//检查某行的摆牌是否完成，若没完成则输出随机的空格
bool gamelogic::find_blank(int row, int &out_row, int &out_col)
{
	if (m_is_done[row])
	{
		BASIC_LOG_INFO(" PPL find_blank wrong : row(%d) is done", row);
		return false;
	}
	if (m_placed_card[m_user_mgr->m_my_station].card_value[row][0] == CARD_INIT &&
		m_placed_card[m_user_mgr->m_my_station].card_value[row][1] == CARD_INIT)
	{
		out_row = row;
		out_col = GetRandValue(0, 1);
	}
	else if (m_placed_card[m_user_mgr->m_my_station].card_value[row][0] == CARD_INIT &&
		m_placed_card[m_user_mgr->m_my_station].card_value[row][1] != CARD_INIT)
	{
		out_row = row;
		out_col = 0;
	}
	else if (m_placed_card[m_user_mgr->m_my_station].card_value[row][0] != CARD_INIT &&
		m_placed_card[m_user_mgr->m_my_station].card_value[row][1] == CARD_INIT)
	{
		out_row = row;
		out_col = 1;
	}
	return true;
}

//检查某行的空个数
int gamelogic::find_blank_num(int row)
{
	if (!m_is_done[row])
	{
		if (m_placed_card[m_user_mgr->m_my_station].card_value[row][0] == CARD_INIT &&
			m_placed_card[m_user_mgr->m_my_station].card_value[row][1] == CARD_INIT)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

bool gamelogic::check_combine_235(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;

	unsigned char hand_card[HAND_CARD_NUM] = { 0 };
	memcpy(hand_card, m_hand_card, sizeof(hand_card));
	m_ppl_logic.SortCard(hand_card, MAX_COUNT, true);

	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		if (m_is_done[r_num] || !m_is_three[r_num]) continue;
		if (m_ppl_logic.GetCardNum(m_public_card[r_num]) != 2 &&
			m_ppl_logic.GetCardNum(m_public_card[r_num]) != 3 &&
			m_ppl_logic.GetCardNum(m_public_card[r_num]) != 5)
		{
			continue;
		}
		
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			if (m_ppl_logic.GetCardNum(hand_card[pos_card]) == m_ppl_logic.GetCardNum(m_public_card[r_num]))
			{
				continue;
			}
			if (m_ppl_logic.GetCardNum(hand_card[pos_card]) == 2 ||
				m_ppl_logic.GetCardNum(hand_card[pos_card]) == 3 ||
				m_ppl_logic.GetCardNum(hand_card[pos_card]) == 5)
			{
				int blank_num = find_blank_num(r_num);
				int priority = COL_COUNT - blank_num;
				if (blank_num == 1 && priority >= max_priority)
				{
					unsigned char temp_list[MAX_COUNT] = { 0 };
					temp_list[0] = m_public_card[r_num];
					for (int c_num = 0; c_num < COL_COUNT; c_num++)
					{
						if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
						{
							temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
						}
					}
					temp_list[2] = hand_card[pos_card];
					if (priority == max_priority)
					{
						if (m_ppl_logic.CheckTypeSpecial(temp_list, MAX_COUNT) &&
							m_ppl_logic.CompareCardData(temp_list, list, TYPE_SPEIAL, TYPE_SPEIAL, false) == 1)
						{
							pick_card = hand_card[pos_card];
							memcpy(list, temp_list, sizeof(list));
							max_priority = priority;
							row_res = r_num;
						}
					}
					else
					{
						if (m_ppl_logic.CheckTypeSpecial(temp_list, MAX_COUNT))
						{
							pick_card = hand_card[pos_card];
							memcpy(list, temp_list, sizeof(list));
							max_priority = priority;
							row_res = r_num;
						}
					}
				}
				else if (blank_num == 2 && priority >= max_priority)
				{
					if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]))
					{
						continue;
					}
					unsigned char temp_list[MAX_COUNT] = { 0 };
					temp_list[0] = m_public_card[r_num];
					temp_list[1] = hand_card[pos_card];
					bool flag = false;
					for (int i = 0; i < ALL_CARD_COUNT; i++)
					{
						if (m_has_out[i] == 1 || m_CardArray[i] == hand_card[pos_card])
						{
							continue;
						}
						temp_list[2] = m_CardArray[i];
						unsigned char tl[MAX_COUNT] = { 0 };
						memcpy(tl, temp_list, sizeof(tl));
						if (m_ppl_logic.CheckTypeSpecial(tl, MAX_COUNT))
						{
							flag = true;
							break;
						}
					}
					if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_SPEIAL, TYPE_SPEIAL, false) == 1)
					{
						pick_card = hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_three 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成三条
bool gamelogic::check_combine_three(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		//检测其他玩家是否完成了235
		if (m_is_done[r_num] || m_is_sp235[r_num]) continue;
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			int blank_num = find_blank_num(r_num);
			int priority = COL_COUNT - blank_num;
			//当该行只剩下一个空格时
			if (blank_num == 1 && priority >= max_priority)
			{
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				for (int c_num = 0; c_num < COL_COUNT; c_num++)
				{
					if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
					{
						temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
					}
				}
				temp_list[2] = m_hand_card[pos_card];
				if (priority == max_priority)
				{
					if (m_ppl_logic.CheckThree(temp_list, MAX_COUNT) &&
						m_ppl_logic.CompareCardData(temp_list, list, TYPE_THREE, TYPE_THREE, false) == 1)
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
				else
				{
					if (m_ppl_logic.CheckThree(temp_list, MAX_COUNT))
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
			}
			//当该行有两个空格时
			else if (blank_num == 2 && priority >= max_priority)
			{
				if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]) || find_same_value(m_hand_card[pos_card] == 0))
				{
					continue;
				}
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				temp_list[1] = m_hand_card[pos_card];
				bool flag = false;
				for (int i = 0; i < ALL_CARD_COUNT; i++)
				{
					if (m_has_out[i] == 1 || m_CardArray[i] == m_hand_card[pos_card])
					{
						continue;
					}
					temp_list[2] = m_CardArray[i];
					unsigned char tl[MAX_COUNT] = { 0 };
					memcpy(tl, temp_list, sizeof(tl));
					if (m_ppl_logic.CheckThree(tl, MAX_COUNT))
					{
						flag = true;
						break;
					}
				}
				if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_THREE, TYPE_THREE, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_three 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成同花顺
bool gamelogic::check_combine_straightflush(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			if (m_is_done[r_num]) continue;
			int blank_num = find_blank_num(r_num);
			int priority = COL_COUNT - blank_num;
			//当该行只剩下一个空格时
			if (blank_num == 1 && priority >= max_priority)
			{
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				for (int c_num = 0; c_num < COL_COUNT; c_num++)
				{
					if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
					{
						temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
					}
				}
				temp_list[2] = m_hand_card[pos_card];
				if (priority == max_priority)
				{
					if (m_ppl_logic.CheckStraightFlush(temp_list, MAX_COUNT) &&
						m_ppl_logic.CompareCardData(temp_list, list, TYPE_STR_FLUSH, TYPE_STR_FLUSH, false) == 1)
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
				else
				{
					if (m_ppl_logic.CheckStraightFlush(temp_list, MAX_COUNT))
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
			}
			//当该行有两个空格时
			else if (blank_num == 2 && priority >= max_priority)
			{
				if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]))
				{
					continue;
				}
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				temp_list[1] = m_hand_card[pos_card];
				bool flag = false;
				for (int i = 0; i < ALL_CARD_COUNT; i++)
				{
					if (m_has_out[i] == 1 || m_CardArray[i] == m_hand_card[pos_card])
					{
						continue;
					}
					temp_list[2] = m_CardArray[i];
					unsigned char tl[MAX_COUNT] = { 0 };
					memcpy(tl, temp_list, sizeof(tl));
					if (m_ppl_logic.CheckStraightFlush(tl, MAX_COUNT))
					{
						flag = true;
						break;
					}
				}
				if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_STR_FLUSH, TYPE_STR_FLUSH, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_straightflush 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成同花
bool gamelogic::check_combine_flush(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			if (m_is_done[r_num]) continue;
			int blank_num = find_blank_num(r_num);
			int priority = COL_COUNT - blank_num;
			//当该行只剩下一个空格时
			if (blank_num == 1 && priority >= max_priority)
			{
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				for (int c_num = 0; c_num < COL_COUNT; c_num++)
				{
					if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
					{
						temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
					}
				}
				temp_list[2] = m_hand_card[pos_card];
				if (priority == max_priority)
				{
					if (m_ppl_logic.CheckFlush(temp_list, MAX_COUNT) &&
						m_ppl_logic.CompareCardData(temp_list, list, TYPE_FLUSH, TYPE_FLUSH, false) == 1)
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
				else
				{
					if (m_ppl_logic.CheckFlush(temp_list, MAX_COUNT))
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
			}
			//当该行有两个空格时
			else if (blank_num == 2 && priority >= max_priority)
			{
				if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]))
				{
					continue;
				}
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				temp_list[1] = m_hand_card[pos_card];
				unsigned char flower_color = temp_list[0] & UG_HUA_MASK;
				bool flag = false;
				for (int val = 2; val < 15; val++)
				{
					unsigned char temp_card = 0x00;
					if (val == 14)
					{
						temp_card = flower_color | 0x01;
					}
					else
					{
						temp_card = flower_color | val;
					}
					if (m_has_out[check_pos(temp_card)] == 1 || m_CardArray[check_pos(temp_card)] == m_hand_card[pos_card])
					{
						continue;
					}
					temp_list[2] = m_CardArray[check_pos(temp_card)];
					unsigned char tl[MAX_COUNT] = { 0 };
					memcpy(tl, temp_list, sizeof(tl));
					if (m_ppl_logic.CheckFlush(tl, MAX_COUNT))
					{
						flag = true;
						break;
					}
				}
				if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_FLUSH, TYPE_FLUSH, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_flush 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成顺子
bool gamelogic::check_combine_straight(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			if (m_is_done[r_num]) continue;
			int blank_num = find_blank_num(r_num);
			int priority = COL_COUNT - blank_num;
			//当该行只剩下一个空格时
			if (blank_num == 1 && priority >= max_priority)
			{
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				for (int c_num = 0; c_num < COL_COUNT; c_num++)
				{
					if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
					{
						temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
					}
				}
				temp_list[2] = m_hand_card[pos_card];
				if (priority == max_priority)
				{
					if (m_ppl_logic.CheckStraight(temp_list, MAX_COUNT) &&
						m_ppl_logic.CompareCardData(temp_list, list, TYPE_STR, TYPE_STR, false) == 1)
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
				else
				{
					if (m_ppl_logic.CheckStraight(temp_list, MAX_COUNT))
					{
						pick_card = m_hand_card[pos_card];
						memcpy(list, temp_list, sizeof(list));
						max_priority = priority;
						row_res = r_num;
					}
				}
			}
			//当该行有两个空格时
			else if (blank_num == 2 && priority >= max_priority)
			{
				if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]))
				{
					continue;
				}
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				temp_list[1] = m_hand_card[pos_card];
				bool flag = false;
				for (int i = 0; i < ALL_CARD_COUNT; i++)
				{
					if (m_has_out[i] == 1 || m_CardArray[i] == m_hand_card[pos_card])
					{
						continue;
					}
					temp_list[2] = m_CardArray[i];
					unsigned char tl[MAX_COUNT] = { 0 };
					memcpy(tl, temp_list, sizeof(tl));
					if (m_ppl_logic.CheckStraight(tl, MAX_COUNT))
					{
						flag = true;
						break;
					}
				}
				if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_STR, TYPE_STR, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_straight 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成对子
bool gamelogic::check_combine_pair(unsigned char &out_card, int &row, int &col)
{
	bool res = false;
	int  max_priority = 0;
	unsigned char pick_card = CARD_INIT;
	unsigned char list[MAX_COUNT] = { 0 };
	int row_res = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		for (int pos_card = 0; pos_card < HAND_CARD_NUM; pos_card++)
		{
			if (m_is_done[r_num]) continue;
			int blank_num = find_blank_num(r_num);
			int priority = COL_COUNT - blank_num;
			//当该行只剩下一个空格时
			if (blank_num == 1 && priority >= max_priority)
			{
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				for (int c_num = 0; c_num < COL_COUNT; c_num++)
				{
					if (m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num] != CARD_INIT)
					{
						temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][c_num];
					}
				}
				temp_list[2] = m_hand_card[pos_card];
				if (m_ppl_logic.CheckPair(temp_list, MAX_COUNT) &&
					m_ppl_logic.CompareCardData(temp_list, list, TYPE_PAIR, TYPE_PAIR, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
			//当该行有两个空格时
			else if (blank_num == 2 && priority >= max_priority)
			{
				if (m_ppl_logic.IsReplaceCard(m_public_card[r_num]))
				{
					continue;
				}
				unsigned char temp_list[MAX_COUNT] = { 0 };
				temp_list[0] = m_public_card[r_num];
				temp_list[1] = m_hand_card[pos_card];
				bool flag = false;
				for (int i = 0; i < ALL_CARD_COUNT; i++)
				{
					if (m_has_out[i] == 1 || m_CardArray[i] == m_hand_card[pos_card])
					{
						continue;
					}
					temp_list[2] = m_CardArray[i];
					unsigned char tl[MAX_COUNT] = { 0 };
					memcpy(tl, temp_list, sizeof(tl));
					if (m_ppl_logic.CheckPair(tl, MAX_COUNT))
					{
						flag = true;
						break;
					}
				}
				if (flag && m_ppl_logic.CompareCardData(temp_list, list, TYPE_PAIR, TYPE_PAIR, false) == 1)
				{
					pick_card = m_hand_card[pos_card];
					memcpy(list, temp_list, sizeof(list));
					max_priority = priority;
					row_res = r_num;
				}
			}
		}
	}
	if (pick_card == CARD_INIT)
	{
		return false;
	}
	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_pair 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

//检查是否能组合成高牌
bool gamelogic::check_combine_high(unsigned char &out_card, int &row, int &col)
{
	int  max_priority = 0;
	unsigned char pick_card = m_hand_card[0];
	int score[ROW_COUNT] = { 0 };
	int row_res = 0;

	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		if (m_is_done[r_num]) continue;
		int blank_num = find_blank_num(r_num);
		if (blank_num == 1)
		{
			unsigned char temp_list[MAX_COUNT] = { 0 };
			temp_list[0] = m_public_card[r_num];
			temp_list[1] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][0];
			temp_list[2] = m_placed_card[m_user_mgr->m_my_station].card_value[r_num][1];
			m_ppl_logic.SortCard(temp_list, MAX_COUNT);
			int list_type = TYPE_ERROR;
			for (int i = 0; i < ALL_CARD_COUNT; i++)
			{
				if (m_has_out[i] == 1 || m_CardArray[i] == pick_card)
				{
					continue;
				}
				temp_list[2] = m_CardArray[i];
				unsigned char tl[MAX_COUNT] = { 0 };
				memcpy(tl, temp_list, sizeof(tl));
				if (m_ppl_logic.CheckStraightFlush(tl, MAX_COUNT) && TYPE_STR_FLUSH > list_type)
				{
					list_type = TYPE_STR_FLUSH;
				}
				else if (m_ppl_logic.CheckFlush(tl, MAX_COUNT) && TYPE_FLUSH > list_type)
				{
					list_type = TYPE_FLUSH;
				}
				else if (m_ppl_logic.CheckStraight(tl, MAX_COUNT) && TYPE_STR > list_type)
				{
					list_type = TYPE_STR;
				}
			}
			if (list_type == TYPE_STR_FLUSH)
			{
				score[r_num] = 4;
			}
			else if (list_type == TYPE_FLUSH)
			{
				score[r_num] = 3;
			}
			else if (list_type == TYPE_STR)
			{
				score[r_num] = 2;
			}
			else
			{
				score[r_num] = 1;
			}
		}
		else if (blank_num == 2)
		{
			score[r_num] = 1;
		}
	}

	int s = 0;
	for (int r_num = 0; r_num < ROW_COUNT; r_num++)
	{
		if (score[r_num] == 0)
		{
			continue;
		}
		if (s == 0)
		{
			row_res = r_num;
			s = score[r_num];
			continue;
		}
		else if (score[r_num] < s)
		{
			row_res = r_num;
			s = score[r_num];
		}
	}

	out_card = pick_card;
	find_blank(row_res, row, col);
	BASIC_LOG_INFO("check_combine_high 选择牌%d (%d, %d) 组合牌型%d", out_card, row, col, TYPE_THREE);
	return true;
}

std::string gamelogic::translate(unsigned char card)
{
	std::string value_str = "";
	std::string color_str = "";
	if (card == CARD_INIT)
	{
		value_str = "空";
	}
	else if (m_ppl_logic.GetCardNum(card) == 2)
	{
		value_str = "2";
	}
	else if (m_ppl_logic.GetCardNum(card) == 3)
	{
		value_str = "3";
	}
	else if (m_ppl_logic.GetCardNum(card) == 4)
	{
		value_str = "4";
	}
	else if (m_ppl_logic.GetCardNum(card) == 5)
	{
		value_str = "5";
	}
	else if (m_ppl_logic.GetCardNum(card) == 6)
	{
		value_str = "6";
	}
	else if (m_ppl_logic.GetCardNum(card) == 7)
	{
		value_str = "7";
	}
	else if (m_ppl_logic.GetCardNum(card) == 8)
	{
		value_str = "8";
	}
	else if (m_ppl_logic.GetCardNum(card) == 9)
	{
		value_str = "9";
	}
	else if (m_ppl_logic.GetCardNum(card) == 10)
	{
		value_str = "10";
	}
	else if (m_ppl_logic.GetCardNum(card) == 11)
	{
		value_str = "J";
	}
	else if (m_ppl_logic.GetCardNum(card) == 12)
	{
		value_str = "Q";
	}
	else if (m_ppl_logic.GetCardNum(card) == 13)
	{
		value_str += "K";
	}
	else if (m_ppl_logic.GetCardNum(card) == 14)
	{
		value_str += "A";
	}
	else
	{
		value_str = "王";
		return value_str;
	}
	
	if ((card & UG_HUA_MASK) >> 4 == 0x00)
	{
		color_str = "方块";
	}
	else if ((card & UG_HUA_MASK) >> 4 == 0x01)
	{
		color_str = "梅花";
	}
	else if ((card & UG_HUA_MASK) >> 4 == 0x02)
	{
		color_str = "红心";
	}
	else if ((card & UG_HUA_MASK) >> 4 == 0x03)
	{
		color_str = "黑桃";
	}
	return color_str + value_str;
}