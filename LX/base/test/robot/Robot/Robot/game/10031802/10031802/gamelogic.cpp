#include "gamelogic.h"
#include "basiclog\basicloginterface.h"
#include <random>

//����㷨
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
	//BASIC_LOG_INIT("dachuan/log/10031800_AI.log");
	memset(m_chip, 0, sizeof(m_chip));
	memset(m_seat_userid, 0, sizeof(m_seat_userid));
	m_usermoney = 0;
	m_chip_num = 0;
	m_chip_area = 255;
	m_myPos = 255;
	m_bfirst = false;
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
	case DC_ASS_GR_GAMEINFOR:
	{
		sc_game_base_info* param = reinterpret_cast<sc_game_base_info *>(buf);
		m_usermoney = param->total_score;
		m_myPos = param->chair_id;
		m_bfirst = true;
		memcpy(m_chip, param->chouma, sizeof(m_chip));
		break;
	}
	case STC_SEND_NOTIFY_OUT:
	{
		deal_send_xiazhu_info(buf, len);
		break;
	}
	case STC_SEND_GAME_RESULT:
	{
		BASIC_LOG_INFO
			("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  �ƣ� ��Ϸ������׼����ʼ ",
			m_user_mgr, sub_id, code_id
			);
		notify_game_result* param = reinterpret_cast<notify_game_result *>(buf);
		m_bfirst = false;
		m_usermoney = param->total_score;
		m_chip_area = 255;
		m_rabot_state = 0;
		m_chip_num = 0;
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
		if (m_user_mgr->m_offline_flag)
		{
			BASIC_LOG_INFO("gamenotify  ���߱�־����ǰ����û�л����˻��������������id = %d �����뿪", m_user_mgr->m_my_userid);
			m_usermoney = 0;
			set_timer(TIME_USER_LEFT_DESK, 1000);
		}
		else
		{
			BASIC_LOG_INFO("gamenotify  m_user_mgr=%0x  sub_id=%d   code_id=%d  ������ ,��ǰ���� = %d ���������� = %d,  ���=%d,��С=%d", m_user_mgr, sub_id, code_id, game_count, m_user_mgr->m_play_game_count, m_user_mgr->m_play_game_count_max, m_user_mgr->m_play_game_count_min);
			if (game_count >= m_user_mgr->m_play_game_count)
			{//�������������뿪
				game_count = 0;
				m_usermoney = 0;
				set_timer(TIME_USER_LEFT_DESK, 1000);
				BASIC_LOG_INFO("gamenotify �������������뿪  m_user_mgr->m_my_userid= %d", m_user_mgr->m_my_userid);
			}
		}
		break;
	}
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
			m_user_mgr->m_user_action_mgr->user_ask_left(user_id);
		}
	}
		break;
	case TIME_USER_LHD_XIAZHU:
	{
		kill_timer(event_id);
		get_user_chips();
		BASIC_LOG_INFO("������ user_id = %d ��ע m_xia_zhu_param.chairid = %d, m_xia_zhu_param.xiazhumoney = %d m_xia_zhu_param.areaid = %d", user_id, m_xia_zhu_param.chair_id, m_xia_zhu_param.xiazhumoney, m_xia_zhu_param.area);
		m_user_mgr->send_msg(CTS_RECV_OUT_CARD, (char*)&m_xia_zhu_param, sizeof(user_out_chip_operate));
		m_chip_num--;
		if (m_chip_num > 0)
		{//ÿ����һ��ע
			int ivalue = GetRandValue(1000, 3000);
			set_timer(TIME_USER_LHD_XIAZHU, ivalue);
		}
		break;
	}
	case TIME_USER_LEFT_DESK:
	{
		kill_timer(event_id);
		m_user_mgr->m_user_action_mgr->user_ask_left(m_user_mgr->m_my_userid);
		break;
	}
	default:
		break;
	}
}

void gamelogic::deal_send_change_desk(unsigned int chair_id)
{
	m_user_mgr->set_swap_desk(true);
	m_user_mgr->user_change_desk(chair_id);
}

//�����ע
void   gamelogic::deal_send_xiazhu_info(char* buf, int len)
{
	user_out_chip_operate* param = reinterpret_cast<user_out_chip_operate*>(buf);
	if (param != NULL)
	{
		unsigned int xiazhuTime = GetRandValue(1000, 5000);			//��עʱ�����ֵ
		m_chip_num = GetRandValue(1, 3);							//�����ע����

		//BASIC_LOG_INFO("������  m_user_mgr->m_my_userid = %d ��ע��ʱ��  = %d deal_send_xiazhu_info ", m_user_mgr->m_my_userid, xiazhuTime);
		set_timer(TIME_USER_LHD_XIAZHU, xiazhuTime);
	}

}
//��ȡ�����עֵ
void gamelogic::get_user_chips()
{
	user_out_chip_operate c_param;
	srand(time(0) + m_user_mgr->m_my_userid);
	unsigned int notenum = 0;
	for (int i = 0; i < CHOUMA_COUNT; i++)
	{
		if (m_usermoney > m_chip[i])
		{
			notenum++;
		}
	}
	BYTE notearea = 0;										//��ע�������ֵ
	BYTE notevalue = 0;										//��ע�������ֵ
	if (m_chip_area >= AREA_COUNT)
	{
		notearea = GetRandValue(0, AREA_COUNT - 1);
		m_chip_area = notearea;
	}
	else
	{
		notearea = m_chip_area;
	}
	if (notenum > 0)
	{
		notevalue = GetRandValue(0, notenum - 1);
		if (notenum >= 3 && notevalue >= notenum - 1)
		{//�жϸ߳���ֵʱ���״�� ����ҽ�ҵ����ڳ����2��ʱ���һ��������ע
			if (m_usermoney < m_chip[notevalue] * 2)
			{
				notevalue--;
			}
		}

		c_param.chair_id = m_user_mgr->m_my_station;
		c_param.area = notearea;
		c_param.xiazhumoney = m_chip[notevalue];
		memcpy(&m_xia_zhu_param, &c_param, sizeof(user_out_chip_operate));
	}
}

